/*
 * Promise Combinators: race, any, all, allSettled
 *
 * JavaScript provides four static combinators on Promise. Each has a distinct
 * failure mode and use case. Knowing which to reach for is a core async skill.
 *
 *  Promise.all()        – wait for every promise; fail-fast on first rejection
 *  Promise.allSettled() – wait for every promise; never short-circuits
 *  Promise.race()       – settle with the first promise that settles (fulfilled OR rejected)
 *  Promise.any()        – settle with the first promise that FULFILLS; reject only when ALL reject
 *
 * Key concepts:
 * - Promise.race() does not cancel losing promises (they keep running)
 * - Promise.any() throws AggregateError when every input rejects
 * - Promise.all() rejects immediately on the first rejection, leaving others in flight
 * - Promise.allSettled() is the only combinator that waits for every outcome
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const task = (id, delayMs, shouldReject = false) =>
  new Promise((resolve, reject) => {
    setTimeout(() => {
      if (shouldReject) {
        reject(new Error(`Task ${id} failed after ${delayMs}ms`));
      } else {
        resolve({ id, delayMs });
      }
    }, delayMs);
  });

// ── Demo 1: Promise.race() ────────────────────────────────────────────────
// Resolves/rejects as soon as any promise settles.
// Use case: request timeout (race a fetch against a sleep-reject).
const demoRace = async () => {
  console.log("--- Promise.race() ---");

  const timeout = (ms) =>
    new Promise((_, reject) =>
      setTimeout(() => reject(new Error(`Timed out after ${ms}ms`)), ms)
    );

  try {
    // The 200 ms task wins; the 600 ms task keeps running but its result is ignored.
    const result = await Promise.race([
      task("A", 200),
      task("B", 600),
      timeout(400),
    ]);
    console.log(`  Winner: Task ${result.id} (${result.delayMs}ms)`);
    console.log("  NOTE: losing promises are NOT cancelled — they run to completion silently.");
  } catch (err) {
    console.log(`  Race lost to timeout: ${err.message}`);
  }

  // Race where the timeout wins first
  try {
    await Promise.race([task("C", 800), timeout(300)]);
  } catch (err) {
    console.log(`  Expected timeout: ${err.message}`);
  }
};

// ── Demo 2: Promise.any() ─────────────────────────────────────────────────
// Resolves with the FIRST fulfilled promise.
// Rejects with AggregateError only when EVERY promise rejects.
// Use case: redundant requests to multiple endpoints — take whichever responds first.
const demoAny = async () => {
  console.log("\n--- Promise.any() ---");

  // First fulfilled wins even though earlier promises rejected
  const result = await Promise.any([
    task("X", 100, true),  // rejects fast
    task("Y", 300),        // resolves slower — this wins
    task("Z", 500),
  ]);
  console.log(`  First success: Task ${result.id} (${result.delayMs}ms)`);

  // All reject → AggregateError
  try {
    await Promise.any([
      task("P", 100, true),
      task("Q", 200, true),
    ]);
  } catch (err) {
    // AggregateError.errors[] holds every individual rejection reason
    console.log(`  AggregateError (all rejected): ${err.errors.map((e) => e.message).join(", ")}`);
  }
};

// ── Demo 3: Promise.all() fail-fast ──────────────────────────────────────
// Rejects immediately when any promise rejects. Other promises continue running.
const demoAll = async () => {
  console.log("\n--- Promise.all() fail-fast ---");

  try {
    const results = await Promise.all([
      task(1, 100),
      task(2, 200, true), // this one rejects
      task(3, 300),       // still runs, but its result is discarded
    ]);
    console.log("  Results:", results);
  } catch (err) {
    console.log(`  Promise.all rejected early: ${err.message}`);
    console.log("  Task 3 may still be running in the background.");
  }

  // All succeed
  const [a, b] = await Promise.all([task("α", 100), task("β", 150)]);
  console.log(`  All succeeded: Task ${a.id}, Task ${b.id}`);
};

// ── Demo 4: Promise.allSettled() ─────────────────────────────────────────
// Waits for every promise regardless of outcome. Never rejects itself.
const demoAllSettled = async () => {
  console.log("\n--- Promise.allSettled() ---");

  const settled = await Promise.allSettled([
    task("M", 100),
    task("N", 200, true), // rejects
    task("O", 300),
  ]);

  settled.forEach((outcome, i) => {
    if (outcome.status === "fulfilled") {
      console.log(`  [${i}] fulfilled — Task ${outcome.value.id}`);
    } else {
      console.log(`  [${i}] rejected  — ${outcome.reason.message}`);
    }
  });

  const successes = settled.filter((o) => o.status === "fulfilled").length;
  console.log(`  ${successes}/${settled.length} succeeded`);
};

const main = async () => {
  console.log("=== Promise Combinators Demo ===\n");
  await demoRace();
  await demoAny();
  await demoAll();
  await demoAllSettled();
  console.log("\n=== Summary ===");
  console.log("race()        — first to settle (fulfilled or rejected)");
  console.log("any()         — first to fulfill; AggregateError if all fail");
  console.log("all()         — all must fulfill; fail-fast on first rejection");
  console.log("allSettled()  — always waits for every promise; inspects each outcome");
};

main().catch(console.error);
