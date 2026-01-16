/*
 * Semaphore Pattern for Async Concurrency Control
 * 
 * This script demonstrates the use of a semaphore to limit concurrent access to a 
 * resource in an asynchronous environment. A semaphore allows a specified number 
 * of tasks to access a resource simultaneously, while other tasks must wait until 
 * a slot becomes available.
 * 
 * Key concepts:
 * - Semaphore implementation using Promises
 * - Limiting concurrent access (e.g., connection pools, rate limiting)
 * - FIFO queue for fair access
 * - try/finally pattern for guaranteed release
 */

"use strict";

class Semaphore {
  constructor(maxConcurrent) {
    if (maxConcurrent < 1) {
      throw new Error("Semaphore maxConcurrent must be at least 1");
    }
    this.maxConcurrent = maxConcurrent;
    this.current = 0;
    this.queue = [];
  }

  async acquire() {
    if (this.current < this.maxConcurrent) {
      this.current++;
      return;
    }

    return new Promise((resolve) => {
      this.queue.push(resolve);
    });
  }

  release() {
    if (this.queue.length > 0) {
      const resolve = this.queue.shift();
      resolve();
    } else {
      this.current--;
    }
  }

  get available() {
    return this.maxConcurrent - this.current;
  }

  get waiting() {
    return this.queue.length;
  }
}

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const limitedResource = async (semaphore, taskId) => {
  const waitStart = Date.now();
  
  await semaphore.acquire();
  const waitTime = Date.now() - waitStart;
  
  try {
    console.log(
      `  Task ${taskId}: acquired resource (waited ${waitTime}ms, ` +
      `${semaphore.available}/${semaphore.maxConcurrent} slots available)`
    );
    
    // Simulate resource usage
    const workTime = Math.floor(Math.random() * 1500) + 500;
    await sleep(workTime);
    
    console.log(`  Task ${taskId}: releasing resource after ${workTime}ms of work`);
  } finally {
    semaphore.release();
  }
};

const main = async () => {
  const MAX_CONCURRENT = 3;
  const NUM_TASKS = 10;

  console.log("=== Semaphore Demo ===");
  console.log(`Max concurrent access: ${MAX_CONCURRENT}`);
  console.log(`Number of tasks: ${NUM_TASKS}\n`);

  const semaphore = new Semaphore(MAX_CONCURRENT);
  const startTime = Date.now();

  const tasks = Array.from({ length: NUM_TASKS }, (_, i) =>
    limitedResource(semaphore, i)
  );

  await Promise.all(tasks);

  const totalTime = Date.now() - startTime;
  console.log(`\nAll tasks completed in ${totalTime}ms`);
  console.log(
    `Without semaphore, all tasks would start immediately.`
  );
  console.log(
    `With semaphore (limit ${MAX_CONCURRENT}), tasks were properly queued.`
  );
};

main().catch(console.error);
