/*
 * Async Generators and Iterators
 * 
 * This script demonstrates async generators and async iteration in JavaScript.
 * Async generators allow you to produce values asynchronously over time,
 * combining the power of generators with async/await.
 * 
 * Use cases:
 * - Streaming data processing
 * - Paginated API requests
 * - Processing large datasets in chunks
 * - Real-time data feeds
 * 
 * Key concepts:
 * - async function* syntax
 * - yield with await
 * - for await...of iteration
 * - AsyncIterator protocol
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// Simple async generator - produces values with delays
async function* countWithDelay(max, delayMs) {
  for (let i = 0; i < max; i++) {
    await sleep(delayMs);
    yield { value: i, timestamp: Date.now() };
  }
}

// Simulates paginated API responses
async function* fetchPages(totalPages, pageSize = 10) {
  for (let page = 1; page <= totalPages; page++) {
    console.log(`  Fetching page ${page}...`);
    await sleep(500); // Simulate network delay
    
    const items = Array.from({ length: pageSize }, (_, i) => ({
      id: (page - 1) * pageSize + i + 1,
      page,
    }));
    
    yield {
      page,
      items,
      hasMore: page < totalPages,
    };
  }
}

// Transforms async iterator - doubles values
async function* transformAsync(asyncIterable, transformFn) {
  for await (const item of asyncIterable) {
    yield transformFn(item);
  }
}

// Filters async iterator
async function* filterAsync(asyncIterable, predicateFn) {
  for await (const item of asyncIterable) {
    if (predicateFn(item)) {
      yield item;
    }
  }
}

// Collects all values from async iterator
const collectAsync = async (asyncIterable) => {
  const results = [];
  for await (const item of asyncIterable) {
    results.push(item);
  }
  return results;
};

const main = async () => {
  console.log("=== Async Generator Demo ===\n");

  // Demo 1: Simple async generator
  console.log("--- Simple Counter ---");
  for await (const item of countWithDelay(5, 300)) {
    console.log(`  Received: ${item.value}`);
  }

  // Demo 2: Paginated data
  console.log("\n--- Paginated API Simulation ---");
  let totalItems = 0;
  for await (const response of fetchPages(3, 5)) {
    console.log(`  Page ${response.page}: ${response.items.length} items`);
    totalItems += response.items.length;
  }
  console.log(`  Total items received: ${totalItems}`);

  // Demo 3: Transform and filter
  console.log("\n--- Transform and Filter ---");
  const source = countWithDelay(10, 100);
  const doubled = transformAsync(source, (item) => ({ ...item, value: item.value * 2 }));
  const evenOnly = filterAsync(doubled, (item) => item.value % 4 === 0);
  
  const results = await collectAsync(evenOnly);
  console.log(`  Filtered results (even after doubling): ${results.map(r => r.value).join(", ")}`);

  // Demo 4: Custom async iterator
  console.log("\n--- Custom AsyncIterator ---");
  const customAsyncIterable = {
    [Symbol.asyncIterator]() {
      let count = 0;
      return {
        async next() {
          if (count >= 3) {
            return { done: true };
          }
          await sleep(200);
          return { done: false, value: `item-${++count}` };
        },
      };
    },
  };

  for await (const item of customAsyncIterable) {
    console.log(`  Custom iterator: ${item}`);
  }

  console.log("\n=== Demo Complete ===");
};

main().catch(console.error);
