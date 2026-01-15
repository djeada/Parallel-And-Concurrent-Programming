/*
 * Parallel HTTP Fetching with Native Fetch API
 * 
 * This script demonstrates fetching multiple URLs in parallel vs sequentially.
 * It uses the native fetch API (Node.js 18+) for modern HTTP requests.
 * 
 * Key concepts:
 * - Native fetch API usage
 * - Sequential vs parallel request patterns
 * - Error handling with try/catch
 * - Performance comparison
 */

"use strict";

const URLS = [
  "https://jsonplaceholder.typicode.com/posts/1",
  "https://jsonplaceholder.typicode.com/posts/2",
  "https://jsonplaceholder.typicode.com/posts/3",
];

const fetchWithTimeout = async (url, timeoutMs = 5000) => {
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), timeoutMs);

  try {
    const response = await fetch(url, { signal: controller.signal });
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }
    return await response.json();
  } finally {
    clearTimeout(timeout);
  }
};

const fetchSequential = async (urls) => {
  console.log("\n=== Sequential Fetching ===");
  const startTime = Date.now();
  const results = [];

  for (const url of urls) {
    try {
      console.log(`  Fetching: ${url}`);
      const data = await fetchWithTimeout(url);
      results.push({ url, success: true, title: data.title });
      console.log(`  Received: "${data.title?.substring(0, 40)}..."`);
    } catch (error) {
      results.push({ url, success: false, error: error.message });
      console.log(`  Error: ${error.message}`);
    }
  }

  const elapsed = Date.now() - startTime;
  console.log(`Sequential fetching took ${elapsed}ms`);
  return { results, elapsed };
};

const fetchParallel = async (urls) => {
  console.log("\n=== Parallel Fetching ===");
  const startTime = Date.now();

  const promises = urls.map(async (url) => {
    try {
      console.log(`  Starting: ${url}`);
      const data = await fetchWithTimeout(url);
      console.log(`  Received: "${data.title?.substring(0, 40)}..."`);
      return { url, success: true, title: data.title };
    } catch (error) {
      console.log(`  Error for ${url}: ${error.message}`);
      return { url, success: false, error: error.message };
    }
  });

  const results = await Promise.all(promises);
  const elapsed = Date.now() - startTime;
  console.log(`Parallel fetching took ${elapsed}ms`);
  return { results, elapsed };
};

const fetchWithSettled = async (urls) => {
  console.log("\n=== Using Promise.allSettled ===");
  const startTime = Date.now();

  const promises = urls.map((url) => fetchWithTimeout(url));
  const settled = await Promise.allSettled(promises);

  const results = settled.map((result, i) => ({
    url: urls[i],
    success: result.status === "fulfilled",
    data: result.status === "fulfilled" ? result.value : null,
    error: result.status === "rejected" ? result.reason.message : null,
  }));

  const elapsed = Date.now() - startTime;
  const successful = results.filter((r) => r.success).length;
  console.log(`Promise.allSettled completed in ${elapsed}ms`);
  console.log(`Success: ${successful}/${urls.length} requests`);
  return { results, elapsed };
};

const main = async () => {
  console.log("=== Parallel Fetch Demo ===");
  console.log(`URLs to fetch: ${URLS.length}`);

  try {
    const sequential = await fetchSequential(URLS);
    const parallel = await fetchParallel(URLS);
    await fetchWithSettled(URLS);

    console.log("\n=== Performance Summary ===");
    console.log(`Sequential: ${sequential.elapsed}ms`);
    console.log(`Parallel: ${parallel.elapsed}ms`);
    console.log(
      `Speedup: ${(sequential.elapsed / parallel.elapsed).toFixed(2)}x faster with parallel`
    );
  } catch (error) {
    console.error("Error:", error.message);
  }
};

main().catch(console.error);
