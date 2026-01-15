/*
 * Thread Pool Pattern
 *
 * This script demonstrates a simple thread pool implementation.
 * A fixed number of workers process tasks from a queue, maximizing
 * resource utilization without creating too many threads.
 *
 * Key concepts:
 * - Fixed pool of worker threads
 * - Task queue processing
 * - Worker reuse for multiple tasks
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
  console.log("=== Thread Pool Demo ===\n");

  const NUM_WORKERS = 3;
  const NUM_TASKS = 10;

  console.log(`Worker pool size: ${NUM_WORKERS}`);
  console.log(`Total tasks: ${NUM_TASKS}\n`);

  const taskQueue = Array.from({ length: NUM_TASKS }, (_, i) => ({
    id: i,
    data: Math.floor(Math.random() * 100),
  }));

  const results = [];
  let activeTasks = 0;
  const startTime = Date.now();

  const processNextTask = (worker, workerId) => {
    const task = taskQueue.shift();
    if (!task) {
      worker.terminate();
      return;
    }

    activeTasks++;
    console.log(`  [Worker ${workerId}] Starting task ${task.id}`);
    worker.postMessage(task);
  };

  // Create worker pool
  for (let i = 0; i < NUM_WORKERS; i++) {
    const worker = new Worker(__filename);
    const workerId = i;

    worker.on("message", (result) => {
      activeTasks--;
      console.log(`  [Worker ${workerId}] Completed task ${result.taskId} (result: ${result.result})`);
      results.push(result);

      if (taskQueue.length > 0) {
        // Process next task
        processNextTask(worker, workerId);
      } else if (results.length === NUM_TASKS) {
        // All tasks done
        const elapsed = Date.now() - startTime;
        console.log(`\n=== Results ===`);
        console.log(`Total tasks completed: ${results.length}`);
        console.log(`Total time: ${elapsed}ms`);
        console.log(`Average time per task: ${(elapsed / NUM_TASKS).toFixed(0)}ms`);
        worker.terminate();
      } else {
        worker.terminate();
      }
    });

    worker.on("error", (error) => {
      console.error(`Worker ${workerId} error:`, error.message);
    });

    // Start processing
    processNextTask(worker, workerId);
  }

} else {
  parentPort.on("message", async (task) => {
    // Simulate variable processing time
    const processingTime = Math.floor(Math.random() * 1000) + 500;
    await sleep(processingTime);

    // Calculate result
    const result = {
      taskId: task.id,
      result: task.data * 2,
      processingTime,
    };

    parentPort.postMessage(result);
  });
}
