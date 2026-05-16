/*
 * Node.js Streams and Async Pipeline
 *
 * Node.js streams model flow-controlled data: a Readable produces data,
 * a Writable consumes it. Connecting them naively ignores backpressure —
 * the producer can overwhelm the consumer's internal buffer, wasting memory.
 *
 * Patterns covered:
 *  1. for await...of — async iteration over a Readable stream
 *  2. Manual backpressure — observing writable.write() return value + drain event
 *  3. stream.pipeline() — correct composition: wires backpressure and propagates errors
 *
 * Key concepts:
 * - A Readable enters "flowing" mode as soon as a listener is added (data event or pipe).
 * - A Writable signals backpressure by returning false from .write(); the producer
 *   must stop writing and wait for the "drain" event before resuming.
 * - stream.pipeline() handles backpressure and calls the callback on error or completion.
 * - for await...of over a Readable respects backpressure automatically.
 * - The older stream.pipe() does NOT forward errors — prefer pipeline() in new code.
 */

"use strict";

const { Readable, Writable, Transform, pipeline } = require("stream");
const { promisify } = require("util");
const { once } = require("events");

const pipelineAsync = promisify(pipeline);
const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// ── Helper: create a Readable that emits N numbered chunks ────────────────
const makeCountingReadable = (n, label = "") =>
  new Readable({
    objectMode: true,
    read() {},
    construct(cb) {
      let i = 0;
      const push = () => {
        if (i >= n) {
          this.push(null); // signal end-of-stream
          return;
        }
        this.push({ index: i++, label });
        setImmediate(push); // yield to event loop between chunks
      };
      push();
      cb();
    },
  });

// ── Helper: create a Writable that logs chunks with optional artificial lag ─
const makeLoggingWritable = (lag = 0, highWaterMark = 8) =>
  new Writable({
    objectMode: true,
    highWaterMark, // internal buffer threshold — backpressure kicks in above this
    async write(chunk, _enc, callback) {
      if (lag) await sleep(lag);
      process.stdout.write(`  consumed: ${chunk.label}[${chunk.index}]\n`);
      callback();
    },
  });

// ── Demo 1: for await...of ────────────────────────────────────────────────
// The cleanest way to consume a Readable when you do not need to compose it
// with other streams. Automatically respects backpressure and handles errors.
const demoAsyncIteration = async () => {
  console.log("--- Demo 1: for await...of over a Readable ---");

  const source = makeCountingReadable(5, "iter");
  for await (const chunk of source) {
    console.log(`  received: ${chunk.label}[${chunk.index}]`);
  }
  console.log("  Stream ended ✓");
};

// ── Demo 2: Manual backpressure ───────────────────────────────────────────
// When you write chunks one-by-one to a Writable:
//   - .write() returns true  → buffer is below highWaterMark; keep writing
//   - .write() returns false → buffer is full; STOP and wait for 'drain'
// Ignoring the return value causes unbounded memory growth.
const demoManualBackpressure = async () => {
  console.log("\n--- Demo 2: Manual backpressure (write + drain) ---");

  const writable = makeLoggingWritable(10, 2); // 10 ms lag, small buffer (hwm=2)

  let drainCount = 0;

  for (let i = 0; i < 8; i++) {
    const chunk = { index: i, label: "manual" };
    const ok = writable.write(chunk);
    if (!ok) {
      drainCount++;
      // Buffer full — wait for writable to drain before continuing
      await once(writable, "drain");
    }
  }

  // Signal end and wait for finish
  writable.end();
  await once(writable, "finish");
  console.log(`  Done. Waited for 'drain' ${drainCount} time(s) — backpressure working ✓`);
};

// ── Demo 3: stream.pipeline() ─────────────────────────────────────────────
// pipeline() wires Readable → Transform → Writable correctly:
//   - Propagates errors from any stage to the callback
//   - Destroys all streams on error to prevent leaks
//   - Handles backpressure automatically
// Prefer it over .pipe() in all new Node.js code.
const demoPipeline = async () => {
  console.log("\n--- Demo 3: stream.pipeline() ---");

  const source = makeCountingReadable(6, "pipe");

  // Transform: keep only even-indexed chunks
  const filterEven = new Transform({
    objectMode: true,
    transform(chunk, _enc, callback) {
      if (chunk.index % 2 === 0) {
        this.push({ ...chunk, label: "filtered" });
      }
      callback();
    },
  });

  const sink = makeLoggingWritable(5, 4);

  await pipelineAsync(source, filterEven, sink);
  console.log("  pipeline() completed ✓");
};

// ── Demo 4: pipeline() error propagation ──────────────────────────────────
// An error thrown inside any stream stage propagates to the callback and
// all streams are destroyed, preventing resource leaks.
const demoPipelineError = async () => {
  console.log("\n--- Demo 4: pipeline() error propagation ---");

  const source = makeCountingReadable(10, "err");

  const faultyTransform = new Transform({
    objectMode: true,
    transform(chunk, _enc, callback) {
      if (chunk.index === 3) {
        callback(new Error("Transform error at index 3"));
      } else {
        this.push(chunk);
        callback();
      }
    },
  });

  const sink = makeLoggingWritable(0, 16);

  try {
    await pipelineAsync(source, faultyTransform, sink);
  } catch (err) {
    console.log(`  pipeline() caught error: ${err.message} ✓`);
    console.log(`  source destroyed: ${source.destroyed}, sink destroyed: ${sink.destroyed}`);
  }
};

const main = async () => {
  console.log("=== Async Streams and Pipeline Demo ===\n");
  await demoAsyncIteration();
  await demoManualBackpressure();
  await demoPipeline();
  await demoPipelineError();
  console.log("\n=== Summary ===");
  console.log("for await...of   — cleanest; respects backpressure automatically");
  console.log("manual write+drain — full control; observe .write() return and wait for drain");
  console.log("pipeline()       — composable; backpressure + error propagation + cleanup");
  console.log("pipe()           — legacy; does NOT forward errors — avoid in new code");
};

main().catch(console.error);
