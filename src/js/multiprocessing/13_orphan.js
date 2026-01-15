/*
 * Orphan Process Demonstration
 *
 * This script demonstrates orphan processes in Node.js. An orphan process is a
 * child process whose parent has terminated. In Unix-like systems, orphaned
 * processes are adopted by the init process (PID 1).
 *
 * Key concepts:
 * - Parent process termination
 * - Process reparenting to init/systemd
 * - Child process lifecycle independence
 * - Detached child processes
 *
 * NOTE: This is an educational demonstration. In production, always properly
 * manage child process lifecycles.
 */

"use strict";

const { fork, spawn } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv[2] === "child") {
  // Child process - will become orphan
  console.log(`[Child] Started (PID: ${process.pid}, Parent PID: ${process.ppid})`);
  
  const checkParent = async () => {
    await sleep(2000);
    console.log(`[Child] After delay - Parent PID: ${process.ppid}`);
    
    // On Unix, if parent terminated, PPID becomes 1 (init) or another system process
    if (process.ppid === 1) {
      console.log(`[Child] I am now an orphan! Adopted by init (PID 1)`);
    }
    
    await sleep(3000);
    console.log(`[Child] Finishing (PID: ${process.pid})`);
  };
  
  checkParent();

} else {
  // Parent process
  console.log("=== Orphan Process Demo ===\n");
  console.log(`[Parent] Started (PID: ${process.pid})`);
  
  // Spawn a detached child that will outlive the parent
  const child = spawn(process.execPath, [__filename, "child"], {
    detached: true,
    stdio: "inherit",
  });
  
  child.unref(); // Allow parent to exit independently
  
  console.log(`[Parent] Created child (PID: ${child.pid})`);
  console.log(`[Parent] Exiting in 1 second...`);
  console.log(`[Parent] Child will become orphan and continue running\n`);
  
  setTimeout(() => {
    console.log(`[Parent] Exiting now. Child ${child.pid} will be orphaned.`);
    console.log("[Parent] Watch the child output below:\n");
    process.exit(0);
  }, 1000);
}
