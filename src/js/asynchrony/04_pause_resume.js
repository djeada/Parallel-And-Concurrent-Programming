/*
 * Pause and Resume Asynchronous Operations
 * 
 * This script demonstrates how to pause and resume function execution using
 * setInterval, clearInterval, and AbortController in Node.js.
 * 
 * Key concepts:
 * - setInterval/clearInterval for periodic tasks
 * - AbortController for cancellation (modern approach)
 * - User input handling with readline
 * - Graceful shutdown handling
 */

"use strict";

const readline = require("readline");

class PausableTask {
  constructor(taskFn, intervalMs = 1000) {
    this.taskFn = taskFn;
    this.intervalMs = intervalMs;
    this.intervalId = null;
    this.isPaused = false;
    this.step = 0;
  }

  start() {
    if (!this.intervalId) {
      console.log("Task started");
      this.intervalId = setInterval(() => {
        this.taskFn(this.step);
        this.step += 1;
      }, this.intervalMs);
    }
  }

  pause() {
    if (this.intervalId && !this.isPaused) {
      clearInterval(this.intervalId);
      this.intervalId = null;
      this.isPaused = true;
      console.log(`Task paused at step ${this.step}`);
    }
  }

  resume() {
    if (this.isPaused) {
      this.isPaused = false;
      this.intervalId = setInterval(() => {
        this.taskFn(this.step);
        this.step += 1;
      }, this.intervalMs);
      console.log(`Task resumed from step ${this.step}`);
    }
  }

  toggle() {
    if (this.isPaused) {
      this.resume();
    } else {
      this.pause();
    }
  }

  stop() {
    if (this.intervalId) {
      clearInterval(this.intervalId);
      this.intervalId = null;
    }
    console.log(`Task stopped at step ${this.step}`);
  }
}

const main = () => {
  console.log("=== Pause/Resume Demo ===");
  console.log("Press ENTER to toggle pause/resume, Ctrl+C to exit\n");

  const task = new PausableTask((step) => {
    console.log(`  Executing step ${step}`);
  });

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  task.start();

  rl.on("line", () => {
    task.toggle();
  });

  // Graceful shutdown
  const cleanup = () => {
    task.stop();
    rl.close();
    process.exit(0);
  };

  process.on("SIGINT", cleanup);
  process.on("SIGTERM", cleanup);
};

main();
