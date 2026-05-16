/*
 * ⚠️  ANTIPATTERN — Race Condition on Shared Counter
 *
 * Multiple workers read, multiply, and write a shared counter without
 * any synchronization.  The result is unpredictable: each run can produce
 * a different final value.
 *
 * Why it races:
 *   The sequence  load → compute → store  is not atomic.  Between
 *   Thread A's load and its store, Thread B can load the same stale value
 *   and overwrite A's result (or vice-versa).
 *
 * Note on SharedArrayBuffer:
 *   SharedArrayBuffer lets workers share raw memory.  Without Atomics,
 *   plain reads/writes are non-atomic and subject to data races even for
 *   a single 32-bit integer.
 *
 * Fix: see 05_mutex.js, which protects the critical section with an
 * Atomics-based mutex so only one thread updates the counter at a time.
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
