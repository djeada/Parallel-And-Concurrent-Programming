/*
 * Process Signals and Graceful Shutdown
 *
 * Signals are asynchronous notifications sent to a process by the OS or
 * another process. Handling them correctly lets a program save state,
 * drain connections, and exit cleanly instead of being killed mid-operation.
 *
 * Key concepts:
 * - SIGTERM: polite shutdown request — the right way to stop a process
 * - SIGINT:  interactive interrupt (Ctrl+C) — also triggers shutdown
 * - SIGKILL: cannot be caught or handled — immediate forced termination
 * - Forwarding signals from parent to forked children
 * - process.once('exit', cleanup) for final resource cleanup
 * - process.on('uncaughtException', ...) for emergency cleanup on crash
 */

"use strict";

const { fork } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// ---------------------------------------------------------------------------
// Worker: simulates ongoing work, handles SIGTERM for graceful stop
// ---------------------------------------------------------------------------
if (process.argv[2] === "worker") {
  const workerId = process.argv[3];
  let running = true;
  let iteration = 0;

  console.log(`  [Worker ${workerId}] Started (PID: ${process.pid})`);

  const doWork = async () => {
    while (running) {
      iteration++;
      process.send({ type: "heartbeat", workerId, iteration });
      await sleep(400);
    }
    console.log(`  [Worker ${workerId}] Work loop stopped after ${iteration} iterations`);
    process.send({ type: "done", workerId });
    process.exit(0);
  };

  // SIGTERM: set flag and let the work loop finish gracefully.
  process.on("SIGTERM", () => {
    console.log(`  [Worker ${workerId}] SIGTERM received — finishing current work`);
    running = false;
  });

  // SIGINT is often inherited from the terminal; handle it the same way.
  process.on("SIGINT", () => {
    console.log(`  [Worker ${workerId}] SIGINT received`);
    running = false;
  });

  process.once("exit", (code) => {
    console.log(`  [Worker ${workerId}] Exiting with code ${code}`);
  });

  doWork();

// ---------------------------------------------------------------------------
// Parent: fork workers, forward SIGTERM/SIGINT, wait for graceful exit
// ---------------------------------------------------------------------------
} else {
  const NUM_WORKERS = 3;
  let completedCount = 0;
  const workers = [];

  console.log("=== Process Signals & Graceful Shutdown Demo ===\n");
  console.log(`Forking ${NUM_WORKERS} workers...`);

  for (let i = 0; i < NUM_WORKERS; i++) {
    const worker = fork(__filename, ["worker", i.toString()]);
    workers.push(worker);

    worker.on("message", (msg) => {
      if (msg.type === "heartbeat") {
        // Suppress after first few to reduce noise
        if (msg.iteration <= 2) {
          console.log(`  [Parent] Heartbeat from worker ${msg.workerId} #${msg.iteration}`);
        }
      } else if (msg.type === "done") {
        completedCount++;
        console.log(`  [Parent] Worker ${msg.workerId} shut down gracefully (${completedCount}/${NUM_WORKERS})`);
        if (completedCount === NUM_WORKERS) {
          console.log("\n[Parent] All workers exited gracefully.\n=== Demo Complete ===");
          process.exit(0);
        }
      }
    });

    worker.on("error", (err) =>
      console.error(`[Parent] Worker ${i} error: ${err.message}`)
    );
  }

  // Forward SIGTERM to all children so the whole process tree shuts down.
  const shutdown = (signal) => {
    console.log(`\n[Parent] Received ${signal} — forwarding to ${workers.length} workers...`);
    workers.forEach((w) => {
      if (!w.exitCode) w.kill(signal);
    });
  };

  process.on("SIGTERM", () => shutdown("SIGTERM"));
  process.on("SIGINT",  () => shutdown("SIGTERM")); // normalize: SIGINT → SIGTERM

  // Demo: send ourselves SIGTERM after 2 seconds to trigger graceful shutdown.
  setTimeout(() => {
    console.log("\n[Parent] Sending SIGTERM to self (simulating `kill <pid>` or systemd stop)...");
    process.kill(process.pid, "SIGTERM");
  }, 2000);
}
