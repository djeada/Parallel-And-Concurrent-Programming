/*
 * Synchronous vs Asynchronous Execution in JavaScript
 * 
 * This program demonstrates the difference between synchronous and asynchronous
 * approaches to handling I/O-bound tasks using Node.js.
 * 
 * - Synchronous: Tasks run sequentially using blocking operations
 * - Asynchronous: Tasks run concurrently using Promises and async/await
 * 
 * The asynchronous approach allows better resource utilization and improved
 * performance when dealing with I/O-bound tasks.
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const syncTask = (taskId, sleepTimeSeconds) => {
  console.log(`Task ${taskId} started.`);
  const start = Date.now();
  while (Date.now() - start < sleepTimeSeconds * 1000) {
    // Busy wait (blocking)
  }
  console.log(`Task ${taskId} finished.`);
};

const syncMain = () => {
  console.log("\n=== Synchronous Execution ===");
  const startTime = Date.now();

  syncTask(1, 2);
  syncTask(2, 1);

  const elapsedTime = (Date.now() - startTime) / 1000;
  console.log(`Synchronous execution took ${elapsedTime.toFixed(2)} seconds.\n`);
};

const asyncTask = async (taskId, sleepTimeSeconds) => {
  console.log(`Task ${taskId} started.`);
  await sleep(sleepTimeSeconds * 1000);
  console.log(`Task ${taskId} finished.`);
};

const asyncMain = async () => {
  console.log("=== Asynchronous Execution ===");
  const startTime = Date.now();

  await Promise.all([asyncTask(1, 2), asyncTask(2, 1)]);

  const elapsedTime = (Date.now() - startTime) / 1000;
  console.log(`Asynchronous execution took ${elapsedTime.toFixed(2)} seconds.\n`);
};

const main = async () => {
  syncMain();
  await asyncMain();
  
  console.log("=== Summary ===");
  console.log("Synchronous: ~3 seconds (2s + 1s sequential)");
  console.log("Asynchronous: ~2 seconds (tasks run in parallel)");
};

main().catch(console.error);
