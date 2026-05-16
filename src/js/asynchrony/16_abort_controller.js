/*
 * AbortController and AbortSignal — Cancellation Patterns
 *
 * AbortController lets you cancel async operations that explicitly observe an
 * AbortSignal. IMPORTANT: calling abort() does NOT magically stop any in-flight
 * Promise. The async operation MUST register an abort listener and react to it.
 *
 * Patterns covered:
 *  1. abortableSleep — manually wiring an abort listener into a timer
 *  2. AbortSignal.timeout()  — creates a signal that auto-aborts after N ms  (Node 17.3+)
 *  3. AbortSignal.any()      — first of several signals to fire wins           (Node 20+)
 *  4. fetch() with AbortSignal — built-in support in native fetch (Node 18+)
 *
 * Key concepts:
 * - signal.throwIfAborted() only checks the CURRENT state; it does not
 *   subscribe to future aborts. Use signal.addEventListener('abort', ...) for that.
 * - AbortError.name === 'AbortError' (check .name, not instanceof, for compatibility)
 * - Cancelling a fetch via abort() does NOT prevent the request reaching the server.
 */

"use strict";

// ── Helper: sleep that respects an AbortSignal ────────────────────────────
// Wires both a timer and an abort listener, whichever fires first wins.
const abortableSleep = (ms, signal) =>
  new Promise((resolve, reject) => {
    signal?.throwIfAborted(); // reject immediately if already aborted

    const timer = setTimeout(resolve, ms);

    const onAbort = () => {
      clearTimeout(timer);
      reject(signal.reason); // reason is the value passed to controller.abort(reason)
    };

    signal?.addEventListener("abort", onAbort, { once: true });
  });

// ── Demo 1: Manual AbortController ───────────────────────────────────────
const demoManualAbort = async () => {
  console.log("--- Demo 1: Manual AbortController ---");

  const controller = new AbortController();
  const { signal } = controller;

  // Cancel after 200 ms
  setTimeout(() => {
    console.log("  Aborting...");
    controller.abort(new Error("User cancelled"));
  }, 200);

  try {
    console.log("  Starting 2 s sleep...");
    await abortableSleep(2000, signal);
    console.log("  Sleep completed (should not reach here)");
  } catch (err) {
    if (err.name === "AbortError" || err instanceof Error) {
      console.log(`  Caught cancellation: ${err.message}`);
    }
  }

  // Demonstrate already-aborted signal throws immediately
  try {
    await abortableSleep(1000, signal);
  } catch (err) {
    console.log(`  Already-aborted signal throws immediately: ${err.message}`);
  }
};

// ── Demo 2: AbortSignal.timeout() ────────────────────────────────────────
// Convenience API — no need to create a controller and schedule setTimeout manually.
const demoTimeoutSignal = async () => {
  console.log("\n--- Demo 2: AbortSignal.timeout() (Node 17.3+) ---");

  const signal = AbortSignal.timeout(300); // auto-aborts after 300 ms

  try {
    console.log("  Starting 1 s sleep with 300 ms timeout signal...");
    await abortableSleep(1000, signal);
    console.log("  Completed (should not reach here)");
  } catch (err) {
    console.log(`  Timed out: ${err.name} — ${err.message}`);
  }

  // Short work that completes before the timeout
  const fast = AbortSignal.timeout(500);
  try {
    await abortableSleep(100, fast);
    console.log("  Fast sleep completed before timeout ✓");
  } catch (err) {
    console.log(`  Unexpected abort: ${err.message}`);
  }
};

// ── Demo 3: AbortSignal.any() ─────────────────────────────────────────────
// Combines multiple signals — the combined signal fires when ANY source fires.
// Use case: "cancel if user clicks cancel OR if global timeout fires."
const demoAnySignal = async () => {
  console.log("\n--- Demo 3: AbortSignal.any() (Node 20+) ---");

  if (typeof AbortSignal.any !== "function") {
    console.log("  AbortSignal.any() not available in this Node version — skipping");
    return;
  }

  const userCancel = new AbortController();
  const combined = AbortSignal.any([
    userCancel.signal,
    AbortSignal.timeout(1000), // hard ceiling
  ]);

  // Simulate user cancelling at 200 ms
  setTimeout(() => userCancel.abort(new Error("User clicked cancel")), 200);

  try {
    await abortableSleep(5000, combined);
    console.log("  Completed (should not reach here)");
  } catch (err) {
    console.log(`  Combined signal fired: ${err.message}`);
  }
};

// ── Demo 4: fetch() with AbortSignal ─────────────────────────────────────
// fetch() natively accepts a signal option. Built-in support since Node 18.
const demoFetchAbort = async () => {
  console.log("\n--- Demo 4: fetch() with AbortSignal ---");

  if (typeof fetch !== "function") {
    console.log("  Native fetch not available in this Node version — skipping");
    return;
  }

  // Use AbortSignal.timeout() for a simple per-request timeout
  const signal = AbortSignal.timeout(50); // 50 ms — very short, likely to abort

  try {
    const response = await fetch("https://jsonplaceholder.typicode.com/posts/1", {
      signal,
    });
    const data = await response.json();
    console.log(`  Fetch succeeded: "${data.title?.substring(0, 40)}"`);
  } catch (err) {
    if (err.name === "TimeoutError" || err.name === "AbortError") {
      console.log(`  Fetch aborted as expected: ${err.name}`);
    } else {
      // Network unavailable — not a bug in the example
      console.log(`  Network error (expected in offline env): ${err.message}`);
    }
  }

  // Show that abort after fetch succeeds does nothing harmful
  const controller2 = new AbortController();
  controller2.abort(); // abort before even starting
  try {
    await fetch("https://example.com", { signal: controller2.signal });
  } catch (err) {
    console.log(`  Pre-aborted fetch throws immediately: ${err.name}`);
  }
};

const main = async () => {
  console.log("=== AbortController / AbortSignal Demo ===\n");
  await demoManualAbort();
  await demoTimeoutSignal();
  await demoAnySignal();
  await demoFetchAbort();
  console.log("\n=== Key Rules ===");
  console.log("1. signal.throwIfAborted() only checks current state — not a subscription");
  console.log("2. Operations must register signal.addEventListener('abort', ...) to react");
  console.log("3. Aborting fetch() does NOT guarantee the request never reached the server");
};

main().catch(console.error);
