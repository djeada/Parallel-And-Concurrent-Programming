/*
 * Zombie Processes — and How Node.js Prevents Them
 *
 * A zombie process is a child that has exited but whose exit status has not
 * yet been collected by the parent via waitpid(). It stays in the process
 * table as a "defunct" entry until the parent reaps it.
 *
 * In C you must call waitpid() manually; forgetting is a classic bug.
 * In Node.js, libuv registers a SIGCHLD handler and calls waitpid() for you
 * automatically, so persistent zombies are generally NOT possible from
 * pure Node.js code.
 *
 * Key concepts:
 * - What zombies are and why they matter
 * - How libuv prevents zombies automatically via the 'exit' event
 * - The 'exit' event is the Node.js equivalent of waitpid()
 * - When zombies CAN still appear (native addons, exec'd external programs)
 *
 * NOTE: This demo shows the CORRECT pattern (Node.js auto-reaping) as well
 * as where to look for zombie risk in hybrid C/Node environments.
 */

"use strict";

const { spawn } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

if (process.argv[2] === "child") {
  console.log(`[Child] Started (PID: ${process.pid})`);
  console.log(`[Child] Exiting in 1 second...`);
  setTimeout(() => {
    console.log(`[Child] Exiting now`);
    process.exit(0);
  }, 1000);
} else {
  console.log("=== Zombie Process Demo ===\n");
  console.log("Explanation:");
  console.log("  A zombie is a child that exited but was not wait()ed by its parent.");
  console.log("  In Node.js, libuv handles SIGCHLD and calls waitpid() automatically.");
  console.log("  The 'exit' event on a ChildProcess is the Node.js equivalent of wait().\n");

  console.log(`[Parent] PID: ${process.pid}`);

  const child = spawn(process.execPath, [__filename, "child"], {
    stdio: "inherit",
  });

  console.log(`[Parent] Spawned child PID: ${child.pid}`);
  console.log("[Parent] Waiting for child to exit (libuv handles waitpid internally)...\n");

  // Node.js fires 'exit' after libuv reaps the child — no zombie left behind.
  child.on("exit", async (code) => {
    console.log(`[Parent] 'exit' event fired — child ${child.pid} reaped (code: ${code})`);
    console.log("[Parent] No zombie: libuv called waitpid() before firing this event.\n");

    console.log("Where zombies CAN still appear in Node.js environments:");
    console.log("  - Native C/C++ addons that spawn processes without waitpid()");
    console.log("  - child_process.exec() shell commands that spawn grandchildren");
    console.log("    whose grandparent (the shell) exits before they do");
    console.log("  - Calling child.unref() keeps the child alive but delays reaping");
    console.log("    until the parent's event loop cycle notices the child exited\n");

    console.log("[Parent] Run 'ps aux | grep defunct' right now — no zombies.\n");

    await sleep(1000);
    console.log("=== Demo Complete ===");
  });
}

