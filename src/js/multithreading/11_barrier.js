/*
 * Barrier Synchronization with Worker Threads
 *
 * This script demonstrates barrier synchronization pattern using Atomics.
 * A barrier ensures all threads reach a synchronization point before any
 * can proceed past it.
 *
 * Key concepts:
 * - Atomics.add for counting arrivals
 * - Atomics.wait/notify for blocking and releasing threads
 * - All threads wait until the last one arrives
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (isMainThread) {
  console.log("=== Barrier Synchronization Demo ===\n");

  const NUM_THREADS = 5;
  
  // Shared memory: [count (4 bytes), released flag (4 bytes)]
  const sharedBuffer = new SharedArrayBuffer(8);
  const view = new Int32Array(sharedBuffer);
  view[0] = 0; // count
  view[1] = 0; // released flag

  console.log(`Threads: ${NUM_THREADS}`);
  console.log("All threads must reach barrier before any can proceed.\n");

  const workers = [];
  let completedThreads = 0;

  for (let i = 0; i < NUM_THREADS; i++) {
    const worker = new Worker(__filename, {
      workerData: { 
        sharedBuffer, 
        threadId: i,
        numThreads: NUM_THREADS,
      },
    });

    worker.on("message", (message) => {
      if (message.type === "status") {
        console.log(`  [Thread ${message.threadId}] ${message.text}`);
      } else if (message.type === "done") {
        completedThreads++;
        if (completedThreads === NUM_THREADS) {
          console.log("\n=== All threads completed ===");
        }
      }
    });

    worker.on("error", (error) => {
      console.error(`Thread ${i} error:`, error.message);
    });

    workers.push(worker);
  }

} else {
  const { sharedBuffer, threadId, numThreads } = workerData;
  const view = new Int32Array(sharedBuffer);

  const barrierWait = () => {
    // Increment count atomically
    const arrivedCount = Atomics.add(view, 0, 1) + 1;

    if (arrivedCount === numThreads) {
      // Last thread to arrive - release everyone
      Atomics.store(view, 1, 1); // Set released flag
      Atomics.notify(view, 1); // Wake all waiting threads
    } else {
      // Wait for released flag to become 1
      while (Atomics.load(view, 1) === 0) {
        Atomics.wait(view, 1, 0, 100); // Wait with timeout
      }
    }
  };

  const run = async () => {
    // Simulate work before barrier
    const workTime = Math.floor(Math.random() * 2000) + 500;
    parentPort.postMessage({
      type: "status",
      threadId,
      text: `Starting work (${workTime}ms)...`,
    });

    await sleep(workTime);

    parentPort.postMessage({
      type: "status",
      threadId,
      text: "Reached barrier, waiting for others...",
    });

    // Wait at barrier
    barrierWait();

    parentPort.postMessage({
      type: "status",
      threadId,
      text: "Passed barrier! Continuing execution...",
    });

    // Work after barrier
    await sleep(500);

    parentPort.postMessage({ type: "done", threadId });
  };

  run();
}
