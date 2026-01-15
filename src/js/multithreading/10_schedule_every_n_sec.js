/*
 * Scheduled Tasks with Worker Threads
 *
 * This script demonstrates running scheduled/periodic tasks in a worker thread.
 * It shows how to start, control, and stop interval-based execution.
 *
 * Key concepts:
 * - Running periodic tasks in a worker
 * - Two-way communication for task control
 * - Clean shutdown of scheduled tasks
 */

"use strict";

const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  console.log("=== Scheduled Tasks Demo ===\n");
  console.log("Starting worker with 1-second interval...\n");

  const worker = new Worker(__filename);
  let tickCount = 0;

  worker.on("message", (message) => {
    if (message.type === "tick") {
      tickCount++;
      console.log(`  [Tick ${tickCount}] ${message.text} at ${new Date().toISOString()}`);
    } else if (message.type === "stopped") {
      console.log("\nWorker stopped gracefully");
    }
  });

  worker.on("error", (error) => {
    console.error("Worker error:", error.message);
  });

  worker.on("exit", (code) => {
    console.log(`Worker exited with code: ${code}`);
    console.log(`Total ticks received: ${tickCount}`);
    console.log("\n=== Demo Complete ===");
  });

  // Start the scheduled task
  worker.postMessage({ command: "start", interval: 1000 });

  // Stop after 5 seconds
  setTimeout(() => {
    console.log("\nSending stop command...");
    worker.postMessage({ command: "stop" });
  }, 5500);

} else {
  let timer = null;
  let tickNumber = 0;

  parentPort.on("message", (message) => {
    const { command, interval } = message;

    if (command === "start") {
      if (timer) {
        clearInterval(timer);
      }
      
      tickNumber = 0;
      timer = setInterval(() => {
        tickNumber++;
        parentPort.postMessage({
          type: "tick",
          text: `Task executed (tick #${tickNumber})`,
          tickNumber,
        });
      }, interval);
      
    } else if (command === "stop") {
      if (timer) {
        clearInterval(timer);
        timer = null;
      }
      parentPort.postMessage({ type: "stopped" });
      parentPort.close();
    }
  });
}
