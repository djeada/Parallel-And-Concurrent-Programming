/*
 * Basic Process Creation with child_process.fork()
 *
 * This script demonstrates the fundamentals of creating child processes in Node.js
 * using the fork() method. Unlike worker threads, child processes have their own
 * memory space and Node.js instance.
 *
 * Key concepts:
 * - fork(): Creates a new Node.js process
 * - process.pid: Each process has a unique process ID
 * - IPC (Inter-Process Communication) is automatically set up with fork()
 * - Child processes are useful for CPU-intensive tasks
 */

"use strict";

const { fork } = require("child_process");
const { threadId } = require("worker_threads");

const workerFunction = () => {
  console.log("  Worker process started");
  console.log(`  Worker process ID: ${process.pid}`);
  console.log(`  Worker thread ID: ${threadId}`);
  console.log("  Worker process finished");
};

const main = () => {
  console.log("=== Basic Process Creation Demo ===\n");
  console.log("Main process started");
  console.log(`Main process ID: ${process.pid}`);
  console.log(`Main thread ID: ${threadId}`);
  console.log("\nForking child process...\n");

  const childProcess = fork(__filename, ["worker"]);

  childProcess.on("error", (error) => {
    console.error(`Child process error: ${error.message}`);
  });

  childProcess.on("exit", (code) => {
    console.log(`\nChild process exited with code: ${code}`);
    console.log("\n=== Demo Complete ===");
  });
};

if (process.argv[2] === "worker") {
  workerFunction();
} else {
  main();
}
