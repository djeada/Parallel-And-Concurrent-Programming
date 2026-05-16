/*
 * Worker Lifecycle & Cooperative Cancellation
 *
 * Worker threads do not stop automatically when their task is done —
 * they must be terminated either by returning from all async work,
 * by the main thread calling worker.terminate(), or by cooperating
 * on a shared cancellation flag.
 *
 * Key concepts:
 * - worker.terminate()   — immediately terminates worker; exit code 1
 * - Cooperative cancel   — worker checks a SharedArrayBuffer flag; main
 *                          sets the flag to signal cancellation
 * - worker.ref()         — default; process stays alive while worker runs
 * - worker.unref()       — process may exit even if worker is still alive
 *                          (useful for background/optional work)
 * - Graceful vs abrupt   — terminate() is abrupt (no cleanup); cooperative
 *                          cancel lets the worker finish current work, free
 *                          resources, and send a final message
 *
 * Three demos:
 *   1. worker.terminate() — abrupt; worker exits mid-work
 *   2. Cooperative cancel — main sets flag; worker checks and exits cleanly
 *   3. worker.unref()     — process exits without waiting for the worker
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== Worker Lifecycle & Cancellation Demo ===\n");

  // ── Demo 1: worker.terminate() ─────────────────────────────────────────
  (async () => {
    console.log("--- Demo 1: worker.terminate() ---");

    const w = new Worker(__filename, { workerData: { demo: "loop" } });
    w.on("message", (msg) => console.log(`  [worker1] ${msg}`));

    await sleep(1200); // let worker run for ~1s
    console.log("  [main] calling worker.terminate()...");
    await w.terminate();
    console.log("  [main] worker terminated (abruptly, no cleanup)\n");

    // ── Demo 2: Cooperative cancellation ──────────────────────────────────
    console.log("--- Demo 2: Cooperative cancellation ---");

    // cancelFlag[0] === 0 means "keep running"; 1 means "please stop"
    const cancelFlag = new SharedArrayBuffer(4);
    const cancelView = new Int32Array(cancelFlag);
    cancelView[0] = 0;

    const w2 = new Worker(__filename, {
      workerData: { demo: "cooperative", cancelFlag },
    });
    w2.on("message", (msg) => console.log(`  [worker2] ${msg}`));
    w2.on("exit", (code) => console.log(`  [main] worker2 exited, code=${code}\n`));

    await sleep(1200);
    console.log("  [main] setting cancel flag...");
    Atomics.store(cancelView, 0, 1);
    Atomics.notify(cancelView, 0, 1);

    // ── Demo 3: worker.unref() ────────────────────────────────────────────
    await sleep(600); // wait for worker2 to finish
    console.log("--- Demo 3: worker.unref() ---");

    const w3 = new Worker(__filename, { workerData: { demo: "unref" } });
    w3.on("message", (msg) => console.log(`  [worker3] ${msg}`));
    // unref: this worker will not prevent the process from exiting
    w3.unref();
    console.log("  [main] worker3 is unref'd — process may exit before it finishes");
    console.log("\n=== Main thread done (process may exit now) ===");
    // Process exits here; worker3 may or may not finish depending on timing.
  })();

// ── Workers ───────────────────────────────────────────────────────────────────
} else if (workerData.demo === "loop") {
  // Runs indefinitely until terminate() is called from outside
  let i = 0;
  const tick = () => {
    i++;
    parentPort.postMessage(`tick ${i} (will be abruptly terminated)`);
    setTimeout(tick, 300);
  };
  tick();

} else if (workerData.demo === "cooperative") {
  // Checks a shared cancellation flag each iteration
  const cancelView = new Int32Array(workerData.cancelFlag);

  const run = async () => {
    let i = 0;
    while (Atomics.load(cancelView, 0) === 0) {
      i++;
      parentPort.postMessage(`working (iteration ${i})`);
      await sleep(300);
    }
    parentPort.postMessage("received cancel signal — cleaning up");
    await sleep(100); // simulate cleanup
    parentPort.postMessage("done, exiting cleanly");
  };
  run();

} else if (workerData.demo === "unref") {
  // Long-running background task
  const run = async () => {
    for (let i = 1; i <= 10; i++) {
      parentPort.postMessage(`background tick ${i}`);
      await sleep(400);
    }
  };
  run();
}
