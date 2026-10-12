/*
 * Supervisor Pattern — Automatic Process Restart with Backoff
 *
 * A supervisor monitors worker processes and restarts them when they crash.
 * Naive restart (immediately, unconditionally) is dangerous: a worker that
 * crashes at startup restarts in a tight loop, consuming resources.
 *
 * The correct approach combines:
 *   1. Exponential backoff — wait longer after each consecutive crash
 *   2. Reset-on-success — consecutive-crash counter resets after a healthy run
 *   3. Max restart limit — give up after N consecutive crashes
 *
 * Key concepts:
 * - Crash detection via the 'exit' event
 * - Exponential backoff with a jitter cap
 * - Reset-on-success (worker ran long enough → crashes are independent)
 * - Maximum restart limit to prevent infinite restart loops
 * - shuttingDown guard to avoid restarting during intentional shutdown
 */

"use strict";

const { fork } = require("child_process");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const MAX_RESTARTS = 5;        // give up after this many consecutive crashes
const BACKOFF_BASE_MS = 200;   // initial delay before first restart
const BACKOFF_CAP_MS = 5000;   // maximum delay between restarts
const HEALTHY_RUN_MS = 2000;   // a run this long counts as "healthy" (resets counter)

// ---------------------------------------------------------------------------
// Worker: crashes on the first few runs, then stabilises
// ---------------------------------------------------------------------------
if (process.argv[2] === "worker") {
  const workerId = process.argv[3];
  const runNum = parseInt(process.argv[4], 10);

  console.log(`  [Worker ${workerId}] Run #${runNum} started (PID: ${process.pid})`);

  // Crash on runs 1–3 (simulates a bug that gets fixed after a few restarts).
  if (runNum <= 3) {
    const crashAfterMs = 300 + Math.random() * 200;
    setTimeout(() => {
      console.log(`  [Worker ${workerId}] Crashing on run #${runNum}! (simulated bug)`);
      process.exit(1);
    }, crashAfterMs);
  } else {
    // Stable: run for a while then exit cleanly.
    console.log(`  [Worker ${workerId}] Running stably on run #${runNum}...`);
    setTimeout(() => {
      console.log(`  [Worker ${workerId}] Completing cleanly on run #${runNum}`);
      process.send({ type: "done" }, () => process.disconnect());
    }, 1500);
  }

// ---------------------------------------------------------------------------
// Supervisor: spawn worker, restart with backoff on crash
// ---------------------------------------------------------------------------
} else {
  let shuttingDown = false;
  let currentWorker = null;
  let restartTimer = null;
  let consecutiveCrashes = 0;
  let runNum = 0;

  console.log("=== Supervisor / Auto-restart with Backoff Demo ===\n");
  console.log(`Max consecutive restarts: ${MAX_RESTARTS}`);
  console.log(`Healthy run threshold: ${HEALTHY_RUN_MS}ms\n`);

  const spawnWorker = () => {
    if (shuttingDown) return;
    runNum++;
    const startedAt = Date.now();
    console.log(`[Supervisor] Spawning worker 0 (run #${runNum})...`);
    const worker = fork(__filename, ["worker", "0", runNum.toString()]);
    currentWorker = worker;

    worker.on("message", (msg) => {
      if (msg.type === "done") {
        console.log("[Supervisor] Worker 0 reported completion; waiting for exit.");
      }
    });
    worker.on("error", (error) => {
      console.error("Worker error:", error.message);
      shuttingDown = true;
      process.exitCode = 1;
      worker.kill("SIGTERM");
    });
    worker.on("exit", (code) => {
      currentWorker = null;
      if (shuttingDown) return;
      if (code === 0) {
        shuttingDown = true;
        console.log("[Supervisor] Worker completed successfully.\n=== Demo Complete ===");
        return;
      }
      const elapsed = Date.now() - startedAt;
      consecutiveCrashes = elapsed >= HEALTHY_RUN_MS ? 0 : consecutiveCrashes + 1;
      if (consecutiveCrashes > MAX_RESTARTS) {
        console.error("Worker exceeded max restarts. Giving up.");
        shuttingDown = true;
        process.exitCode = 1;
        return;
      }
      const delay = Math.min(BACKOFF_CAP_MS,
        BACKOFF_BASE_MS * 2 ** Math.max(0, consecutiveCrashes - 1));
      console.log(`[Supervisor] Worker crashed (code=${code}); restarting in ${delay}ms...`);
      restartTimer = setTimeout(spawnWorker, delay);
    });
  };

  const shutdown = () => {
    if (shuttingDown) return;
    console.log("[Supervisor] Shutting down worker and cancelling restarts");
    shuttingDown = true;
    clearTimeout(restartTimer);
    // Let Node exit naturally after the child has actually exited.
    currentWorker?.kill("SIGTERM");
  };
  process.on("SIGTERM", shutdown);
  process.on("SIGINT", shutdown);
  spawnWorker();
}
