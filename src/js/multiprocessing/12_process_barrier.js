/*
 * Process Barrier Pattern
 *
 * This script demonstrates a barrier synchronization primitive that ensures
 * all processes reach a certain point before any can continue. This is useful
 * for phased computations where all processes must complete a phase together.
 *
 * Key concepts:
 * - Barrier synchronization
 * - Process coordination
 * - Message-based barrier implementation
 * - Phased execution
 */

"use strict";

const { fork } = require("child_process");

const NUM_WORKERS = 4;

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv[2] === "worker") {
  // Worker process
  const workerId = parseInt(process.argv[3], 10);
  
  const doWork = async () => {
    // Phase 1: Initial work
    const workTime = Math.floor(Math.random() * 2000) + 500;
    console.log(`  [Worker ${workerId}] Starting phase 1 (${workTime}ms work)`);
    await sleep(workTime);
    
    console.log(`  [Worker ${workerId}] Phase 1 complete, waiting at barrier...`);
    process.send({ type: "arrived", workerId });
  };

  process.on("message", async (message) => {
    if (message.type === "start") {
      await doWork();
    } else if (message.type === "released") {
      console.log(`  [Worker ${workerId}] Barrier released! Continuing to phase 2...`);
      
      // Phase 2: Work after barrier
      const postWorkTime = Math.floor(Math.random() * 500) + 200;
      await sleep(postWorkTime);
      
      console.log(`  [Worker ${workerId}] Phase 2 complete`);
      process.send({ type: "done", workerId });
    }
  });

} else {
  // Main process - barrier manager
  console.log("=== Process Barrier Demo ===\n");
  console.log(`Number of workers: ${NUM_WORKERS}`);
  console.log("All workers must complete phase 1 before any can proceed to phase 2\n");

  let arrivedCount = 0;
  let completedCount = 0;
  const workers = [];
  const startTime = Date.now();

  // Create worker processes
  for (let i = 0; i < NUM_WORKERS; i++) {
    const worker = fork(__filename, ["worker", i.toString()]);
    
    worker.on("message", (message) => {
      if (message.type === "arrived") {
        arrivedCount++;
        console.log(`[Barrier] Worker ${message.workerId} arrived (${arrivedCount}/${NUM_WORKERS})`);
        
        if (arrivedCount === NUM_WORKERS) {
          console.log("\n[Barrier] All workers arrived - releasing barrier!\n");
          workers.forEach((w) => w.send({ type: "released" }));
        }
      } else if (message.type === "done") {
        completedCount++;
        
        if (completedCount === NUM_WORKERS) {
          const elapsed = Date.now() - startTime;
          console.log("\n=== Results ===");
          console.log(`All ${NUM_WORKERS} workers completed in ${elapsed}ms`);
          console.log("Barrier ensured synchronized phase transition");
          workers.forEach((w) => w.kill());
        }
      }
    });

    worker.on("error", (error) => {
      console.error(`Worker ${i} error: ${error.message}`);
    });

    workers.push(worker);
  }

  // Start all workers
  workers.forEach((w) => w.send({ type: "start" }));
}
