/*
 * MessageChannel — Direct Worker-to-Worker Communication
 *
 * A MessageChannel creates a pair of linked MessagePorts.  The main thread
 * wires two workers together by transferring one port to each; after that,
 * the workers can exchange messages directly without every message being
 * relayed through the main thread.
 *
 * Key concepts:
 * - new MessageChannel() → { port1, port2 } — linked bidirectional channel
 * - Pass ports in the Worker's transferList option (alongside workerData) to
 *   move ownership rather than clone; a port can only belong to one thread
 * - After transfer, port1/port2 are detached in the main thread (unusable)
 *
 * This is preferable to routing all messages via the main thread when:
 *   - Two workers have a high-volume dialogue
 *   - You want a typed, dedicated channel between a pair of workers
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
  MessageChannel,
} = require("worker_threads");

// ── Main thread ──────────────────────────────────────────────────────────────
if (isMainThread) {
  console.log("=== MessageChannel Demo ===\n");
  console.log("Main wires two workers together, then steps back.");
  console.log("All subsequent messages flow worker-to-worker.\n");

  const { port1, port2 } = new MessageChannel();

  // Transfer port1 to the sender worker, port2 to the receiver worker.
  // Include the ports in transferList so ownership moves (no clone/copy).
  // After this, port1 and port2 are detached in the main thread.
  const sender = new Worker(__filename, {
    workerData: { role: "sender", port: port1 },
    transferList: [port1],
  });
  const receiver = new Worker(__filename, {
    workerData: { role: "receiver", port: port2 },
    transferList: [port2],
  });

  let done = 0;
  for (const w of [sender, receiver]) {
    w.on("message", (msg) => console.log(`  [main] status: ${msg}`));
    w.on("error", (err) => console.error("Worker error:", err.message));
    w.on("exit", () => { if (++done === 2) console.log("\n=== Demo complete ==="); });
  }

// ── Sender worker ─────────────────────────────────────────────────────────────
} else if (workerData.role === "sender") {
  const port = workerData.port;
  const MESSAGES = ["Hello from sender!", "Worker threads rock.", "Goodbye."];
  parentPort.postMessage("sender ready, sending 3 messages");

  for (const text of MESSAGES) {
    port.postMessage(text);
  }

  // Close our end; receiver will see a 'close' event.
  port.close();
  parentPort.postMessage("sender done");

// ── Receiver worker ───────────────────────────────────────────────────────────
} else if (workerData.role === "receiver") {
  const port = workerData.port;
  parentPort.postMessage("receiver ready, listening");

  port.on("message", (text) => {
    console.log(`  [receiver] received: "${text}"`);
  });

  port.on("close", () => {
    parentPort.postMessage("receiver done (port closed)");
  });
}
