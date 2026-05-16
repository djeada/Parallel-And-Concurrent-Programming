/*
 * The Node.js Event Loop and Task Queues
 *
 * Node.js runs JavaScript on a single thread. "Asynchronous" code does not run
 * in parallel — it runs in between turns of the event loop. Understanding the
 * order in which callbacks are scheduled is essential for avoiding subtle bugs.
 *
 * Queue priority (highest → lowest):
 *  1. Synchronous code (current stack)
 *  2. process.nextTick()  — drains completely before the next phase
 *                           ⚠️  can starve the event loop if called recursively
 *  3. Promise microtasks  — .then() / queueMicrotask() — same microtask queue
 *  4. Macrotasks: timers (setTimeout/setInterval), I/O callbacks, setImmediate
 *
 * setImmediate() vs setTimeout(fn, 0) at the TOP LEVEL:
 *   - The order is non-deterministic (depends on OS timer resolution).
 *
 * setImmediate() vs setTimeout(fn, 0) INSIDE an I/O callback:
 *   - setImmediate always wins — it fires in the current "check" phase,
 *     while the timer waits for the next timer phase.
 *
 * Key concepts:
 * - microtask queue empties completely between event loop phases
 * - process.nextTick() callbacks run before Promise microtasks
 * - setImmediate() fires in the "check" phase, after I/O callbacks
 * - Recursive process.nextTick() starves I/O, timers, and Promises
 */

"use strict";

const fs = require("fs");
const os = require("os");
const path = require("path");

// ── Demo 1: Microtask vs macrotask ordering ───────────────────────────────
const demoMicrotaskOrder = () => {
  console.log("--- Demo 1: Execution Order (microtask vs macrotask) ---");

  const log = (label) => process.stdout.write(`  ${label}\n`);

  // Schedule all callbacks before any of them run
  setTimeout(() => log("6  setTimeout(0)"), 0);
  setImmediate(() => log("?  setImmediate (order vs setTimeout is non-deterministic at top level)"));
  Promise.resolve().then(() => log("4  Promise.then (microtask)"));
  queueMicrotask(() => log("5  queueMicrotask (microtask)"));
  process.nextTick(() => log("3  process.nextTick"));
  process.nextTick(() => log("3b process.nextTick (second, same batch)"));

  log("1  synchronous (start)");
  log("2  synchronous (end)");

  // The output order will be:
  //   1 → 2 → 3 → 3b → 4 → 5 → setTimeout/setImmediate (in some order)
};

// ── Demo 2: Inside an I/O callback — setImmediate wins ───────────────────
const demoInsideIO = () => {
  console.log("\n--- Demo 2: Inside I/O callback — setImmediate before setTimeout ---");

  const tmpFile = path.join(os.tmpdir(), "event_loop_demo.txt");
  fs.writeFileSync(tmpFile, "x");

  fs.readFile(tmpFile, () => {
    // Inside an I/O callback: setImmediate fires in the current check phase;
    // setTimeout fires in the NEXT timer phase — setImmediate always wins here.
    setTimeout(() => {
      console.log("  setTimeout(0) — SECOND inside I/O callback");
      fs.unlinkSync(tmpFile);
    }, 0);

    setImmediate(() => {
      console.log("  setImmediate  — FIRST inside I/O callback ✓");
    });
  });
};

// ── Demo 3: process.nextTick starvation ──────────────────────────────────
// Recursive process.nextTick() never lets the event loop advance.
// This is a real footgun when processing large datasets with nextTick recursion.
//
// We use setImmediate as a proxy for "something the event loop wants to do next".
// With unbounded nextTick recursion, setImmediate never gets a chance to run.
const demoNextTickStarvation = (callback) => {
  console.log("\n--- Demo 3: nextTick starvation ---");
  console.log("  5 recursive nextTick calls delay setImmediate from running:");

  let tickCount = 0;
  const MAX = 5;

  setImmediate(() => {
    console.log(`  setImmediate finally ran after ${tickCount} nextTick callbacks ✓`);
    callback();
  });

  const recurse = () => {
    tickCount++;
    console.log(`  nextTick #${tickCount}`);
    if (tickCount < MAX) {
      // ⚠️  each nextTick delays all I/O and macrotasks further
      process.nextTick(recurse);
    }
    // When recurse stops, setImmediate can finally execute
  };

  process.nextTick(recurse);
};

// ── Demo 4: queueMicrotask vs Promise.then ────────────────────────────────
// Both enter the same microtask queue. The difference: queueMicrotask()
// is a direct enqueue (no Promise wrapping overhead) and errors thrown
// inside it cannot be caught with .catch() — they throw as uncaught exceptions.
const demoQueueMicrotask = async () => {
  console.log("\n--- Demo 4: queueMicrotask vs Promise.then (same queue) ---");

  const order = [];

  await new Promise((resolve) => {
    queueMicrotask(() => order.push("queueMicrotask A"));
    Promise.resolve().then(() => order.push("Promise.then B"));
    queueMicrotask(() => order.push("queueMicrotask C"));
    Promise.resolve().then(() => order.push("Promise.then D"));
    // Resolve from nextTick so that all microtasks above run before resolve
    process.nextTick(resolve);
  });

  console.log(`  Microtask order: ${order.join(" → ")}`);
  console.log("  (FIFO within same microtask queue — A, B, C, D in scheduling order)");
};

const main = async () => {
  console.log("=== Event Loop and Task Queue Demo ===\n");
  demoMicrotaskOrder();

  // Allow demo 1 to settle before printing demo 2
  await new Promise((resolve) => setTimeout(resolve, 20));

  demoInsideIO();
  await new Promise((resolve) => setTimeout(resolve, 50));

  await new Promise((resolve) => demoNextTickStarvation(resolve));

  await demoQueueMicrotask();

  console.log("\n=== Summary ===");
  console.log("nextTick      — runs before Promises; recursive use starves the event loop");
  console.log("queueMicrotask/Promise.then — same microtask queue, FIFO order");
  console.log("setImmediate  — check phase; deterministically before setTimeout inside I/O");
  console.log("setTimeout(0) — next timer phase; non-deterministic vs setImmediate at top level");
};

main().catch(console.error);
