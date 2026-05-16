/*
 * Atomics.waitAsync — Non-Blocking Coordination
 *
 * Atomics.wait() blocks the entire worker thread until the condition is met.
 * That is fine inside a worker (where blocking is acceptable), but it cannot
 * be used on the main thread and breaks async code that needs to keep the
 * event loop alive.
 *
 * Atomics.waitAsync() solves this: it returns a Promise-like result object
 * so the calling code can await it without blocking the thread.
 *
 * Return value shape:
 *   const result = Atomics.waitAsync(view, index, expected[, timeout]);
 *   if (result.async) {
 *     const outcome = await result.value; // Promise<"ok"|"timed-out">
 *   } else {
 *     const outcome = result.value; // "not-equal" (immediate synchronous return)
 *   }
 *
 * "not-equal" means view[index] !== expected at call time — no wait needed.
 * "ok"         means the notification arrived (Atomics.notify was called).
 * "timed-out"  means the timeout expired before notification.
 *
 * Key concepts:
 * - Atomics.wait    — synchronous / blocking; worker threads only
 * - Atomics.waitAsync — async / non-blocking; safe on main thread
 * - Atomics.notify  — wakes one or more waiters (both sync and async)
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

// Shared memory: one Int32 used as a signal flag (0 = not ready, 1 = ready)
const sharedBuffer = new SharedArrayBuffer(Int32Array.BYTES_PER_ELEMENT * 2);
// Index 0: signal flag
// Index 1: result value written by worker

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== Atomics.waitAsync Demo ===\n");

  const view = new Int32Array(sharedBuffer);
  view[0] = 0; // flag starts at 0 (not ready)

  // ── Demo 1: Atomics.waitAsync on main thread ───────────────────────────
  console.log("--- Demo 1: main thread waits for worker signal (non-blocking) ---");
  console.log("Main is NOT blocked; the event loop keeps running while waiting.\n");

  const worker = new Worker(__filename, {
    workerData: { demo: "signal", sharedBuffer },
  });

  // waitAsync: main thread waits for flag[0] to change from 0
  const waitResult = Atomics.waitAsync(view, 0, 0);

  if (!waitResult.async) {
    // Value was already != 0 at call time — immediate synchronous result
    console.log(`[main] waitAsync immediate: "${waitResult.value}"`);
  } else {
    // Async path: value was 0, now waiting for a notify
    console.log("[main] waitAsync is pending (async === true)...");
    console.log("[main] Event loop is free — other tasks can run here.");

    waitResult.value.then((outcome) => {
      const result = Atomics.load(view, 1);
      console.log(`\n[main] waitAsync resolved: "${outcome}"`);
      console.log(`[main] Worker wrote result: ${result}`);
    });
  }

  // ── Demo 2: waitAsync with timeout ───────────────────────────────────
  console.log("\n--- Demo 2: Atomics.waitAsync with a short timeout ---");

  const shortBuffer = new SharedArrayBuffer(4);
  const shortView = new Int32Array(shortBuffer);
  shortView[0] = 0;

  // Nobody will notify this — it should time out
  const timedWait = Atomics.waitAsync(shortView, 0, 0, 500); // 500 ms
  if (timedWait.async) {
    timedWait.value.then((outcome) => {
      console.log(`[main] Short wait resolved: "${outcome}"`); // "timed-out"
      console.log("\n=== Demo complete ===");
    });
  }

  // ── Demo 3: "not-equal" synchronous case ─────────────────────────────
  console.log("\n--- Demo 3: Atomics.waitAsync when value already differs ---");
  const nowBuffer = new SharedArrayBuffer(4);
  const nowView = new Int32Array(nowBuffer);
  Atomics.store(nowView, 0, 1); // already 1
  const syncResult = Atomics.waitAsync(nowView, 0, 0); // expects 0, but it's 1
  console.log(`[main] async: ${syncResult.async}, value: "${syncResult.value}"`);
  // → async: false, value: "not-equal"

// ── Worker ───────────────────────────────────────────────────────────────────
} else if (workerData.demo === "signal") {
  const view = new Int32Array(workerData.sharedBuffer);

  // Simulate CPU-bound work
  let sum = 0;
  for (let i = 0; i < 50_000_000; i++) sum += i;

  // Write the result, then notify waiters
  Atomics.store(view, 1, sum % 1_000_000); // store a result
  Atomics.store(view, 0, 1);               // set flag
  Atomics.notify(view, 0, 1);              // wake one async waiter
}
