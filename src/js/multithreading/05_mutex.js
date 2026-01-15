/*
 * Mutex (Mutual Exclusion) with Worker Threads
 *
 * This script demonstrates using a mutex to protect shared data from race conditions.
 * It uses Atomics.compareExchange for lock acquisition and Atomics.wait/notify
 * for efficient blocking.
 *
 * Key concepts:
 * - Atomics.compareExchange: atomic compare-and-swap for lock acquisition
 * - Atomics.wait: block thread until lock is released
 * - Atomics.notify: wake up waiting threads
 * - Critical section protection
 *
 * Compare with 04_race_condition.js to see the difference proper synchronization makes.
 */

"use strict";

const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  console.log("=== Mutex Demo ===");
  console.log("Multiple threads modifying shared counter WITH mutex protection\n");

  const NUM_THREADS = 5;
  const ITERATIONS = 10;

  // Shared memory layout: [counter (4 bytes), mutex (4 bytes), doneCount (4 bytes)]
  const sharedBuffer = new SharedArrayBuffer(12);
  const counterView = new Int32Array(sharedBuffer, 0, 1);
  const mutexView = new Int32Array(sharedBuffer, 4, 1);
  const doneCounterView = new Int32Array(sharedBuffer, 8, 1);
  
  counterView[0] = 1;
  mutexView[0] = 0;  // 0 = unlocked, 1 = locked
  doneCounterView[0] = 0;

  console.log(`Initial counter value: ${counterView[0]}`);
  console.log(`Threads: ${NUM_THREADS}, Iterations per thread: ${ITERATIONS}\n`);

  const workers = [];

  for (let i = 0; i < NUM_THREADS; i++) {
    const worker = new Worker(__filename);
    
    worker.on("message", (message) => {
      if (message.type === "print") {
        console.log(`  [Thread ${message.threadId}] ${message.text}`);
      } else if (message.type === "done") {
        doneCounterView[0]++;
        if (doneCounterView[0] === NUM_THREADS) {
          console.log(`\n=== Results ===`);
          console.log(`Final counter value: ${counterView[0]}`);
          console.log("Note: With mutex, updates are properly synchronized!");
        }
      }
    });
    
    worker.on("error", (error) => console.error(`Worker ${i} error:`, error));
    worker.postMessage({ type: "init", id: i, sharedBuffer });
    workers.push(worker);
  }
} else {
  let id;
  let counterView;
  let mutexView;

  parentPort.on("message", (message) => {
    if (message.type === "init") {
      id = message.id;
      counterView = new Int32Array(message.sharedBuffer, 0, 1);
      mutexView = new Int32Array(message.sharedBuffer, 4, 1);

      for (let i = 0; i < 10; i++) {
        // Acquire mutex (spin until we get it)
        while (Atomics.compareExchange(mutexView, 0, 0, 1) === 1) {
          // Wait for mutex to be released (value to become 0)
          Atomics.wait(mutexView, 0, 1);
        }

        // Critical section - safely access shared data
        const localCounter = counterView[0];
        const multipliedCounter = localCounter * (id + 1);
        
        parentPort.postMessage({
          type: "print",
          threadId: id,
          text: `Multiplying ${localCounter} by ${id + 1} = ${multipliedCounter}`,
        });
        
        counterView[0] = multipliedCounter;

        // Release mutex
        Atomics.store(mutexView, 0, 0);
        Atomics.notify(mutexView, 0, 1);
      }

      parentPort.postMessage({ type: "done" });
    }
  });
}
