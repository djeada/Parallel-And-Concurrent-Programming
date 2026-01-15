/*
 * Parallel HTTP Fetching with Worker Threads
 *
 * This script demonstrates parallel HTTP requests using worker threads.
 * It compares sequential vs concurrent fetching performance.
 *
 * Key concepts:
 * - Using native fetch API (Node.js 18+)
 * - Parallel execution with worker threads
 * - Performance comparison between sequential and concurrent approaches
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const URLS = [
  "https://jsonplaceholder.typicode.com/posts/1",
  "https://jsonplaceholder.typicode.com/posts/2",
  "https://jsonplaceholder.typicode.com/posts/3",
  "https://jsonplaceholder.typicode.com/posts/4",
];

const fetchUrl = async (url) => {
  try {
    const response = await fetch(url);
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    const data = await response.json();
    return { url, contentLength: JSON.stringify(data).length, success: true };
  } catch (error) {
    return { url, error: error.message, success: false };
  }
};

const sequentialFetch = async (urls) => {
  console.log("--- Sequential Fetching ---");
  const startTime = Date.now();
  const results = [];

  for (const url of urls) {
    const result = await fetchUrl(url);
    if (result.success) {
      console.log(`  Fetched ${url} (${result.contentLength} chars)`);
    } else {
      console.log(`  Failed ${url}: ${result.error}`);
    }
    results.push(result);
  }

  const elapsed = Date.now() - startTime;
  console.log(`Sequential time: ${elapsed}ms\n`);
  return { results, elapsed };
};

const parallelFetchWithWorkers = async (urls) => {
  console.log("--- Parallel Fetching with Workers ---");
  const startTime = Date.now();

  const workerPromises = urls.map((url) => {
    return new Promise((resolve) => {
      const worker = new Worker(__filename, { workerData: url });
      
      worker.on("message", (result) => {
        if (result.success) {
          console.log(`  Fetched ${result.url} (${result.contentLength} chars)`);
        } else {
          console.log(`  Failed ${result.url}: ${result.error}`);
        }
        resolve(result);
      });
      
      worker.on("error", (error) => {
        resolve({ url, error: error.message, success: false });
      });
    });
  });

  const results = await Promise.all(workerPromises);
  const elapsed = Date.now() - startTime;
  console.log(`Parallel time: ${elapsed}ms\n`);
  return { results, elapsed };
};

const main = async () => {
  console.log("=== Parallel Fetch Demo ===");
  console.log(`URLs to fetch: ${URLS.length}\n`);

  try {
    const sequential = await sequentialFetch(URLS);
    const parallel = await parallelFetchWithWorkers(URLS);

    console.log("=== Performance Summary ===");
    console.log(`Sequential: ${sequential.elapsed}ms`);
    console.log(`Parallel:   ${parallel.elapsed}ms`);
    
    if (parallel.elapsed < sequential.elapsed) {
      const speedup = (sequential.elapsed / parallel.elapsed).toFixed(2);
      console.log(`Speedup:    ${speedup}x faster with parallel workers`);
    }
  } catch (error) {
    console.error("Error:", error.message);
  }
};

if (isMainThread) {
  main();
} else {
  (async () => {
    const result = await fetchUrl(workerData);
    parentPort.postMessage(result);
  })();
}
