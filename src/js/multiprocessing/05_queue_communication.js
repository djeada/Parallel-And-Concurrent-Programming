/*
 * Queue-Based Inter-Process Communication
 *
 * This script demonstrates using a message queue for communication between
 * parent and child processes. The child process acts as a producer that
 * creates items, and the parent consumes them.
 *
 * Key concepts:
 * - Message passing with process.send() and process.on('message')
 * - Queue data structure for buffering
 * - Producer-consumer pattern across processes
 * - Bidirectional IPC communication
 */

"use strict";

const { fork } = require("child_process");

const random = (min, max) => Math.floor(Math.random() * (max - min + 1)) + min;

class Queue {
  constructor() {
    this.items = [];
  }

  push(item) {
    this.items.push(item);
  }

  pop() {
    return this.items.length > 0 ? this.items.shift() : null;
  }

  isEmpty() {
    return this.items.length === 0;
  }

  size() {
    return this.items.length;
  }
}

if (process.argv[2] === "child") {
  // Child process - producer
  const queue = new Queue();

  process.on("message", (message) => {
    if (message.type === "produce") {
      const item = random(1, 100);
      queue.push(item);
      console.log(`  [Child] Produced item: ${item} (queue size: ${queue.size()})`);
      process.send({ type: "produced", item });
    } else if (message.type === "consume") {
      if (!queue.isEmpty()) {
        const item = queue.pop();
        console.log(`  [Child] Sending item: ${item} to parent`);
        process.send({ type: "item", value: item });
      } else {
        process.send({ type: "empty" });
      }
    }
  });

} else {
  // Parent process - consumer
  console.log("=== Queue Communication Demo ===\n");
  console.log("Demonstrating producer-consumer pattern across processes\n");

  const childProcess = fork(__filename, ["child"], { stdio: "inherit" });

  const NUM_ITEMS = 5;
  let producedCount = 0;
  let consumedCount = 0;
  const receivedItems = [];

  // Produce items
  console.log("--- Producing items ---");
  for (let i = 0; i < NUM_ITEMS; i++) {
    childProcess.send({ type: "produce" });
  }

  // Consume items after a delay
  setTimeout(() => {
    console.log("\n--- Consuming items ---");
    for (let i = 0; i < NUM_ITEMS; i++) {
      childProcess.send({ type: "consume" });
    }
  }, 500);

  childProcess.on("message", (message) => {
    if (message.type === "produced") {
      producedCount++;
    } else if (message.type === "item") {
      receivedItems.push(message.value);
      console.log(`  [Parent] Received item: ${message.value}`);
      consumedCount++;
      
      if (consumedCount === NUM_ITEMS) {
        console.log("\n=== Summary ===");
        console.log(`Items produced: ${producedCount}`);
        console.log(`Items consumed: ${consumedCount}`);
        console.log(`Received items: [${receivedItems.join(", ")}]`);
        childProcess.kill();
      }
    } else if (message.type === "empty") {
      console.log("  [Parent] Queue is empty");
    }
  });

  childProcess.on("error", (error) => {
    console.error(`Child process error: ${error.message}`);
  });
}
