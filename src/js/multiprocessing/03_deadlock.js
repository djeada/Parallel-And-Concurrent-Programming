/*
 * Deadlock Demonstration
 *
 * This script demonstrates a deadlock situation in multiprocessing.
 * A deadlock occurs when two or more processes are waiting for each other
 * to release resources, creating a circular dependency that never resolves.
 *
 * Key concepts:
 * - Circular wait condition
 * - Resource acquisition order
 * - Deadlock prevention strategies
 * - Message-based lock simulation
 *
 * WARNING: This script intentionally creates a deadlock for educational purposes.
 * It will hang indefinitely - use Ctrl+C to terminate.
 */

"use strict";

const { fork } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv.length === 2) {
  // Main process - acts as a lock manager
  console.log("=== Deadlock Demonstration ===\n");
  console.log("This demo shows how deadlocks occur when processes");
  console.log("try to acquire locks in different orders.\n");
  console.log("Press Ctrl+C to exit (the program will hang due to deadlock)\n");

  const workerA = fork(__filename, ["workerA"]);
  const workerB = fork(__filename, ["workerB"]);

  const locks = {
    A: { held: false, holder: null },
    B: { held: false, holder: null },
  };

  const requestQueue = {
    A: [],
    B: [],
  };

  const handleLockRequest = (worker, workerName, lockName) => {
    if (!locks[lockName].held) {
      locks[lockName].held = true;
      locks[lockName].holder = workerName;
      worker.send(`grantLock${lockName}`);
      console.log(`  [Manager] Granted lock ${lockName} to ${workerName}`);
    } else {
      requestQueue[lockName].push({ worker, workerName });
      console.log(`  [Manager] ${workerName} waiting for lock ${lockName} (held by ${locks[lockName].holder})`);
    }
  };

  workerA.on("message", (message) => {
    if (message === "requestLockA") {
      handleLockRequest(workerA, "workerA", "A");
    } else if (message === "requestLockB") {
      handleLockRequest(workerA, "workerA", "B");
    }
  });

  workerB.on("message", (message) => {
    if (message === "requestLockA") {
      handleLockRequest(workerB, "workerB", "A");
    } else if (message === "requestLockB") {
      handleLockRequest(workerB, "workerB", "B");
    }
  });

  // Detect deadlock after timeout
  setTimeout(() => {
    console.log("\n=== DEADLOCK DETECTED ===");
    console.log("Both workers are waiting for locks held by each other:");
    console.log(`  - workerA holds lock A, waiting for lock B`);
    console.log(`  - workerB holds lock B, waiting for lock A`);
    console.log("\nThis is a circular wait - neither can proceed.");
    console.log("Terminating processes...\n");
    workerA.kill();
    workerB.kill();
    process.exit(0);
  }, 5000);

} else {
  // Worker process
  const workerName = process.argv[2];
  const firstLock = workerName === "workerA" ? "A" : "B";
  const secondLock = workerName === "workerA" ? "B" : "A";

  console.log(`${workerName}: Starting, will acquire locks in order: ${firstLock} -> ${secondLock}`);

  process.send(`requestLock${firstLock}`);

  process.on("message", async (message) => {
    if (message === `grantLock${firstLock}`) {
      console.log(`${workerName}: Acquired lock ${firstLock}`);
      
      // Simulate some work
      await sleep(1000);
      
      console.log(`${workerName}: Now trying to acquire lock ${secondLock}...`);
      process.send(`requestLock${secondLock}`);
      
    } else if (message === `grantLock${secondLock}`) {
      console.log(`${workerName}: Acquired lock ${secondLock}`);
      console.log(`${workerName}: Has both locks, completing work`);
      // This point is never reached due to deadlock
    }
  });
}
