/*
 * ⚠️  ANTIPATTERN — Reader-Writer Lock via Naive Polling
 *
 * This implementation looks like a reader-writer lock but has two
 * time-of-check / time-of-use (TOCTOU) races that break both exclusivity
 * and reader safety:
 *
 *   Reader TOCTOU — each reader checks for active/waiting writers with
 *     await sleep() in a loop, then increments the reader count.  But
 *     `await` yields to the event loop; between resuming and the
 *     Atomics.add a writer can acquire the lock, so a reader and writer
 *     run concurrently.
 *
 *   Writer TOCTOU — two writers can both see `readers === 0 && writers === 0`,
 *     both decrement waitingWriters and increment activeWriters, giving
 *     writers[0] === 2 — two concurrent writers that corrupt shared data.
 *
 * The header originally claimed "fair scheduling to prevent writer
 * starvation" — this is incorrect; neither fairness nor exclusivity is
 * guaranteed.
 *
 * Fix: use a single Int32 state word and a CAS loop to atomically
 * encode reader count + writer-present flag, or use a proper mutex
 * (05_mutex.js) around each read/write section.
 *
 * The polling pattern with `await sleep()` is also CPU-wasteful (busy
 * wait disguised as async); prefer Atomics.wait / Atomics.waitAsync for
 * efficient blocking.
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// Shared memory layout:
// [0] = data value
// [1] = active readers count
// [2] = active writers count (0 or 1)
// [3] = waiting writers count

if (isMainThread) {
  console.log("=== Reader-Writer Lock Demo ===\n");

  const NUM_READERS = 3;
  const NUM_WRITERS = 2;
  const RUN_TIME_MS = 5000;

  console.log(`Readers: ${NUM_READERS}, Writers: ${NUM_WRITERS}`);
  console.log(`Running for ${RUN_TIME_MS / 1000} seconds...\n`);

  const sharedArrayBuffer = new SharedArrayBuffer(4 * Int32Array.BYTES_PER_ELEMENT);
  const sharedData = new Int32Array(sharedArrayBuffer);
  sharedData[0] = 0; // data
  sharedData[1] = 0; // readers
  sharedData[2] = 0; // writers
  sharedData[3] = 0; // waiting writers

  const workers = [];

  // Create readers
  for (let i = 0; i < NUM_READERS; i++) {
    const worker = new Worker(__filename, {
      workerData: { type: "reader", threadId: i, sharedArrayBuffer },
    });

    worker.on("message", (msg) => {
      if (msg.type === "read") {
        console.log(`  [Reader ${msg.threadId}] Read value: ${msg.value}`);
      }
    });

    workers.push(worker);
  }

  // Create writers
  for (let i = 0; i < NUM_WRITERS; i++) {
    const worker = new Worker(__filename, {
      workerData: { type: "writer", threadId: i, sharedArrayBuffer },
    });

    worker.on("message", (msg) => {
      if (msg.type === "write") {
        console.log(`  [Writer ${msg.threadId}] Wrote value: ${msg.value}`);
      }
    });

    workers.push(worker);
  }

  // Stop after timeout
  setTimeout(() => {
    console.log("\n=== Stopping workers ===");
    workers.forEach((w) => w.terminate());
    console.log(`Final data value: ${sharedData[0]}`);
  }, RUN_TIME_MS);

} else {
  const { type, threadId, sharedArrayBuffer } = workerData;
  const sharedData = new Int32Array(sharedArrayBuffer);

  const acquireReadLock = async () => {
    // Wait while there are active or waiting writers
    while (
      Atomics.load(sharedData, 2) > 0 ||
      Atomics.load(sharedData, 3) > 0
    ) {
      // Use a short timeout to avoid burning CPU while waiting
      await sleep(10);
    }
    // Increment reader count
    Atomics.add(sharedData, 1, 1);
  };

  const releaseReadLock = () => {
    Atomics.sub(sharedData, 1, 1);
  };

  const acquireWriteLock = async () => {
    // Signal that we're waiting to write
    Atomics.add(sharedData, 3, 1);

    // Wait for no readers and no other writers
    while (
      Atomics.load(sharedData, 1) > 0 ||
      Atomics.load(sharedData, 2) > 0
    ) {
      // Use a short timeout to avoid burning CPU while waiting
      await sleep(10);
    }

    // We're now ready, become active writer
    Atomics.sub(sharedData, 3, 1);
    Atomics.add(sharedData, 2, 1);
  };

  const releaseWriteLock = () => {
    Atomics.sub(sharedData, 2, 1);
  };

  const runReader = async () => {
    while (true) {
      await sleep(Math.random() * 500 + 200);
      
      await acquireReadLock();
      const value = sharedData[0];
      parentPort.postMessage({ type: "read", threadId, value });
      await sleep(100); // Simulate read time
      releaseReadLock();
    }
  };

  const runWriter = async () => {
    while (true) {
      await sleep(Math.random() * 1000 + 500);
      
      await acquireWriteLock();
      sharedData[0]++;
      const value = sharedData[0];
      parentPort.postMessage({ type: "write", threadId, value });
      await sleep(200); // Simulate write time
      releaseWriteLock();
    }
  };

  if (type === "reader") {
    runReader();
  } else if (type === "writer") {
    runWriter();
  }
}
