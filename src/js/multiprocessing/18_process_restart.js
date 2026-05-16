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
      process.send({ type: "done" });
      process.exit(0);
    }, 1500);
  }

// ---------------------------------------------------------------------------
// Supervisor: spawn worker, restart with backoff on crash
// ---------------------------------------------------------------------------
} else {
  let shuttingDown = false;

  console.log("=== Supervisor / Auto-restart with Backoff Demo ===\n");
  console.log(`Max consecutive restarts : ${MAX_RESTARTS}`);
  console.log(`Backoff range            : ${BACKOFF_BASE_MS}ms – ${BACKOFF_CAP_MS}ms`);
  console.log(`Healthy run threshold    : ${HEALTHY_RUN_MS}ms\n`);

  // Backoff delay: min(cap, base * 2^attempt) — exponential with a ceiling.
  const backoffMs = (attempt) =>
    Math.min(BACKOFF_CAP_MS, BACKOFF_BASE_MS * Math.pow(2, attempt));

  const supervise = (workerId) => {
    let consecutiveCrashes = 0;
    let runNum = 0;
    let startedAt = 0;

    const spawn = () => {
      runNum++;
      startedAt = Date.now();
      console.log(`[Supervisor] Spawning worker ${workerId} (run #${runNum})...`);

      const worker = fork(__filename, [
        "worker",
        workerId.toString(),
        runNum.toString(),
      ]);

      worker.on("message", (msg) => {
        if (msg.type === "done") {
          console.log(`[Supervisor] Worker ${workerId} completed successfully.`);
          console.log("=== Demo Complete ===");
          shuttingDown = true;
          process.exit(0);
        }
      });

      worker.on("exit", async (code) => {
        if (shuttingDown) return;

        const elapsed = Date.now() - startedAt;

        if (code === 0) {
          consecutiveCrashes = 0; // clean exit — reset crash counter
          return;
        }

        // It was a crash (non-zero exit code).
        if (elapsed >= HEALTHY_RUN_MS) {
          // Ran long enough to be considered healthy; reset crash counter.
          console.log(
            `[Supervisor] Worker ${workerId} crashed after ${elapsed}ms — ` +
            `resetting crash counter (was healthy)`
          );
          consecutiveCrashes = 0;
        } else {
          consecutiveCrashes++;
        }

        if (consecutiveCrashes > MAX_RESTARTS) {
          console.error(
            `[Supervisor] Worker ${workerId} exceeded max restarts ` +
            `(${MAX_RESTARTS}). Giving up.`
          );
          process.exit(1);
        }

        const delay = backoffMs(consecutiveCrashes - 1);
        console.log(
          `[Supervisor] Worker ${workerId} crashed (code=${code}, crash #${consecutiveCrashes}). ` +
          `Restarting in ${delay}ms...`
        );
        await sleep(delay);
        spawn();
      });
    };

    spawn();
  };

  supervise(0);

  process.on("SIGTERM", () => {
    console.log("[Supervisor] SIGTERM — shutting down");
    shuttingDown = true;
    process.exit(0);
  });
}
