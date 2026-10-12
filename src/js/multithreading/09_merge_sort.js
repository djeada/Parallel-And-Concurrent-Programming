/*
 * Parallel Merge Sort with Worker Threads
 *
 * This script demonstrates parallelizing the merge sort algorithm using
 * worker threads. It divides the array into chunks, sorts each chunk
 * in a separate worker, then merges the results.
 *
 * Key concepts:
 * - Divide and conquer parallelization
 * - Worker thread pool for parallel chunk sorting
 * - Performance comparison: single-threaded vs multi-threaded
 */

"use strict";

const { Worker, isMainThread, parentPort } = require("worker_threads");
const os = require("os");

const NUM_CPUS = Math.max(1, Math.min(os.availableParallelism?.() ?? os.cpus().length, 4));

const merge = (left, right) => {
  const result = new Array(left.length + right.length);
  let leftIdx = 0;
  let rightIdx = 0;
  let resultIdx = 0;

  while (leftIdx < left.length && rightIdx < right.length) {
    if (left[leftIdx] < right[rightIdx]) {
      result[resultIdx++] = left[leftIdx++];
    } else {
      result[resultIdx++] = right[rightIdx++];
    }
  }

  while (leftIdx < left.length) {
    result[resultIdx++] = left[leftIdx++];
  }
  
  while (rightIdx < right.length) {
    result[resultIdx++] = right[rightIdx++];
  }

  return result;
};

const mergeSort = (arr) => {
  if (arr.length <= 1) {
    return arr;
  }

  const mid = Math.floor(arr.length / 2);
  const left = mergeSort(arr.slice(0, mid));
  const right = mergeSort(arr.slice(mid));

  return merge(left, right);
};

const parallelMergeSort = async (arr) => {
  if (arr.length <= 1) return [...arr];
  const chunkSize = Math.ceil(arr.length / NUM_CPUS);
  const chunks = [];

  // Split array into chunks
  for (let i = 0; i < arr.length; i += chunkSize) {
    chunks.push(arr.slice(i, i + chunkSize));
  }

  // Every worker belongs to this operation, including when another one fails.
  const workers = [];
  let sortedChunks;
  try {
    const sortPromises = chunks.map((chunk) => {
      return new Promise((resolve, reject) => {
        const worker = new Worker(__filename);
        workers.push(worker);

        let received = false;
        worker.once("message", (sortedChunk) => {
          received = true;
          resolve(sortedChunk);
        });
        worker.once("exit", (code) => {
          if (!received) reject(new Error(`Sort worker exited before returning a result (code ${code})`));
        });
        worker.once("error", reject);
        worker.postMessage(chunk);
      });
    });
    sortedChunks = await Promise.all(sortPromises);
  } finally {
    // Promise.all rejects immediately; pending peers must be stopped explicitly.
    await Promise.all(workers.map((worker) => worker.terminate()));
  }

  // Merge all sorted chunks
  return sortedChunks.reduce((acc, chunk) => merge(acc, chunk), []);
};

const generateRandomArray = (size) => {
  return Array.from({ length: size }, () => Math.floor(Math.random() * 100000));
};

const isSorted = (arr) => {
  for (let i = 1; i < arr.length; i++) {
    if (arr[i] < arr[i - 1]) return false;
  }
  return true;
};

const main = async () => {
  console.log("=== Parallel Merge Sort Demo ===\n");
  console.log(`CPU cores: ${NUM_CPUS}`);

  const ARRAY_SIZE = 100000;
  const arr = generateRandomArray(ARRAY_SIZE);
  console.log(`Array size: ${ARRAY_SIZE.toLocaleString()} elements\n`);

  // Single-threaded merge sort
  console.log("--- Single-threaded Merge Sort ---");
  const singleStart = Date.now();
  const singleSorted = mergeSort([...arr]);
  const singleElapsed = Date.now() - singleStart;
  console.log(`Time: ${singleElapsed}ms`);
  console.log(`Correctly sorted: ${isSorted(singleSorted)}\n`);

  // Parallel merge sort
  console.log("--- Parallel Merge Sort ---");
  const parallelStart = Date.now();
  const parallelSorted = await parallelMergeSort([...arr]);
  const parallelElapsed = Date.now() - parallelStart;
  console.log(`Time: ${parallelElapsed}ms`);
  console.log(`Correctly sorted: ${isSorted(parallelSorted)}\n`);

  // Summary
  console.log("=== Performance Summary ===");
  console.log(`Single-threaded: ${singleElapsed}ms`);
  console.log(`Parallel:        ${parallelElapsed}ms`);
  
  if (parallelElapsed < singleElapsed) {
    const speedup = (singleElapsed / parallelElapsed).toFixed(2);
    console.log(`Speedup:         ${speedup}x`);
  } else {
    console.log("Note: Parallel may be slower for small arrays due to thread overhead");
  }
};

if (isMainThread) {
  if (require.main === module) main().catch((error) => {
    console.error(error);
    process.exitCode = 1;
  });
} else {
  parentPort.once("message", (arr) => {
    const sorted = mergeSort(arr);
    parentPort.postMessage(sorted);
    parentPort.close();
  });
}

module.exports = { mergeSort, parallelMergeSort };
