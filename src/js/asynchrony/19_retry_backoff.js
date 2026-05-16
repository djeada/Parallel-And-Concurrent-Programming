/*
 * Retry with Exponential Backoff and Circuit Breaker
 *
 * Two essential resilience patterns for async operations that call unreliable
 * external services (HTTP APIs, databases, message queues).
 *
 * RETRY with Exponential Backoff + Jitter
 *   - On failure, wait before retrying; double the wait each attempt
 *   - Add random jitter to prevent "thundering herd" (many clients retrying in sync)
 *   - Stop after maxAttempts or when error is non-retryable
 *
 * CIRCUIT BREAKER
 *   - Tracks consecutive failures; opens the circuit after a threshold
 *   - While OPEN: reject immediately without calling the service (fail fast)
 *   - After a recovery timeout: move to HALF-OPEN; allow one probe request
 *   - On probe success: CLOSE the circuit; on probe failure: OPEN again
 *
 * Key concepts:
 * - Jitter prevents synchronized retries that overload a recovering service
 * - Circuit breaker avoids cascading failures when a downstream is down
 * - Non-retryable errors (e.g. 4xx) should bypass the retry loop
 */

"use strict";

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// ── Retry with exponential backoff + full jitter ──────────────────────────

class RetryableError extends Error {}
class NonRetryableError extends Error {}

/**
 * Retries `fn` up to `maxAttempts` times using exponential backoff with
 * full jitter. Throws immediately for NonRetryableError.
 *
 * @param {() => Promise<any>} fn           - The operation to retry
 * @param {{ maxAttempts?, baseDelayMs?, maxDelayMs? }} [options]
 */
const withRetry = async (fn, { maxAttempts = 5, baseDelayMs = 100, maxDelayMs = 5000 } = {}) => {
  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    try {
      return await fn();
    } catch (err) {
      if (err instanceof NonRetryableError) {
        throw err; // no point retrying 4xx-style errors
      }

      if (attempt === maxAttempts) {
        throw new Error(`Failed after ${maxAttempts} attempts: ${err.message}`);
      }

      // Exponential cap + full jitter: delay is random in [0, min(cap, base * 2^attempt)]
      const cap = Math.min(maxDelayMs, baseDelayMs * Math.pow(2, attempt));
      const jitter = Math.random() * cap;
      console.log(
        `  Attempt ${attempt} failed (${err.message}). Retrying in ${jitter.toFixed(0)}ms...`
      );
      await sleep(jitter);
    }
  }
};

// ── Circuit Breaker ───────────────────────────────────────────────────────

const CircuitState = Object.freeze({ CLOSED: "CLOSED", OPEN: "OPEN", HALF_OPEN: "HALF_OPEN" });

class CircuitBreaker {
  /**
   * @param {number} failureThreshold  - Consecutive failures before opening
   * @param {number} recoveryTimeoutMs - Time to wait before attempting HALF_OPEN
   */
  constructor(failureThreshold = 3, recoveryTimeoutMs = 1000) {
    this.threshold = failureThreshold;
    this.recoveryTimeout = recoveryTimeoutMs;
    this.state = CircuitState.CLOSED;
    this.failures = 0;
    this.openedAt = null;
  }

  async call(fn) {
    if (this.state === CircuitState.OPEN) {
      const elapsed = Date.now() - this.openedAt;
      if (elapsed < this.recoveryTimeout) {
        throw new Error(`Circuit OPEN — fast failing (${elapsed}ms < ${this.recoveryTimeout}ms timeout)`);
      }
      // Recovery timeout elapsed — try one probe
      this.state = CircuitState.HALF_OPEN;
      console.log("  Circuit → HALF_OPEN (probing)");
    }

    try {
      const result = await fn();
      this._onSuccess();
      return result;
    } catch (err) {
      this._onFailure();
      throw err;
    }
  }

  _onSuccess() {
    this.failures = 0;
    if (this.state === CircuitState.HALF_OPEN) {
      console.log("  Circuit → CLOSED (probe succeeded)");
    }
    this.state = CircuitState.CLOSED;
  }

  _onFailure() {
    this.failures++;
    if (this.state === CircuitState.HALF_OPEN || this.failures >= this.threshold) {
      this.state = CircuitState.OPEN;
      this.openedAt = Date.now();
      console.log(`  Circuit → OPEN after ${this.failures} consecutive failures`);
    }
  }

  get status() {
    return this.state;
  }
}

// ── Demo helpers ──────────────────────────────────────────────────────────

let callCount = 0;

// Simulates an unreliable service: fails for the first N calls, then succeeds.
const makeUnreliableService = (failFirstN) => async () => {
  callCount++;
  if (callCount <= failFirstN) {
    throw new RetryableError(`Service unavailable (call ${callCount})`);
  }
  return `Success on call ${callCount}`;
};

// ── Demo 1: Retry ─────────────────────────────────────────────────────────
const demoRetry = async () => {
  console.log("--- Demo 1: Retry with Exponential Backoff ---");
  callCount = 0;

  // Fails first 3 calls, succeeds on call 4
  const unreliable = makeUnreliableService(3);

  try {
    const result = await withRetry(unreliable, {
      maxAttempts: 5,
      baseDelayMs: 20,
      maxDelayMs: 200,
    });
    console.log(`  Final result: ${result}`);
  } catch (err) {
    console.log(`  Exhausted retries: ${err.message}`);
  }

  // Non-retryable error — should throw immediately
  console.log("\n  Non-retryable error (no retries):");
  try {
    await withRetry(
      async () => {
        throw new NonRetryableError("Bad request (4xx)");
      },
      { maxAttempts: 5 }
    );
  } catch (err) {
    console.log(`  Threw immediately: ${err.message}`);
  }
};

// ── Demo 2: Circuit Breaker ───────────────────────────────────────────────
const demoCircuitBreaker = async () => {
  console.log("\n--- Demo 2: Circuit Breaker ---");

  const cb = new CircuitBreaker(3, 300); // open after 3 failures; 300 ms recovery
  let serviceDown = true;

  const service = async () => {
    if (serviceDown) throw new Error("Service down");
    return "OK";
  };

  // Trip the circuit with 3 consecutive failures
  for (let i = 0; i < 3; i++) {
    try {
      await cb.call(service);
    } catch (err) {
      console.log(`  Call ${i + 1}: ${err.message} [circuit: ${cb.status}]`);
    }
  }

  // Circuit is now OPEN — fast-fail immediately
  try {
    await cb.call(service);
  } catch (err) {
    console.log(`  Fast-fail: ${err.message}`);
  }

  // Restore service and wait for recovery timeout
  console.log("  Waiting for recovery timeout (300ms)...");
  serviceDown = false;
  await sleep(350);

  // HALF_OPEN probe succeeds → circuit closes
  const result = await cb.call(service);
  console.log(`  Probe result: ${result} [circuit: ${cb.status}]`);
};

const main = async () => {
  console.log("=== Retry and Circuit Breaker Demo ===\n");
  await demoRetry();
  await demoCircuitBreaker();
  console.log("\n=== Summary ===");
  console.log("Retry backoff: random jitter prevents thundering herd on recovery");
  console.log("Circuit breaker: fail fast when service is down, probe on recovery timeout");
};

main().catch(console.error);
