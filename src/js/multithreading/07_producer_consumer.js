/*
 * Producer-Consumer Pattern with Worker Threads
 *
 * This script demonstrates the classic producer-consumer pattern using
 * shared memory and Atomics for synchronization.
 *
 * Key concepts:
 * - Circular buffer implementation in SharedArrayBuffer
 * - Producer/consumer synchronization without blocking the event loop
 * - Atomic operations for thread-safe queue management
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

const NUM_PRODUCERS = 2;
const NUM_CONSUMERS = 2;
const QUEUE_SIZE = 10;
const ITEMS_PER_PRODUCER = 5;

if (isMainThread) {
  console.log("=== Producer-Consumer Pattern Demo ===\n");
  console.log(`Producers: ${NUM_PRODUCERS}, Consumers: ${NUM_CONSUMERS}`);
  console.log(`Queue size: ${QUEUE_SIZE}, Items per producer: ${ITEMS_PER_PRODUCER}\n`);

  // Shared buffer layout:
  // [0..QUEUE_SIZE-1]: queue items
  // QUEUE_SIZE: current count of items
  // QUEUE_SIZE+1: write index
  // QUEUE_SIZE+2: read index
  const buffer = new SharedArrayBuffer(Int32Array.BYTES_PER_ELEMENT * (QUEUE_SIZE + 3));
  const queue = new Int32Array(buffer);

  const workers = [];
  let producedCount = 0;
  let consumedCount = 0;
  const totalItems = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

  // Create producers
  for (let i = 0; i < NUM_PRODUCERS; i++) {
    const worker = new Worker(__filename, {
      workerData: {
        buffer,
        role: "producer",
        index: i,
        queueSize: QUEUE_SIZE,
        itemsToProduce: ITEMS_PER_PRODUCER,
      },
    });

    worker.on("message", (message) => {
      if (message.type === "produced") {
        producedCount++;
        console.log(`  [Producer ${message.producerIndex}] Produced item: ${message.value}`);
      } else if (message.type === "done") {
        console.log(`  [Producer ${message.index}] Finished producing`);
      }
    });

    worker.on("error", (error) => console.error(`Producer ${i} error:`, error));
    workers.push(worker);
  }

  // Create consumers
  for (let i = 0; i < NUM_CONSUMERS; i++) {
    const worker = new Worker(__filename, {
      workerData: {
        buffer,
        role: "consumer",
        index: i,
        queueSize: QUEUE_SIZE,
        itemsToConsume: Math.ceil(totalItems / NUM_CONSUMERS),
      },
    });

    worker.on("message", (message) => {
      if (message.type === "consumed") {
        consumedCount++;
        console.log(`  [Consumer ${message.consumerIndex}] Consumed item: ${message.value}`);
        
        if (consumedCount >= totalItems) {
          console.log(`\n=== All ${totalItems} items produced and consumed ===`);
          workers.forEach((w) => w.terminate());
        }
      }
    });

    worker.on("error", (error) => console.error(`Consumer ${i} error:`, error));
    workers.push(worker);
  }
} else {
  const { buffer, role, index, queueSize, itemsToProduce, itemsToConsume } = workerData;
  const queue = new Int32Array(buffer);

  const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

  const put = (item) => {
    // Wait for space in queue using Atomics.wait for efficiency
    while (Atomics.load(queue, queueSize) >= queueSize) {
      Atomics.wait(queue, queueSize, queueSize, 50);
    }

    const writeIdx = Atomics.load(queue, queueSize + 1) % queueSize;
    Atomics.store(queue, writeIdx, item);
    Atomics.add(queue, queueSize + 1, 1);
    Atomics.add(queue, queueSize, 1);
    Atomics.notify(queue, queueSize, 1); // Notify consumers
  };

  const get = () => {
    // Wait for items in queue using Atomics.wait for efficiency
    while (Atomics.load(queue, queueSize) <= 0) {
      Atomics.wait(queue, queueSize, 0, 50);
    }

    const readIdx = Atomics.load(queue, queueSize + 2) % queueSize;
    const item = Atomics.load(queue, readIdx);
    Atomics.add(queue, queueSize + 2, 1);
    Atomics.add(queue, queueSize, -1);
    Atomics.notify(queue, queueSize, 1); // Notify producers
    return item;
  };

  const produce = async () => {
    for (let i = 0; i < itemsToProduce; i++) {
      const item = (index + 1) * 100 + i;
      put(item);
      parentPort.postMessage({
        type: "produced",
        producerIndex: index,
        value: item,
      });
      await sleep(Math.random() * 200 + 100);
    }
    parentPort.postMessage({ type: "done", index });
  };

  const consume = async () => {
    for (let i = 0; i < itemsToConsume; i++) {
      // Check if there are items to consume (with timeout)
      let attempts = 0;
      while (Atomics.load(queue, queueSize) <= 0 && attempts < 50) {
        await sleep(100);
        attempts++;
      }
      
      if (Atomics.load(queue, queueSize) <= 0) {
        break; // No more items to consume
      }

      const item = get();
      parentPort.postMessage({
        type: "consumed",
        consumerIndex: index,
        value: item,
      });
      await sleep(Math.random() * 300 + 100);
    }
  };

  if (role === "producer") {
    produce();
  } else if (role === "consumer") {
    consume();
  }
}
