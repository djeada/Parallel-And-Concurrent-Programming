/*
 * ⚠️  ANTIPATTERN — Worker Thread Deadlock (Circular Atomics.wait)
 *
 * Two workers each wait for the other to signal first.
 * Neither can proceed → deadlock.
 *
 * Four necessary conditions (same as process deadlocks):
 *   1. Mutual exclusion  — each flag is "held" by one waiter
 *   2. Hold-and-wait     — worker A holds its own flag, waits for B's
 *   3. No preemption     — Atomics.wait cannot be forcibly cancelled
 *   4. Circular wait     — A waits for B; B waits for A
 *
 * Atomics.wait BLOCKS the worker thread, not just the event loop.
 * The worker is completely suspended — no timers, no I/O, no messages
 * will be processed while it waits.
 *
 * This demo exits cleanly by passing a 3-second timeout to Atomics.wait.
 * Without the timeout, both workers would hang forever.
 *
 * Fix: establish an ordering — always signal flag_a before waiting on
 * flag_b in both workers, so there is no circular dependency.
 */

"use strict";

const {
  Worker,
  isMainThread,
  workerData,
  parentPort,
} = require("worker_threads");

// Two flags: flagA is set by Worker A; flagB is set by Worker B.
const sabA = new SharedArrayBuffer(4); // Worker A's "I'm done" flag
const sabB = new SharedArrayBuffer(4); // Worker B's "I'm done" flag
const TIMEOUT_MS = 3000;

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== Deadlock Antipattern Demo ===\n");
  console.log("Worker A waits for B to signal first.");
  console.log("Worker B waits for A to signal first.");
  console.log(`Both have a ${TIMEOUT_MS}ms timeout so the process can exit.\n`);

  const workerA = new Worker(__filename, {
    workerData: { id: "A", waitOn: sabB, myFlag: sabA },
  });
  const workerB = new Worker(__filename, {
    workerData: { id: "B", waitOn: sabA, myFlag: sabB },
  });

  let exited = 0;
  for (const w of [workerA, workerB]) {
    w.on("message", (msg) => console.log(`  [worker] ${msg}`));
    w.on("error", (err) => console.error("Worker error:", err.message));
    w.on("exit", (code) => {
      if (++exited === 2) {
        console.log("\n=== Both workers exited (deadlock resolved by timeout) ===");
        console.log("Fix: signal your own flag BEFORE waiting for another.");
      }
    });
  }

// ── Worker ───────────────────────────────────────────────────────────────────
} else {
  const { id, waitOn, myFlag } = workerData;
  const waitView = new Int32Array(waitOn);
  const myView   = new Int32Array(myFlag);

  parentPort.postMessage(`Worker ${id}: waiting for the other worker to go first...`);

  // Atomics.wait blocks this thread. If the other worker is also waiting here,
  // neither can proceed — classic deadlock.
  const outcome = Atomics.wait(waitView, 0, 0, TIMEOUT_MS);

  if (outcome === "timed-out") {
    parentPort.postMessage(
      `Worker ${id}: ⚠️  DEADLOCK DETECTED — timed out after ${TIMEOUT_MS}ms`
    );
    // Exit without setting our flag — the other worker will also time out.
  } else {
    // This branch only runs if the deadlock were somehow resolved externally.
    parentPort.postMessage(`Worker ${id}: unblocked (outcome: "${outcome}")`);
    Atomics.store(myView, 0, 1);
    Atomics.notify(myView, 0, 1);
  }
}
