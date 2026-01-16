/*
 * Race Condition Demonstration
 *
 * This script demonstrates a classic race condition problem in multithreading.
 * Multiple workers attempt to read, modify, and write a shared counter without
 * proper synchronization, resulting in unpredictable final values.
 *
 * Key concepts:
 * - SharedArrayBuffer for shared memory between threads
 * - Race condition: multiple threads accessing shared data without synchronization
 * - Why the final value is unpredictable
 *
 * Compare with 05_mutex.js to see how to fix this issue.
 */

"use strict";

const { Worker, isMainThread, parentPort } = require("worker_threads");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (isMainThread) {
  console.log("=== Race Condition Demo ===");
  console.log("Multiple threads modifying shared counter WITHOUT synchronization\n");

  const NUM_THREADS = 5;
  const ITERATIONS = 10;
  
  // Shared memory layout: [counter (4 bytes), doneCount (4 bytes)]
  const sharedBuffer = new SharedArrayBuffer(8);
  const counterView = new Int32Array(sharedBuffer, 0, 1);
  const doneCounterView = new Int32Array(sharedBuffer, 4, 1);
  
  counterView[0] = 1;
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
          console.log("Note: Value is unpredictable due to race conditions!");
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

  parentPort.on("message", async (message) => {
    if (message.type === "init") {
      id = message.id;
      counterView = new Int32Array(message.sharedBuffer, 0, 1);

      for (let i = 0; i < 10; i++) {
        // Read current value (RACE: value may change before we write)
        const localCounter = counterView[0];
        const multipliedCounter = localCounter * (id + 1);
        
        // Simulate some processing time (increases race condition probability)
        await sleep(10);
        
        parentPort.postMessage({
          type: "print",
          threadId: id,
          text: `Multiplying ${localCounter} by ${id + 1} = ${multipliedCounter}`,
        });
        
        // Write back (RACE: may overwrite another thread's update)
        counterView[0] = multipliedCounter;
      }

      parentPort.postMessage({ type: "done" });
      parentPort.close();
    }
  });
}
