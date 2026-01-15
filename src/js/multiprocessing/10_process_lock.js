/*
 * Process Lock (Mutex) Pattern
 *
 * This script demonstrates using a file-based lock to protect access to a
 * shared resource (counter file) across multiple processes. This prevents
 * race conditions when multiple processes try to update the same data.
 *
 * Key concepts:
 * - File-based mutex implementation
 * - Critical section protection
 * - Atomic read-modify-write operations
 * - Lock cleanup
 */

"use strict";

const fs = require("fs");
const path = require("path");
const { fork } = require("child_process");

const COUNTER_FILE = process.env.COUNTER_FILE || path.join("/tmp", `counter_${process.pid}.txt`);
const LOCK_FILE = process.env.LOCK_FILE || path.join("/tmp", `counter_${process.pid}.lock`);
const NUM_PROCESSES = 4;

const sleep = (ms) => {
  const end = Date.now() + ms;
  while (Date.now() < end) {
    // Busy wait
  }
};

const acquireLock = () => {
  const maxAttempts = 100;
  let attempts = 0;
  
  while (attempts < maxAttempts) {
    try {
      // Try to create lock file exclusively
      fs.writeFileSync(LOCK_FILE, process.pid.toString(), { flag: "wx" });
      return true;
    } catch (error) {
      if (error.code === "EEXIST") {
        // Lock is held, wait and retry
        sleep(50);
        attempts++;
      } else {
        throw error;
      }
    }
  }
  
  throw new Error("Failed to acquire lock after maximum attempts");
};

const releaseLock = () => {
  try {
    fs.unlinkSync(LOCK_FILE);
  } catch (error) {
    if (error.code !== "ENOENT") {
      throw error;
    }
  }
};

const workerTask = () => {
  console.log(`  [Process ${process.pid}] Starting, waiting for lock...`);
  
  try {
    acquireLock();
    console.log(`  [Process ${process.pid}] Lock acquired`);
    
    // Critical section - read, modify, write
    const currentValue = fs.existsSync(COUNTER_FILE)
      ? parseInt(fs.readFileSync(COUNTER_FILE, "utf-8"), 10)
      : 0;
    
    // Simulate some processing time
    sleep(100);
    
    const newValue = currentValue + 1;
    fs.writeFileSync(COUNTER_FILE, newValue.toString());
    
    console.log(`  [Process ${process.pid}] Counter: ${currentValue} -> ${newValue}`);
  } finally {
    releaseLock();
    console.log(`  [Process ${process.pid}] Lock released`);
  }
};

if (process.argv[2] === "worker") {
  workerTask();
} else {
  console.log("=== Process Lock (Mutex) Demo ===\n");
  console.log(`Number of processes: ${NUM_PROCESSES}`);
  console.log("Each process will increment a shared counter with lock protection\n");

  // Initialize counter file
  fs.writeFileSync(COUNTER_FILE, "0");
  
  // Clean up any stale lock file
  if (fs.existsSync(LOCK_FILE)) {
    fs.unlinkSync(LOCK_FILE);
  }

  const children = [];
  const startTime = Date.now();

  // Fork worker processes
  for (let i = 0; i < NUM_PROCESSES; i++) {
    const child = fork(__filename, ["worker"], {
      env: { ...process.env, COUNTER_FILE, LOCK_FILE },
    });
    children.push(child);
  }

  // Wait for all children to complete
  Promise.all(
    children.map((child) => new Promise((resolve) => child.on("exit", resolve)))
  ).then(() => {
    const elapsed = Date.now() - startTime;
    const finalCounter = parseInt(fs.readFileSync(COUNTER_FILE, "utf-8"), 10);
    
    console.log("\n=== Results ===");
    console.log(`Final counter value: ${finalCounter}`);
    console.log(`Expected value: ${NUM_PROCESSES}`);
    console.log(`Correct: ${finalCounter === NUM_PROCESSES}`);
    console.log(`Total time: ${elapsed}ms`);

    // Cleanup
    try {
      fs.unlinkSync(COUNTER_FILE);
      if (fs.existsSync(LOCK_FILE)) {
        fs.unlinkSync(LOCK_FILE);
      }
    } catch (err) {
      // Ignore cleanup errors
    }
  });
}
