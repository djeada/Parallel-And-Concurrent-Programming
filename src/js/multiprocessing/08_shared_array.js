/*
 * Shared Array Pattern
 *
 * This script demonstrates simulating shared array state between processes
 * using message passing. Each child process modifies the array through the
 * parent process, which acts as the state manager.
 *
 * Key concepts:
 * - Array state management across processes
 * - Centralized state in parent process
 * - Coordinated modifications
 * - State broadcast to children
 */

"use strict";

const { fork } = require("child_process");

const ARRAY_SIZE = 5;
const NUM_ITERATIONS = 3;
const INTERVAL_MS = 500;

if (process.argv[2] === "child") {
  // Child process - sends increment/decrement requests for array
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
    if (message.type === "arrayUpdate") {
      console.log(`  [${action}er] Array is now: [${message.array.join(", ")}]`);
    } else if (message.type === "continue") {
      setTimeout(doAction, INTERVAL_MS);
    }
  });

  // Start the action cycle
  doAction();

} else {
  // Parent process - manages shared array
  console.log("=== Shared Array Demo ===\n");
  console.log(`Array size: ${ARRAY_SIZE}`);
  console.log(`Iterations per child: ${NUM_ITERATIONS}\n`);

  let sharedArray = new Array(ARRAY_SIZE).fill(0);
  let completedProcesses = 0;

  const incrementer = fork(__filename, ["child", "increment"]);
  const decrementer = fork(__filename, ["child", "decrement"]);

  const handleMessage = (childName, child) => (message) => {
    if (message.action === "increment") {
      sharedArray = sharedArray.map((v) => v + 1);
      console.log(`[Parent] ${childName} incremented -> [${sharedArray.join(", ")}]`);
      child.send({ type: "arrayUpdate", array: sharedArray });
      child.send({ type: "continue" });
    } else if (message.action === "decrement") {
      sharedArray = sharedArray.map((v) => v - 1);
      console.log(`[Parent] ${childName} decremented -> [${sharedArray.join(", ")}]`);
      child.send({ type: "arrayUpdate", array: sharedArray });
      child.send({ type: "continue" });
    } else if (message.action === "done") {
      completedProcesses++;
      console.log(`[Parent] ${childName} completed`);
      
      if (completedProcesses === 2) {
        console.log(`\n=== Final Result ===`);
        console.log(`Shared array: [${sharedArray.join(", ")}]`);
        console.log(`Expected: all zeros (${NUM_ITERATIONS} increments + ${NUM_ITERATIONS} decrements)`);
        console.log(`Sum: ${sharedArray.reduce((a, b) => a + b, 0)}`);
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
