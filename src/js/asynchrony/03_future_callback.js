/*
 * Promises and Callbacks in Asynchronous Programming
 * 
 * This script demonstrates how to use Promises with callbacks in JavaScript.
 * It shows different patterns for handling asynchronous results:
 * 
 * 1. Using .then() callbacks
 * 2. Using async/await with manual callback invocation
 * 3. Chaining operations with Promise.all()
 * 
 * Key concepts:
 * - Promise creation and resolution
 * - Callback patterns for result handling
 * - Parallel vs sequential promise execution
 */

"use strict";

const COMPUTATION_DELAY_MS = 1000;

const slowSquare = (x) => {
  return new Promise((resolve) => {
    console.log(`  Starting slow square computation for ${x}`);
    setTimeout(() => {
      const result = x * x;
      console.log(`  Finished slow square computation for ${x} = ${result}`);
      resolve(result);
    }, COMPUTATION_DELAY_MS);
  });
};

const squareCallback = (input, result) => {
  console.log(`  Callback: square(${input}) = ${result}`);
};

// Pattern 1: Using .then() callbacks
const usingThenCallbacks = async () => {
  console.log("\n=== Pattern 1: Using .then() Callbacks ===");
  const startTime = Date.now();

  await Promise.all([
    slowSquare(3).then((result) => squareCallback(3, result)),
    slowSquare(4).then((result) => squareCallback(4, result)),
  ]);

  const duration = Date.now() - startTime;
  console.log(`Execution time: ${duration}ms`);
};

// Pattern 2: Using async/await
const usingAsyncAwait = async () => {
  console.log("\n=== Pattern 2: Using async/await ===");
  const startTime = Date.now();

  const [result1, result2] = await Promise.all([slowSquare(5), slowSquare(6)]);

  squareCallback(5, result1);
  squareCallback(6, result2);

  const duration = Date.now() - startTime;
  console.log(`Execution time: ${duration}ms`);
};

// Pattern 3: Sequential execution for comparison
const sequentialExecution = async () => {
  console.log("\n=== Pattern 3: Sequential Execution ===");
  const startTime = Date.now();

  const result1 = await slowSquare(7);
  squareCallback(7, result1);

  const result2 = await slowSquare(8);
  squareCallback(8, result2);

  const duration = Date.now() - startTime;
  console.log(`Execution time: ${duration}ms`);
};

const main = async () => {
  await usingThenCallbacks();
  await usingAsyncAwait();
  await sequentialExecution();

  console.log("\n=== Summary ===");
  console.log("Parallel patterns (~1s): Use Promise.all() for concurrent execution");
  console.log("Sequential pattern (~2s): Await each promise individually");
};

main().catch(console.error);
