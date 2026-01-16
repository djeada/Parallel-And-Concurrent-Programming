/*
 * Zombie Process Demonstration
 *
 * This script demonstrates zombie processes in Node.js. A zombie process is a
 * child process that has terminated but whose parent has not yet collected
 * its exit status (via wait/waitpid). The process remains in the process table.
 *
 * Key concepts:
 * - Process termination without cleanup
 * - Parent not calling wait() on child
 * - Process table entry retention
 * - Resource leak from zombies
 *
 * WARNING: This creates a zombie process intentionally for educational purposes.
 * The zombie is cleaned up when this script exits.
 */

"use strict";

const { spawn } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv[2] === "child") {
  // Child process - will become zombie after termination
  console.log(`[Child] Started (PID: ${process.pid})`);
  console.log(`[Child] Will exit in 1 second...`);
  
  setTimeout(() => {
    console.log(`[Child] Exiting now`);
    process.exit(0);
  }, 1000);

} else {
  // Parent process - intentionally doesn't wait for child
  console.log("=== Zombie Process Demo ===\n");
  console.log(`[Parent] Started (PID: ${process.pid})`);
  
  const child = spawn(process.execPath, [__filename, "child"], {
    detached: true,
    stdio: "inherit",
  });
  
  console.log(`[Parent] Created child (PID: ${child.pid})`);
  console.log(`[Parent] NOT waiting for child to exit (no wait() call)`);
  console.log(`[Parent] Child will become zombie after termination\n`);
  
  // Don't wait for child - this creates a zombie when child exits
  // child.unref() - we don't call this to keep parent running
  
  const checkZombie = async () => {
    await sleep(3000);
    
    console.log("\n[Parent] After child termination:");
    console.log(`[Parent] Child ${child.pid} is now a zombie (defunct)`);
    console.log("[Parent] It exists in process table but consumes no resources");
    console.log("[Parent] Run 'ps aux | grep defunct' to see zombies\n");
    
    await sleep(2000);
    
    console.log("[Parent] Now cleaning up by exiting...");
    console.log("[Parent] When parent exits, init will reap the zombie");
    console.log("\n=== Demo Complete ===");
    
    process.exit(0);
  };
  
  checkZombie();
}
