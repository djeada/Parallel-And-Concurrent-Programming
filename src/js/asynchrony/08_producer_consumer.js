/*
 * Producer-Consumer Pattern with Async Queue
 * 
 * This script demonstrates the producer-consumer pattern using an async queue.
 * Producers create items and add them to a shared queue, while consumers take
 * items from the queue and process them.
 * 
 * Key concepts:
 * - Async queue with blocking get/put operations
 * - Multiple producers and consumers
 * - Proper termination with poison pills
 * - Concurrent task coordination
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class AsyncQueue {
  constructor() {
    this.items = [];
    this.waitingConsumers = [];
  }

  put(item) {
    if (this.waitingConsumers.length > 0) {
      const resolve = this.waitingConsumers.shift();
      resolve(item);
    } else {
      this.items.push(item);
    }
  }

  async get() {
    if (this.items.length > 0) {
      return this.items.shift();
    }
    return new Promise((resolve) => this.waitingConsumers.push(resolve));
  }

  get size() {
    return this.items.length;
  }
}

const POISON_PILL = Symbol("POISON_PILL");

const producer = async (queue, producerId, numItems) => {
  for (let i = 0; i < numItems; i++) {
    const item = { producerId, itemId: i, timestamp: Date.now() };
    queue.put(item);
    console.log(`  Producer ${producerId}: produced item ${i}`);
    await sleep(Math.random() * 1000 + 200);
  }
  console.log(`  Producer ${producerId}: finished producing`);
};

const consumer = async (queue, consumerId) => {
  let consumed = 0;
  
  while (true) {
    const item = await queue.get();
    
    // Poison pill signals this consumer to stop
    if (item === POISON_PILL) {
      console.log(`  Consumer ${consumerId}: received termination signal, stopping`);
      break;
    }
    
    // Simulate processing time
    await sleep(Math.random() * 800 + 100);
    consumed++;
    
    console.log(
      `  Consumer ${consumerId}: processed item ${item.itemId} ` +
      `from producer ${item.producerId}`
    );
  }
  
  console.log(`  Consumer ${consumerId}: finished consuming (${consumed} items)`);
};

const main = async () => {
  console.log("=== Producer-Consumer Pattern Demo ===\n");

  const NUM_PRODUCERS = 3;
  const NUM_CONSUMERS = 3;
  const ITEMS_PER_PRODUCER = 4;

  const queue = new AsyncQueue();
  const startTime = Date.now();

  // Start consumers first so they are ready to receive
  const consumers = Array.from({ length: NUM_CONSUMERS }, (_, i) =>
    consumer(queue, i)
  );

  // Start producers concurrently
  const producers = Array.from({ length: NUM_PRODUCERS }, (_, i) =>
    producer(queue, i, ITEMS_PER_PRODUCER)
  );

  // Wait for all producers to finish, then send one poison pill per consumer
  await Promise.all(producers);
  for (let i = 0; i < NUM_CONSUMERS; i++) {
    queue.put(POISON_PILL);
  }

  // Wait for all consumers to drain and exit
  await Promise.all(consumers);

  const elapsed = Date.now() - startTime;
  console.log(`\n=== Summary ===`);
  console.log(`Total time: ${elapsed}ms`);
  console.log(`Producers: ${NUM_PRODUCERS}, each produced ${ITEMS_PER_PRODUCER} items`);
  console.log(`Consumers: ${NUM_CONSUMERS} total items consumed`);
  console.log(`Queue size at end: ${queue.size}`);
};

main().catch(console.error);
