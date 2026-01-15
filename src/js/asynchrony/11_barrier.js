/*
 * Async Barrier Pattern
 * 
 * This script demonstrates the barrier synchronization pattern for async tasks.
 * A barrier ensures that multiple tasks wait for each other at a synchronization
 * point before any of them can proceed.
 * 
 * Use cases:
 * - Phased computations where all tasks must complete phase N before starting phase N+1
 * - Coordinating parallel initialization
 * - Ensuring all workers are ready before starting main computation
 * 
 * Key concepts:
 * - Barrier implementation using Promises
 * - Reusable barriers for multiple phases
 * - Task synchronization
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class AsyncBarrier {
  constructor(numParties) {
    if (numParties < 1) {
      throw new Error("Barrier requires at least 1 party");
    }
    this.numParties = numParties;
    this.count = 0;
    this.generation = 0;
    this.waiters = [];
  }

  async wait() {
    const currentGeneration = this.generation;
    this.count++;

    if (this.count >= this.numParties) {
      // Last one to arrive - release everyone and reset
      this.count = 0;
      this.generation++;
      
      // Resolve all waiting promises
      const waitersToResolve = this.waiters;
      this.waiters = [];
      waitersToResolve.forEach((resolve) => resolve(true));
      
      return true; // Indicates this was the last one
    }

    // Not the last one - wait for others
    return new Promise((resolve) => {
      this.waiters.push(resolve);
    });
  }

  get waiting() {
    return this.count;
  }

  get parties() {
    return this.numParties;
  }
}

const task = async (taskId, barrier, phases = 2) => {
  for (let phase = 1; phase <= phases; phase++) {
    // Simulate work with random duration
    const workTime = Math.floor(Math.random() * 800) + 200;
    console.log(`  Task ${taskId}: starting phase ${phase} (${workTime}ms work)`);
    
    await sleep(workTime);
    
    console.log(`  Task ${taskId}: finished phase ${phase}, waiting at barrier...`);
    
    const wasLast = await barrier.wait();
    
    if (wasLast) {
      console.log(`  Task ${taskId}: was last to arrive, barrier released!`);
    }
    
    console.log(`  Task ${taskId}: passed barrier, continuing to phase ${phase + 1}`);
  }
  
  console.log(`  Task ${taskId}: completed all phases`);
};

const main = async () => {
  const NUM_TASKS = 4;
  const NUM_PHASES = 3;

  console.log("=== Async Barrier Demo ===");
  console.log(`Tasks: ${NUM_TASKS}, Phases: ${NUM_PHASES}`);
  console.log("All tasks must complete each phase before any can proceed.\n");

  const barrier = new AsyncBarrier(NUM_TASKS);
  const startTime = Date.now();

  const tasks = Array.from({ length: NUM_TASKS }, (_, i) =>
    task(i, barrier, NUM_PHASES)
  );

  await Promise.all(tasks);

  const elapsed = Date.now() - startTime;
  console.log(`\n=== All tasks completed in ${elapsed}ms ===`);
};

main().catch(console.error);
