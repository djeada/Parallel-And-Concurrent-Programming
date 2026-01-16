/*
 * Process Pool Pattern
 *
 * This script demonstrates a process pool for executing CPU-bound tasks.
 * A process pool maintains a fixed number of worker processes that handle
 * tasks from a queue, improving resource utilization.
 *
 * Key concepts:
 * - Fixed pool of worker processes
 * - Task queue with worker reuse
 * - CPU-bound task parallelization
 * - Process lifecycle management
 */

"use strict";

const { fork } = require("child_process");
const os = require("os");

const NUM_CPUS = os.cpus().length;
const NUM_TASKS = 10;

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// Worker process logic
const workerTask = async (taskId) => {
  const startTime = Date.now();
  const processingTime = Math.floor(Math.random() * 1000) + 500;
  
  console.log(`  [Task ${taskId}] Starting (processing time: ${processingTime}ms)`);
  
  // Simulate CPU-intensive work
  await sleep(processingTime);
  const result = taskId * 2;
  
  const elapsed = Date.now() - startTime;
  console.log(`  [Task ${taskId}] Completed in ${elapsed}ms, result: ${result}`);
  
  return { taskId, result, processingTime };
};

// Main process logic
const main = () => {
  console.log("=== Process Pool Demo ===\n");
  console.log(`CPU cores: ${NUM_CPUS}`);
  console.log(`Total tasks: ${NUM_TASKS}`);
  console.log(`Pool size: ${NUM_CPUS}\n`);

  const taskQueue = Array.from({ length: NUM_TASKS }, (_, i) => i);
  const results = [];
  const startTime = Date.now();
  let activeWorkers = 0;

  const processNextTask = () => {
    if (taskQueue.length === 0) {
      if (activeWorkers === 0) {
        const totalTime = Date.now() - startTime;
        const avgProcessingTime = results.reduce((sum, r) => sum + r.processingTime, 0) / results.length;
        
        console.log("\n=== Results ===");
        console.log(`Total tasks completed: ${results.length}`);
        console.log(`Total wall time: ${totalTime}ms`);
        console.log(`Avg processing time: ${avgProcessingTime.toFixed(0)}ms`);
      }
      return;
    }

    if (activeWorkers >= NUM_CPUS) {
      return;
    }

    const taskId = taskQueue.shift();
    activeWorkers++;

    const worker = fork(__filename, ["worker", taskId.toString()]);

    worker.on("message", (result) => {
      results.push(result);
    });

    worker.on("error", (error) => {
      console.error(`Worker error: ${error.message}`);
    });

    worker.on("exit", (code) => {
      activeWorkers--;
      processNextTask();
    });

    // Start more workers if available
    processNextTask();
  };

  processNextTask();
};

if (process.argv[2] === "worker") {
  const taskId = parseInt(process.argv[3], 10);
  
  workerTask(taskId).then((result) => {
    process.send(result);
    process.exit(0);
  });
} else {
  main();
}
