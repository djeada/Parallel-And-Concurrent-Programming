/*
 * Creating and Managing Async Tasks (Futures/Promises)
 * 
 * This script demonstrates how to create and manage asynchronous tasks in JavaScript.
 * It compares synchronous (blocking) execution with asynchronous (non-blocking)
 * execution using Promises - JavaScript's equivalent to Futures in other languages.
 * 
 * Key concepts:
 * - Promise.all() for parallel task execution
 * - Blocking vs non-blocking operations
 * - Performance measurement with performance.now()
 */

"use strict";

const { performance } = require("perf_hooks");

const COMPUTATION_DELAY_MS = 2000;

const slowSquareSync = (x) => {
  console.log(`  Starting slow square computation for ${x}`);
  const startTime = Date.now();
  while (Date.now() - startTime < COMPUTATION_DELAY_MS) {
    // Busy wait (blocking)
  }
  const result = x * x;
  console.log(`  Finished slow square computation for ${x} = ${result}`);
  return result;
};

const slowSquareAsync = async (x) => {
  console.log(`  Starting slow square computation for ${x}`);
  await new Promise((resolve) => setTimeout(resolve, COMPUTATION_DELAY_MS));
  const result = x * x;
  console.log(`  Finished slow square computation for ${x} = ${result}`);
  return result;
};

const synchronousExecution = () => {
  console.log("\n=== Synchronous Execution ===");
  const startTime = performance.now();

  const result1 = slowSquareSync(3);
  const result2 = slowSquareSync(4);

  const elapsedTime = (performance.now() - startTime) / 1000;
  console.log(`Synchronous execution took ${elapsedTime.toFixed(2)} seconds.`);
  console.log(`Results: [${result1}, ${result2}]`);
};

const asynchronousExecution = async () => {
  console.log("\n=== Asynchronous Execution ===");
  const startTime = performance.now();

  const results = await Promise.all([slowSquareAsync(3), slowSquareAsync(4)]);

  const elapsedTime = (performance.now() - startTime) / 1000;
  console.log(`Asynchronous execution took ${elapsedTime.toFixed(2)} seconds.`);
  console.log(`Results: [${results.join(", ")}]`);
};

const main = async () => {
  synchronousExecution();
  await asynchronousExecution();
  
  console.log("\n=== Summary ===");
  console.log("Synchronous: ~4 seconds (tasks run sequentially)");
  console.log("Asynchronous: ~2 seconds (tasks run in parallel)");
};

main().catch(console.error);
