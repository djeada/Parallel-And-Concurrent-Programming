/*
 * Running Heavy Functions Asynchronously
 * 
 * This script demonstrates how to run multiple heavy/long-running computations
 * concurrently while keeping the event loop responsive. This allows the application
 * to handle user input even while computations are running.
 * 
 * Key concepts:
 * - Non-blocking async operations
 * - Promise.all() for parallel execution
 * - Keeping the event loop responsive
 * - Graceful shutdown handling
 */

"use strict";

const readline = require("readline");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const heavyComputation = async (taskId, durationMs = 5000) => {
  console.log(`  [Task ${taskId}] Starting heavy computation...`);
  const startTime = Date.now();
  
  // Simulate heavy computation with periodic yields to event loop
  const chunks = 10;
  const chunkDuration = durationMs / chunks;
  
  for (let i = 0; i < chunks; i++) {
    await sleep(chunkDuration);
  }
  
  const elapsed = Date.now() - startTime;
  console.log(`  [Task ${taskId}] Completed in ${elapsed}ms`);
  return { taskId, elapsed };
};

const runHeavyFunctions = async (numTasks = 6) => {
  console.log(`\nStarting ${numTasks} heavy computations in parallel...`);
  const startTime = Date.now();

  const tasks = Array.from({ length: numTasks }, (_, i) =>
    heavyComputation(i, 5000)
  );

  const results = await Promise.all(tasks);
  
  const totalElapsed = Date.now() - startTime;
  console.log(`\nAll ${numTasks} computations completed in ${totalElapsed}ms`);
  console.log("Results:", results.map((r) => `Task ${r.taskId}`).join(", "));
  
  return results;
};

const main = async () => {
  console.log("=== Heavy Functions Demo ===");
  console.log("The event loop remains responsive during computation.");
  console.log("Try typing while computations run (press Ctrl+C to exit).\n");

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on("line", (input) => {
    console.log(`  >> User input received: "${input}"`);
  });

  // Graceful shutdown
  const cleanup = () => {
    console.log("\nShutting down...");
    rl.close();
    process.exit(0);
  };

  process.on("SIGINT", cleanup);
  process.on("SIGTERM", cleanup);

  await runHeavyFunctions();

  console.log("\nComputations finished. You can continue to enter text.");
  console.log("Press Ctrl+C to exit.");
};

main().catch(console.error);
