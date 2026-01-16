/*
 * Data Sharing with Async Queue
 * 
 * This script demonstrates data sharing and communication between asynchronous
 * tasks using an async queue. It implements a simple producer-consumer pattern
 * where one task produces data and another consumes it.
 * 
 * Key concepts:
 * - Async queue implementation with Promises
 * - Producer-consumer pattern
 * - Data sharing between async tasks
 * - Optional: bounded queue with backpressure
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class AsyncQueue {
  constructor(maxSize = Infinity) {
    this.maxSize = maxSize;
    this.items = [];
    this.waitingConsumers = [];
    this.waitingProducers = [];
  }

  async put(item) {
    // If queue is full, wait for space
    while (this.items.length >= this.maxSize) {
      await new Promise((resolve) => this.waitingProducers.push(resolve));
    }

    // If consumers are waiting, give directly to consumer
    if (this.waitingConsumers.length > 0) {
      const resolve = this.waitingConsumers.shift();
      resolve(item);
    } else {
      this.items.push(item);
    }
  }

  async get() {
    // If items available, return immediately
    if (this.items.length > 0) {
      const item = this.items.shift();
      // Notify one waiting producer if any
      if (this.waitingProducers.length > 0) {
        const resolve = this.waitingProducers.shift();
        resolve();
      }
      return item;
    }

    // Wait for item to be available
    return new Promise((resolve) => this.waitingConsumers.push(resolve));
  }

  get size() {
    return this.items.length;
  }

  get isEmpty() {
    return this.items.length === 0;
  }
}

const produceData = () => {
  const data = Math.floor(Math.random() * 100) + 1;
  return data;
};

const producer = async (queue, producerId, numItems) => {
  for (let i = 0; i < numItems; i++) {
    const data = produceData();
    await queue.put(data);
    console.log(`  Producer ${producerId}: produced ${data}`);
    await sleep(Math.random() * 500 + 100);
  }
  console.log(`  Producer ${producerId}: finished`);
};

const consumer = async (queue, consumerId, numItems) => {
  for (let i = 0; i < numItems; i++) {
    const data = await queue.get();
    console.log(`  Consumer ${consumerId}: consumed ${data}`);
    await sleep(Math.random() * 500 + 200);
  }
  console.log(`  Consumer ${consumerId}: finished`);
};

const main = async () => {
  console.log("=== Async Queue Data Sharing Demo ===\n");

  // Simple example
  console.log("--- Simple Producer-Consumer ---");
  const simpleQueue = new AsyncQueue();
  
  const data = produceData();
  await simpleQueue.put(data);
  console.log(`Produced: ${data}`);
  
  const consumed = await simpleQueue.get();
  console.log(`Consumed: ${consumed}`);

  // Multiple producers and consumers
  console.log("\n--- Multiple Producers and Consumers ---");
  const queue = new AsyncQueue(5); // Bounded queue

  const numProducers = 2;
  const numConsumers = 2;
  const itemsPerActor = 3;

  const producers = Array.from({ length: numProducers }, (_, i) =>
    producer(queue, i, itemsPerActor)
  );

  const consumers = Array.from({ length: numConsumers }, (_, i) =>
    consumer(queue, i, itemsPerActor)
  );

  await Promise.all([...producers, ...consumers]);

  console.log("\n=== All producers and consumers finished ===");
};

main().catch(console.error);
