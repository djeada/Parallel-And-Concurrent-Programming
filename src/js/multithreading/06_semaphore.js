/*
 * Semaphore with Worker Threads
 *
 * This script demonstrates a semaphore implementation using Atomics.
 * A semaphore allows a limited number of threads to access a resource
 * concurrently (in this case, limit is 1, making it behave like a mutex).
 *
 * Key concepts:
 * - Atomics.wait: block until semaphore value > 0
 * - Atomics.sub/add: decrement/increment semaphore atomically
 * - Atomics.notify: wake up waiting threads
 * - Controlled access to shared resources
 */

"use strict";

const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  console.log("=== Semaphore Demo ===");
  console.log("Multiple threads using semaphore for synchronization\n");

  const NUM_WORKERS = 5;
  const ITERATIONS = 5;

  // Shared memory buffers
  const sharedCounter = new SharedArrayBuffer(4);
  const counterView = new Int32Array(sharedCounter);
  counterView[0] = 1;

  const sharedSemaphore = new SharedArrayBuffer(4);
  const semaphoreView = new Int32Array(sharedSemaphore);
  semaphoreView[0] = 1; // Semaphore initialized to 1 (binary semaphore)

  const sharedDoneCounter = new SharedArrayBuffer(4);
  const doneCounterView = new Int32Array(sharedDoneCounter);
  doneCounterView[0] = 0;

  console.log(`Initial counter value: ${counterView[0]}`);
  console.log(`Workers: ${NUM_WORKERS}, Iterations per worker: ${ITERATIONS}`);
  console.log(`Semaphore permits: ${semaphoreView[0]}\n`);

  const workers = [];

  for (let i = 0; i < NUM_WORKERS; i++) {
    const worker = new Worker(__filename);
    
    worker.on("message", (message) => {
      if (message.type === "print") {
        console.log(`  [Worker ${message.workerId}] ${message.text}`);
      } else if (message.type === "done") {
        Atomics.add(doneCounterView, 0, 1);
        if (Atomics.load(doneCounterView, 0) === NUM_WORKERS) {
          console.log(`\n=== Results ===`);
          console.log(`Final counter value: ${counterView[0]}`);
        }
      }
    });
    
    worker.on("error", (error) => console.error(`Worker ${i} error:`, error));
    
    worker.postMessage({
      type: "init",
      id: i,
      sharedCounter,
      sharedSemaphore,
      iterations: ITERATIONS,
    });
    
    workers.push(worker);
  }
} else {
  let id;
  let counterView;
  let semaphoreView;
  let iterations;

  parentPort.on("message", (message) => {
    if (message.type === "init") {
      id = message.id;
      counterView = new Int32Array(message.sharedCounter);
      semaphoreView = new Int32Array(message.sharedSemaphore);
      iterations = message.iterations;

      const processIterations = async () => {
        for (let i = 0; i < iterations; i++) {
          // Wait for semaphore (wait while value is 0)
          while (Atomics.load(semaphoreView, 0) <= 0) {
            Atomics.wait(semaphoreView, 0, 0);
          }
          
          // Acquire semaphore
          Atomics.sub(semaphoreView, 0, 1);

          // Critical section
          const localCounter = counterView[0];
          const newCounter = localCounter * (id + 1);
          
          parentPort.postMessage({
            type: "print",
            workerId: id,
            text: `Multiplying ${localCounter} by ${id + 1} = ${newCounter}`,
          });
          
          counterView[0] = newCounter;

          // Release semaphore
          Atomics.add(semaphoreView, 0, 1);
          Atomics.notify(semaphoreView, 0, 1);

          // Small delay between iterations
          await new Promise((resolve) => setTimeout(resolve, 50));
        }

        parentPort.postMessage({ type: "done" });
      };

      processIterations();
    }
  });
}
