/*
 * Shared Value Pattern
 *
 * This script demonstrates simulating shared state between processes using
 * message passing. In Node.js, processes don't share memory directly,
 * so we coordinate through IPC to maintain a "shared" value.
 *
 * Key concepts:
 * - Message-based state sharing
 * - Centralized state management in parent process
 * - Coordinating multiple child processes
 * - State synchronization
 */

"use strict";

const { fork } = require("child_process");

const NUM_ITERATIONS = 5;
const INTERVAL_MS = 500;

if (process.argv[2] === "child") {
  // Child process - sends increment/decrement requests
  const action = process.argv[3];
  let count = 0;

  const doAction = () => {
    if (count >= NUM_ITERATIONS) {
      process.send({ action: "done" });
      return;
    }
    
    process.send({ action });
    count++;
  };

  process.on("message", (message) => {
    if (message.type === "valueUpdate") {
      console.log(`  [${action}er] Shared value is now: ${message.value}`);
    } else if (message.type === "continue") {
      setTimeout(doAction, INTERVAL_MS);
    }
  });

  // Start the action cycle
  doAction();

} else {
  // Parent process - manages shared state
  console.log("=== Shared Value Demo ===\n");
  console.log("Simulating shared state across processes using IPC\n");

  let sharedValue = 0;
  let completedProcesses = 0;

  const incrementer = fork(__filename, ["child", "increment"]);
  const decrementer = fork(__filename, ["child", "decrement"]);

  const handleMessage = (childName, child) => (message) => {
    if (message.action === "increment") {
      sharedValue++;
      console.log(`[Parent] ${childName} incremented -> ${sharedValue}`);
      child.send({ type: "valueUpdate", value: sharedValue });
      child.send({ type: "continue" });
    } else if (message.action === "decrement") {
      sharedValue--;
      console.log(`[Parent] ${childName} decremented -> ${sharedValue}`);
      child.send({ type: "valueUpdate", value: sharedValue });
      child.send({ type: "continue" });
    } else if (message.action === "done") {
      completedProcesses++;
      console.log(`[Parent] ${childName} completed`);
      
      if (completedProcesses === 2) {
        console.log(`\n=== Final Result ===`);
        console.log(`Shared value: ${sharedValue}`);
        console.log(`Expected: 0 (${NUM_ITERATIONS} increments + ${NUM_ITERATIONS} decrements)`);
        incrementer.kill();
        decrementer.kill();
      }
    }
  };

  incrementer.on("message", handleMessage("Incrementer", incrementer));
  decrementer.on("message", handleMessage("Decrementer", decrementer));

  incrementer.on("error", (err) => console.error(`Incrementer error: ${err.message}`));
  decrementer.on("error", (err) => console.error(`Decrementer error: ${err.message}`));
}
