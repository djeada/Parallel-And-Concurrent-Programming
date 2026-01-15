/*
 * Process Semaphore Pattern
 *
 * This script demonstrates a semaphore that limits concurrent access to a
 * resource across multiple processes. Unlike a mutex (limit=1), a semaphore
 * allows multiple processes to access a resource simultaneously up to a limit.
 *
 * Key concepts:
 * - Semaphore as a counting mechanism
 * - Message-based semaphore coordination
 * - Concurrent access limiting
 * - Process coordination
 */

"use strict";

const { fork } = require("child_process");

const SEMAPHORE_LIMIT = 2;
const NUM_WORKERS = 5;

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv[2] === "worker") {
  // Worker process
  const workerId = parseInt(process.argv[3], 10);
  
  process.send({ type: "acquire", workerId });
  
  process.on("message", async (message) => {
    if (message.type === "granted") {
      console.log(`  [Worker ${workerId}] Acquired semaphore, starting work...`);
      
      // Simulate some work
      const workTime = Math.floor(Math.random() * 1000) + 500;
      await sleep(workTime);
      
      console.log(`  [Worker ${workerId}] Work completed (${workTime}ms), releasing semaphore`);
      process.send({ type: "release", workerId });
      process.exit(0);
    }
  });

} else {
  // Main process - semaphore manager
  console.log("=== Process Semaphore Demo ===\n");
  console.log(`Semaphore limit: ${SEMAPHORE_LIMIT}`);
  console.log(`Number of workers: ${NUM_WORKERS}\n`);

  let semaphoreCount = SEMAPHORE_LIMIT;
  const waitQueue = [];
  const workers = [];
  let completedCount = 0;
  const startTime = Date.now();

  const tryGrant = () => {
    while (semaphoreCount > 0 && waitQueue.length > 0) {
      const { worker, workerId } = waitQueue.shift();
      semaphoreCount--;
      console.log(`[Manager] Granting semaphore to worker ${workerId} (${semaphoreCount}/${SEMAPHORE_LIMIT} available)`);
      worker.send({ type: "granted" });
    }
  };

  // Create worker processes
  for (let i = 0; i < NUM_WORKERS; i++) {
    const worker = fork(__filename, ["worker", i.toString()]);
    
    worker.on("message", (message) => {
      if (message.type === "acquire") {
        console.log(`[Manager] Worker ${message.workerId} requesting semaphore`);
        waitQueue.push({ worker, workerId: message.workerId });
        tryGrant();
      } else if (message.type === "release") {
        semaphoreCount++;
        console.log(`[Manager] Worker ${message.workerId} released semaphore (${semaphoreCount}/${SEMAPHORE_LIMIT} available)`);
        tryGrant();
      }
    });

    worker.on("exit", () => {
      completedCount++;
      
      if (completedCount === NUM_WORKERS) {
        const elapsed = Date.now() - startTime;
        console.log("\n=== Results ===");
        console.log(`All ${NUM_WORKERS} workers completed in ${elapsed}ms`);
        console.log(`Maximum concurrent workers: ${SEMAPHORE_LIMIT}`);
      }
    });

    workers.push(worker);
  }
}
