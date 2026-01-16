/*
 * Pipe-Based Inter-Process Communication
 *
 * This script demonstrates using stdio pipes for communication between
 * parent and child processes. Pipes provide a stream-based interface for
 * transferring data.
 *
 * Key concepts:
 * - spawn() with stdio configuration for pipes
 * - Stream-based communication (stdout/stdin)
 * - Process data flow with readable/writable streams
 * - Handling chunked data
 */

"use strict";

const { spawn } = require("child_process");
const { Readable } = require("stream");

const random = (min, max) => Math.floor(Math.random() * (max - min + 1)) + min;

if (process.argv[2] === "child") {
  // Child process - generates data and writes to stdout
  console.error("[Child] Starting data generation...");
  
  const readable = new Readable({
    read() {},
  });

  const NUM_ITEMS = 5;
  let itemsGenerated = 0;

  const generateItem = () => {
    if (itemsGenerated >= NUM_ITEMS) {
      console.error("[Child] All items generated, closing stream");
      readable.push(null);
      return;
    }

    const item = random(1, 100);
    const delay = random(200, 800);
    
    setTimeout(() => {
      console.error(`[Child] Generated: ${item}`);
      readable.push(`${item}\n`);
      itemsGenerated++;
      generateItem();
    }, delay);
  };

  generateItem();
  readable.pipe(process.stdout);

} else {
  // Parent process - receives data from child
  console.log("=== Pipe Communication Demo ===\n");
  console.log("Demonstrating stream-based IPC using pipes\n");

  const childProcess = spawn(process.argv[0], [__filename, "child"], {
    stdio: ["pipe", "pipe", "inherit"],
  });

  const receivedItems = [];
  let buffer = "";

  childProcess.stdout.setEncoding("utf8");
  
  childProcess.stdout.on("data", (chunk) => {
    buffer += chunk;
    
    // Process complete lines
    const lines = buffer.split("\n");
    buffer = lines.pop(); // Keep incomplete line in buffer
    
    for (const line of lines) {
      if (line.trim()) {
        const value = parseInt(line.trim(), 10);
        receivedItems.push(value);
        console.log(`[Parent] Received via pipe: ${value}`);
      }
    }
  });

  childProcess.on("error", (error) => {
    console.error(`Child process error: ${error.message}`);
  });

  childProcess.on("exit", (code) => {
    console.log(`\n=== Summary ===`);
    console.log(`Child exited with code: ${code}`);
    console.log(`Items received: ${receivedItems.length}`);
    console.log(`Values: [${receivedItems.join(", ")}]`);
    console.log(`Sum: ${receivedItems.reduce((a, b) => a + b, 0)}`);
  });
}
