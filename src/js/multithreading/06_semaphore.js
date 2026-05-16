/*
 * Semaphore with Worker Threads
 *
 * A counting semaphore limits the number of workers that may be inside a
 * critical section simultaneously.  This example initialises the semaphore
 * to 1 (binary semaphore / mutex-like), but the same pattern works for
 * permits > 1.
 *
 * Key concepts:
 * - Atomics.compareExchange: atomic compare-and-swap for correct acquisition
 * - Atomics.wait: block this worker thread until the semaphore is released
 * - Atomics.notify: wake one waiting thread on release
 * - Why load + sub is wrong (TOCTOU) and CAS is right
 *
 * Previous TOCTOU bug (now fixed):
 *   while (load > 0) {}    // two threads both see > 0
 *   Atomics.sub(...)       // both decrement → value goes negative, critical
 *                          // section is entered by both
 *
 * Correct acquire: use compareExchange to atomically claim the decrement;
 * only wait (blocking) when the permit count is actually 0.
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
          workers.forEach((w) => w.terminate());
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
          // Acquire semaphore with CAS loop (avoids TOCTOU load + sub race).
          // Only block (Atomics.wait) when permits == 0; retry immediately
          // after a failed CAS so we don't sleep with permits available.
          while (true) {
            const cur = Atomics.load(semaphoreView, 0);
            if (cur > 0) {
              if (Atomics.compareExchange(semaphoreView, 0, cur, cur - 1) === cur) {
                break; // permit acquired
              }
              continue; // CAS lost to another thread, retry immediately
            }
            // cur === 0: no permits; wait until the value changes
            Atomics.wait(semaphoreView, 0, 0);
          }

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
