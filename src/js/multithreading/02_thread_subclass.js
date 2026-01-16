/*
 * Thread Subclass Pattern
 *
 * This script demonstrates creating a reusable thread wrapper class in Node.js.
 * It shows how to:
 * - Wrap Worker Threads in a class-based interface
 * - Pass functions to workers for execution
 * - Join (wait for) thread completion
 *
 * Key concepts:
 * - Class-based thread abstraction
 * - Function serialization for worker execution
 * - Thread synchronization with callbacks
 *
 * Note: Using eval() for function serialization is for demonstration only.
 * In production, consider using separate worker files or worker_threads' eval option.
 */

"use strict";

const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

class MyThread {
  constructor(func, ...args) {
    this.funcName = func.name || "anonymous";
    this.worker = new Worker(__filename, {
      workerData: { func: func.toString(), args },
    });

    this.worker.on("message", (message) => {
      console.log(`  [${this.funcName}] ${message}`);
    });

    this.worker.on("error", (error) => {
      console.error(`  [${this.funcName}] Error: ${error.message}`);
    });

    this.worker.on("exit", (code) => {
      if (code !== 0) {
        console.error(`  [${this.funcName}] Exited with code ${code}`);
      }
    });
  }

  join(callback) {
    this.worker.on("exit", callback);
    return this;
  }
}

const myFunction = () => {
  parentPort.postMessage("Going to sleep for 2 seconds...");
  setTimeout(() => {
    parentPort.postMessage("Woke up and completed!");
  }, 2000);
};

const main = () => {
  console.log("=== Thread Subclass Pattern Demo ===\n");

  const threadA = new MyThread(myFunction);
  const threadB = new MyThread(() => {
    parentPort.postMessage("Lambda function executing");
    parentPort.postMessage("Lambda function completed!");
  });

  let finishedThreads = 0;
  const totalThreads = 2;

  const onThreadFinished = () => {
    finishedThreads++;
    if (finishedThreads === totalThreads) {
      console.log("\n=== All threads finished ===");
    }
  };

  threadA.join(onThreadFinished);
  threadB.join(onThreadFinished);
};

if (isMainThread) {
  main();
} else {
  try {
    const func = eval(`(${workerData.func})`);
    func(...workerData.args);
  } catch (error) {
    parentPort.postMessage(`Error: ${error.message}`);
  }
}
