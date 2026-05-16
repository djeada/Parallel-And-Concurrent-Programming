/*
 * Transferable Objects — Zero-Copy vs Structured Clone
 *
 * By default, postMessage() deep-clones its payload (structured clone).
 * For large ArrayBuffers this copy is expensive.  Instead, you can
 * *transfer* an ArrayBuffer: ownership moves to the receiver, and the
 * sender's buffer becomes detached (byteLength === 0) — no copy needed.
 *
 * Three memory-sharing mechanisms compared:
 *
 *   Structured clone  — default; data is copied into a new buffer
 *   Transfer          — ownership moves; sender loses access (detached)
 *   SharedArrayBuffer — truly shared; both sides see the same bytes
 *
 * Key rules:
 *   - Include the buffer in the *transfer list* (second arg of postMessage)
 *     to trigger transfer rather than clone.
 *   - A transferred buffer is detached in the sending context immediately
 *     after postMessage returns.
 *   - SharedArrayBuffer is not transferable; it is shared and never detached.
 *   - Node Buffers backed by the same ArrayBuffer pool share bytes with
 *     other Buffers — only transfer an ArrayBuffer you own exclusively.
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const BUFFER_SIZE = 4 * 1024 * 1024; // 4 MB

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== Transferable Objects Demo ===\n");
  console.log(`Buffer size: ${BUFFER_SIZE / 1024 / 1024} MB each\n`);

  let done = 0;
  const check = () => { if (++done === 3) console.log("\n=== Demo complete ==="); };

  // ── Demo 1: Structured clone (default) ──────────────────────────────────
  {
    const buf = new ArrayBuffer(BUFFER_SIZE);
    new Int32Array(buf).fill(42);

    const w = new Worker(__filename, { workerData: { demo: "clone" } });
    const t = Date.now();
    // No transfer list → structured clone
    w.postMessage({ buf });
    console.log(`[clone]    sender byteLength after postMessage: ${buf.byteLength} (still valid)`);

    w.on("message", ({ received, elapsed }) => {
      console.log(`[clone]    worker received ${received} bytes, main sent in ${Date.now() - t}ms`);
      check();
    });
    w.on("error", (e) => console.error("clone worker error:", e.message));
  }

  // ── Demo 2: Transfer ─────────────────────────────────────────────────────
  {
    const buf = new ArrayBuffer(BUFFER_SIZE);
    new Int32Array(buf).fill(99);

    const w = new Worker(__filename, { workerData: { demo: "transfer" } });
    const t = Date.now();
    // Pass buf in the transfer list → zero-copy ownership move
    w.postMessage({ buf }, [buf]);
    console.log(`[transfer] sender byteLength after postMessage: ${buf.byteLength} (detached!)`);

    w.on("message", ({ received, elapsed }) => {
      console.log(`[transfer] worker received ${received} bytes, main sent in ${Date.now() - t}ms`);
      check();
    });
    w.on("error", (e) => console.error("transfer worker error:", e.message));
  }

  // ── Demo 3: SharedArrayBuffer ────────────────────────────────────────────
  {
    const sab = new SharedArrayBuffer(BUFFER_SIZE);
    const view = new Int32Array(sab);
    view.fill(0);

    const w = new Worker(__filename, { workerData: { demo: "shared", sab } });
    const t = Date.now();
    // No postMessage needed for the SAB — it was in workerData
    // Write a value; the worker will see it
    view[0] = 7;

    w.on("message", ({ workerRead }) => {
      console.log(`[shared]   main wrote view[0]=7, worker read view[0]=${workerRead}`);
      view[0] = 100;
      console.log(`[shared]   main updated view[0]=100, worker will see it too`);
      check();
    });
    w.on("error", (e) => console.error("shared worker error:", e.message));
  }

// ── Worker ───────────────────────────────────────────────────────────────────
} else {
  const { demo, sab } = workerData;

  if (demo === "clone" || demo === "transfer") {
    parentPort.once("message", ({ buf }) => {
      parentPort.postMessage({ received: buf.byteLength });
    });

  } else if (demo === "shared") {
    const view = new Int32Array(sab);
    // Read value the main thread wrote before sending the ready signal
    const workerRead = Atomics.load(view, 0);
    parentPort.postMessage({ workerRead });
  }
}
