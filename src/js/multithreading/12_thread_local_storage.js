/*
 * Thread-Local Storage Pattern
 *
 * This script demonstrates thread-local storage in Node.js worker threads.
 * Each worker has its own isolated global scope, allowing thread-specific data
 * without explicit synchronization.
 *
 * Key concepts:
 * - Worker threads have isolated memory spaces
 * - Global variables in workers are thread-local
 * - No synchronization needed for thread-local data
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

// This variable is thread-local - each worker has its own copy
let threadLocalData = null;

const someWork = () => {
  // Modify thread-local data
  threadLocalData.counter *= 2;
  threadLocalData.operations.push("doubled");
};

const moreWork = () => {
  // More modifications
  threadLocalData.counter += 10;
  threadLocalData.operations.push("added 10");
};

const workerFunction = (threadId) => {
  // Initialize thread-local data
  threadLocalData = {
    threadId,
    counter: threadId,
    operations: [],
    startTime: Date.now(),
  };

  // Simulate work that modifies thread-local data
  someWork();
  moreWork();
  someWork();

  // Each thread has its own isolated state
  const result = {
    threadId,
    finalCounter: threadLocalData.counter,
    operations: threadLocalData.operations,
    elapsed: Date.now() - threadLocalData.startTime,
  };

  return result;
};

if (isMainThread) {
  console.log("=== Thread-Local Storage Demo ===\n");
  console.log("Each worker has isolated memory - no shared state concerns.\n");

  const NUM_THREADS = 5;
  const workers = [];
  const results = [];

  for (let i = 0; i < NUM_THREADS; i++) {
    const worker = new Worker(__filename, { workerData: i + 1 });

    worker.on("message", (result) => {
      console.log(`  [Thread ${result.threadId}] Final counter: ${result.finalCounter}`);
      console.log(`    Operations: ${result.operations.join(" -> ")}`);
      results.push(result);

      if (results.length === NUM_THREADS) {
        console.log("\n=== Summary ===");
        console.log("Each thread computed independently:");
        results.sort((a, b) => a.threadId - b.threadId);
        for (const r of results) {
          console.log(`  Thread ${r.threadId}: ${r.threadId} -> ${r.finalCounter}`);
        }
      }
    });

    worker.on("error", (error) => {
      console.error(`Worker ${i} error:`, error.message);
    });

    workers.push(worker);
  }

} else {
  const result = workerFunction(workerData);
  parentPort.postMessage(result);
}
