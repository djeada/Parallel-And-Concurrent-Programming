/*
 * ⚠️  ANTIPATTERN: Common Async/Await Mistakes
 *
 * These patterns silently discard errors, execute in the wrong order, or
 * introduce race conditions. Each demo shows the broken form, explains why
 * it fails, and then shows the correct alternative.
 *
 * Antipatterns covered:
 *  1. Unhandled Promise rejection   — rejected Promise with no .catch() or try/catch
 *  2. async function in forEach     — forEach ignores returned Promises; errors vanish
 *  3. Forgetting await              — async call returns a pending Promise, not a value
 *  4. Sequential await in a loop   — accidentally serialises work that could be parallel
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const flakyOp = async (id, failProb = 0) => {
  await sleep(50);
  if (Math.random() < failProb) throw new Error(`Op ${id} failed`);
  return `result-${id}`;
};

// ── Antipattern 1: Unhandled Promise rejection ────────────────────────────
// A Promise that rejects with no handler attached silently disappears (or, in
// Node, emits "unhandledRejection"). Many real bugs are silently swallowed
// this way.
const demoUnhandledRejection = async () => {
  console.log("--- ⚠️  Antipattern 1: Unhandled rejection ---");

  // ❌ BAD: rejection with no .catch() and no await in a try/catch
  const badFn = () => {
    Promise.reject(new Error("I have no handler")); // fire-and-forget rejection
  };

  // Temporarily install a handler so the demo doesn't crash the process
  const onUnhandled = (reason) => {
    console.log(`  [unhandledRejection caught for demo]: ${reason.message}`);
  };
  process.once("unhandledRejection", onUnhandled);

  badFn();
  await sleep(10); // let the event loop process the rejection

  process.removeListener("unhandledRejection", onUnhandled);

  // ✅ GOOD: always attach .catch() or use try/await
  try {
    await Promise.reject(new Error("This one is handled"));
  } catch (err) {
    console.log(`  Handled correctly: ${err.message}`);
  }
};

// ── Antipattern 2: async function in forEach ──────────────────────────────
// Array.prototype.forEach is synchronous and ignores the Promise each async
// callback returns. As a result:
//   - errors inside the callback are silently dropped
//   - the outer await does NOT wait for the iterations to complete
const demoAsyncForEach = async () => {
  console.log("\n--- ⚠️  Antipattern 2: async function in forEach ---");

  const items = [1, 2, 3];
  const results = [];

  // ❌ BAD: forEach discards the returned Promises
  console.log("  BAD: results after forEach (async):");
  items.forEach(async (item) => {
    const r = await flakyOp(item); // this Promise is never awaited
    results.push(r);
  });
  // forEach has returned; results is still empty because the Promises haven't resolved
  console.log(`  results.length = ${results.length} (expected 3, got 0)`);
  await sleep(200); // wait manually to show they eventually resolve
  console.log(`  After 200 ms delay: results.length = ${results.length}`);

  // ✅ GOOD option A: for...of with sequential await
  const sequential = [];
  console.log("  GOOD (sequential): for...of with await");
  for (const item of items) {
    sequential.push(await flakyOp(item));
  }
  console.log(`  sequential.length = ${sequential.length} ✓`);

  // ✅ GOOD option B: Promise.all + map for parallel execution
  console.log("  GOOD (parallel): Promise.all + map");
  const parallel = await Promise.all(items.map((item) => flakyOp(item)));
  console.log(`  parallel.length = ${parallel.length} ✓`);
};

// ── Antipattern 3: Forgetting await ──────────────────────────────────────
// Calling an async function without await returns a pending Promise object.
// Code that follows runs immediately with the Promise — not the resolved value.
const demoForgotAwait = async () => {
  console.log("\n--- ⚠️  Antipattern 3: Forgetting await ---");

  // ❌ BAD: result is a Promise, not the value
  const bad = flakyOp("no-await"); // no await
  console.log(`  BAD: typeof result = ${typeof bad} (${bad})`);
  // Any error thrown inside flakyOp becomes an unhandled rejection
  await bad; // consume the Promise to avoid unhandledRejection

  // ❌ BAD: try/catch around sync code doesn't catch async errors
  let caughtAnything = false;
  try {
    flakyOp("will-throw-but-not-caught"); // no await — rejection escapes catch block
  } catch (_) {
    caughtAnything = true;
  }
  // Install temporary handler to catch the escapee
  const onUnhandled2 = () => {};
  process.once("unhandledRejection", onUnhandled2);
  await sleep(10);
  process.removeListener("unhandledRejection", onUnhandled2);
  console.log(`  BAD: try/catch caught synchronous throw? ${caughtAnything} (always false)`);

  // ✅ GOOD: await inside try/catch
  try {
    const value = await flakyOp("properly-awaited");
    console.log(`  GOOD: got value = ${value}`);
  } catch (err) {
    console.log(`  GOOD: error properly caught: ${err.message}`);
  }
};

// ── Antipattern 4: Sequential await in a loop ─────────────────────────────
// Each `await` waits for the current iteration before starting the next.
// Independent tasks should run in parallel with Promise.all().
const demoSequentialLoop = async () => {
  console.log("\n--- ⚠️  Antipattern 4: Sequential await in a loop (unnecessary) ---");

  const ids = [1, 2, 3, 4];

  // ❌ BAD: each request starts only after the previous one resolves
  const badStart = Date.now();
  const badResults = [];
  for (const id of ids) {
    badResults.push(await sleep(100).then(() => id * 10)); // 100 ms each, serialised
  }
  console.log(`  BAD:  ${Date.now() - badStart}ms for ${ids.length} tasks (expect ~400ms)`);

  // ✅ GOOD: all tasks start simultaneously
  const goodStart = Date.now();
  const goodResults = await Promise.all(ids.map((id) => sleep(100).then(() => id * 10)));
  console.log(`  GOOD: ${Date.now() - goodStart}ms for ${ids.length} tasks (expect ~100ms)`);
  console.log(`  Results match: ${JSON.stringify(badResults) === JSON.stringify(goodResults)}`);
};

const main = async () => {
  console.log("=== ⚠️  Async Antipatterns Demo ===\n");
  await demoUnhandledRejection();
  await demoAsyncForEach();
  await demoForgotAwait();
  await demoSequentialLoop();
  console.log("\n=== Summary ===");
  console.log("1. Always attach .catch() or use try/await — never let rejections escape");
  console.log("2. forEach ignores returned Promises — use for...of or Promise.all+map");
  console.log("3. Forgetting await returns a Promise, not a value; try/catch won't help");
  console.log("4. Sequential await in a loop serialises independent work — prefer Promise.all");
};

main().catch(console.error);
