/*
 * Multiple Worker Threads Example
 *
 * This script demonstrates running multiple worker threads concurrently.
 * It shows how to:
 * - Create multiple workers executing different functions
 * - Pass data to workers via workerData
 * - Track completion of all workers
 *
 * Key concepts:
 * - Concurrent worker execution
 * - Function dispatch based on workerData
 * - Worker coordination and completion tracking
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const foo = async (x) => {
  const { threadId } = workerData;
  parentPort.postMessage(`foo() starting with arg: ${x}`);
  await sleep(1000);
  parentPort.postMessage(`foo() completed with arg: ${x}`);
};

const bar = async (x) => {
  const { threadId } = workerData;
  parentPort.postMessage(`bar() starting with arg: ${x}`);
  await sleep(100);
  parentPort.postMessage(`bar() completed with arg: ${x}`);
};

const main = () => {
  console.log("=== Multiple Worker Threads Demo ===\n");
  console.log("Starting workers...\n");

  const workers = [];
  const NUM_FOO_WORKERS = 5;
  const NUM_BAR_WORKERS = 5;

  // Create foo workers
  for (let i = 0; i < NUM_FOO_WORKERS; i++) {
    const worker = new Worker(__filename, {
      workerData: { functionName: "foo", args: [i], threadId: i },
    });
    worker.on("message", (msg) => console.log(`  [Thread ${i}] ${msg}`));
    worker.on("error", (err) => console.error(`  [Thread ${i}] Error: ${err.message}`));
    workers.push(worker);
  }

  // Create bar workers
  for (let i = NUM_FOO_WORKERS; i < NUM_FOO_WORKERS + NUM_BAR_WORKERS; i++) {
    const worker = new Worker(__filename, {
      workerData: { functionName: "bar", args: [i], threadId: i },
    });
    worker.on("message", (msg) => console.log(`  [Thread ${i}] ${msg}`));
    worker.on("error", (err) => console.error(`  [Thread ${i}] Error: ${err.message}`));
    workers.push(worker);
  }

  let finishedThreads = 0;
  const startTime = Date.now();

  for (const worker of workers) {
    worker.on("exit", (code) => {
      finishedThreads++;
      if (finishedThreads === workers.length) {
        const elapsed = Date.now() - startTime;
        console.log(`\n=== All ${workers.length} workers finished in ${elapsed}ms ===`);
      }
    });
  }
};

if (isMainThread) {
  main();
} else {
  const { functionName, args } = workerData;

  if (functionName === "foo") {
    foo(...args);
  } else if (functionName === "bar") {
    bar(...args);
  }
}
