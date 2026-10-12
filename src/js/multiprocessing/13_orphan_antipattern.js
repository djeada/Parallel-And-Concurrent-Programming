/*
 * ⚠️  ANTIPATTERN: Orphan Process
 *
 * An orphan process is a child process whose parent has terminated before
 * the child. On Unix-like systems, orphans are "adopted" by init/systemd
 * (typically PID 1), but this varies — containers and subreaper processes
 * may use a different adopting PID.
 *
 * Key concepts:
 * - Parent process termination before child
 * - Process reparenting to init/systemd (subreaper)
 * - Child process lifecycle independence
 * - Detached child processes (detached: true + unref())
 *
 * NOTE: Orphan processes are usually unintentional and indicate missing
 * lifecycle management. In production, always track child PIDs and
 * terminate or await them during shutdown.
 *
 * NOTE: process.ppid may not become 1 on all systems. Docker containers,
 * systemd, and subreaper processes can adopt orphans with a different PID.
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
