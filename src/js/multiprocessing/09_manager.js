/*
 * Process Manager Pattern
 *
 * This script demonstrates a manager that coordinates multiple worker processes.
 * The manager spawns workers, collects their output, and handles completion.
 * Uses a shared file to demonstrate coordinated output.
 *
 * Key concepts:
 * - Process spawning and management
 * - Coordinated file access
 * - Worker completion tracking
 * - Resource cleanup
 */

"use strict";

const { spawn, fork } = require("child_process");
const fs = require("fs");
const path = require("path");

const SHARED_FILE = path.join("/tmp", `shared_data_${process.pid}.txt`);

class ProcessManager {
  constructor(numProcesses) {
    this.numProcesses = numProcesses;
    this.completedCount = 0;
    this.startTime = Date.now();
  }

  start() {
    console.log("=== Process Manager Demo ===\n");
    console.log(`Managing ${this.numProcesses} worker processes`);
    console.log(`Output file: ${SHARED_FILE}\n`);

    // Initialize the shared file
    fs.writeFileSync(SHARED_FILE, "");

    for (let i = 0; i < this.numProcesses; i++) {
      this.createWorker(i);
    }
  }

  createWorker(id) {
    const worker = fork(__filename, ["worker", id.toString()]);

    worker.on("message", (message) => {
      if (message.type === "output") {
        fs.appendFileSync(SHARED_FILE, message.text + "\n");
        console.log(`  [Worker ${id}] ${message.text}`);
      } else if (message.type === "done") {
        this.handleCompletion(id);
      }
    });

    worker.on("error", (error) => {
      console.error(`  [Worker ${id}] Error: ${error.message}`);
    });

    worker.on("exit", (code) => {
      if (code !== 0) {
        console.error(`  [Worker ${id}] Exited with code ${code}`);
      }
    });
  }

  handleCompletion(workerId) {
    this.completedCount++;
    
    if (this.completedCount === this.numProcesses) {
      this.printResults();
    }
  }

  printResults() {
    const elapsed = Date.now() - this.startTime;
    
    console.log("\n=== Results ===");
    console.log(`All ${this.numProcesses} processes completed in ${elapsed}ms`);
    console.log("\nContents of shared file:");
    
    const data = fs.readFileSync(SHARED_FILE, "utf-8");
    console.log(data);
    
    // Cleanup
    fs.unlinkSync(SHARED_FILE);
    console.log("Cleanup complete.");
  }
}

// Worker process logic
const workerTask = (workerId) => {
  const startTime = Date.now();
  const processingTime = Math.floor(Math.random() * 500) + 200;
  
  setTimeout(() => {
    const elapsed = Date.now() - startTime;
    process.send({
      type: "output",
      text: `Process ${workerId} completed task (PID: ${process.pid}, took ${elapsed}ms)`,
    });
    process.send({ type: "done" });
  }, processingTime);
};

if (process.argv[2] === "worker") {
  const workerId = parseInt(process.argv[3], 10);
  workerTask(workerId);
} else {
  const NUM_PROCESSES = 4;
  const manager = new ProcessManager(NUM_PROCESSES);
  manager.start();
}
