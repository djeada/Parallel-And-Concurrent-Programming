/*
 * Multiple Process Creation
 *
 * This script demonstrates creating and managing multiple child processes.
 * Each child process runs independently and can execute tasks in parallel,
 * taking advantage of multiple CPU cores.
 *
 * Key concepts:
 * - Creating multiple child processes with fork()
 * - Passing arguments to child processes
 * - Tracking completion of all child processes
 * - Process isolation (each process has its own memory space)
 */

"use strict";

const { fork } = require("child_process");
const { threadId } = require("worker_threads");

const NUM_PROCESSES = 3;

const task = (processNum) => {
  console.log(`  [Process ${processNum}] Started (PID: ${process.pid}, Thread: ${threadId})`);
  
  // Simulate some work
  const startTime = Date.now();
  let sum = 0;
  for (let i = 0; i < 1000000; i++) {
    sum += Math.sqrt(i);
  }
  const elapsed = Date.now() - startTime;
  
  console.log(`  [Process ${processNum}] Completed in ${elapsed}ms`);
};

const main = () => {
  console.log("=== Multiple Process Demo ===\n");
  console.log(`Main process ID: ${process.pid}`);
  console.log(`Creating ${NUM_PROCESSES} child processes...\n`);

  const startTime = Date.now();
  let completedCount = 0;

  for (let i = 1; i <= NUM_PROCESSES; i++) {
    const childProcess = fork(__filename, ["worker", i.toString()]);

    childProcess.on("error", (error) => {
      console.error(`Process ${i} error: ${error.message}`);
    });

    childProcess.on("exit", (code) => {
      completedCount++;
      
      if (completedCount === NUM_PROCESSES) {
        const totalTime = Date.now() - startTime;
        console.log(`\n=== All ${NUM_PROCESSES} processes completed in ${totalTime}ms ===`);
      }
    });
  }
};

if (process.argv[2] === "worker") {
  const processNum = parseInt(process.argv[3], 10);
  task(processNum);
} else {
  main();
}
