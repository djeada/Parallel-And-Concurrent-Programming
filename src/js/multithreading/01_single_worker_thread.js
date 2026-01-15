/*
 * Single Worker Thread Example
 *
 * This script demonstrates the basics of Node.js Worker Threads.
 * It shows how to:
 * - Create a worker thread from the same file
 * - Communicate between main and worker threads using messages
 * - Handle worker lifecycle events (message, exit, error)
 *
 * Key concepts:
 * - isMainThread: Check if running in main or worker thread
 * - parentPort: Communication channel in worker thread
 * - Worker: Create new worker threads
 */

"use strict";

const {
  Worker,
  isMainThread,
  threadId,
  parentPort,
} = require("worker_threads");

const main = () => {
  console.log("=== Single Worker Thread Demo ===\n");
  console.log(`Main thread process ID: ${process.pid}`);
  console.log(`Main thread ID: ${threadId}`);

  const worker = new Worker(__filename);

  worker.on("message", (message) => {
    console.log(`  [Worker] ${message}`);
  });

  worker.on("error", (error) => {
    console.error(`Worker error: ${error.message}`);
  });

  worker.on("exit", (code) => {
    console.log(`\nWorker thread exited with code: ${code}`);
    console.log("=== Demo Complete ===");
  });
};

const workerFunction = () => {
  parentPort.postMessage(`Starting worker function`);
  parentPort.postMessage(`Process ID: ${process.pid}`);
  parentPort.postMessage(`Thread ID: ${threadId}`);
  parentPort.postMessage(`Worker completed successfully`);
};

if (isMainThread) {
  main();
} else {
  workerFunction();
}
