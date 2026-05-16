/*
 * Worker Error Handling
 *
 * Worker threads are isolated V8 instances.  Errors thrown inside a worker
 * do NOT crash the main thread — they are surfaced through worker events.
 *
 * Key concepts:
 * - worker.on('error')  — fires when an exception escapes the worker's top
 *                         level; the worker exits with code 1
 * - worker.on('exit')   — always fires; code 0 = clean, non-zero = error
 * - 'unhandledRejection' inside a worker behaves like an uncaught exception
 *   in recent Node.js versions (terminates the worker with code 1)
 * - process.on('uncaughtException') in a worker lets you log/cleanup before
 *   the worker exits; do NOT use it to swallow fatal errors and continue —
 *   the worker state is undefined after an uncaught exception
 * - To propagate error detail back to the main thread, postMessage the error
 *   before exiting (or re-throw via a structured message protocol)
 *
 * Four demos run sequentially:
 *   1. Worker throws synchronously           → worker.on('error') fires
 *   2. Worker has unhandled Promise rejection → worker.on('error') fires
 *   3. Worker uses process.on('uncaughtException') to log + exit cleanly
 *   4. Worker sends error to main via postMessage, then exits
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== Worker Error Handling Demo ===\n");

  const runDemo = (demoNum, label) =>
    new Promise((resolve) => {
      console.log(`--- Demo ${demoNum}: ${label} ---`);
      const w = new Worker(__filename, { workerData: { demo: demoNum } });

      w.on("message", (msg) => {
        console.log(`  [worker ${demoNum}] message: ${JSON.stringify(msg)}`);
      });

      w.on("error", (err) => {
        console.log(`  [main]   worker.on('error'): ${err.message}`);
      });

      w.on("exit", (code) => {
        console.log(`  [main]   worker.on('exit'): code=${code}\n`);
        resolve();
      });
    });

  (async () => {
    await runDemo(1, "synchronous throw");
    await runDemo(2, "unhandled Promise rejection");
    await runDemo(3, "process.on('uncaughtException') in worker");
    await runDemo(4, "worker reports error via postMessage");
    console.log("=== All demos complete ===");
  })();

// ── Worker demos ──────────────────────────────────────────────────────────────
} else {
  const { demo } = workerData;

  if (demo === 1) {
    // Synchronous throw — escapes worker top level → worker.on('error')
    throw new Error("Intentional synchronous error from worker");

  } else if (demo === 2) {
    // Unhandled Promise rejection — treated as uncaught exception in Node 15+
    // → worker.on('error') fires, worker exits with code 1
    Promise.reject(new Error("Unhandled rejection from worker"));
    // Keep the worker alive long enough for the rejection to propagate
    setTimeout(() => {}, 500);

  } else if (demo === 3) {
    // process.on('uncaughtException') lets you log/cleanup before exit.
    // You MUST call process.exit() — Node's default is to terminate anyway,
    // and continuing after an uncaught exception risks corrupted state.
    process.on("uncaughtException", (err) => {
      parentPort.postMessage(`caught uncaughtException: ${err.message}`);
      // Cleanup would go here
      process.exit(1);
    });

    process.on("unhandledRejection", (reason) => {
      parentPort.postMessage(`caught unhandledRejection: ${reason}`);
      process.exit(1);
    });

    throw new Error("Error caught by uncaughtException handler");

  } else if (demo === 4) {
    // Cooperative error propagation: worker sends structured error to main,
    // then exits cleanly (code 0).  Main inspects the message.
    try {
      throw new Error("Something went wrong in worker task");
    } catch (err) {
      parentPort.postMessage({ type: "error", message: err.message, stack: err.stack });
      // Exit cleanly — main thread has the error detail
      process.exit(0);
    }
  }
}
