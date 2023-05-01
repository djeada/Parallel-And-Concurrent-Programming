/*
This program demonstrates a deadlock situation using multiple processes in Node.js.
A deadlock occurs when two processes are waiting for each other to release a lock, creating
a cycle that never ends. In this example, two processes each acquire a lock and then try
to acquire the other lock, causing a deadlock.

Note: This example uses a simple lock mechanism through messages and does not
use a proper lock library for demonstration purposes.
*/

const { fork } = require("child_process");

if (process.argv.length === 2) {
  // Main process
  const workerA = fork(__filename, ["workerA"]);
  const workerB = fork(__filename, ["workerB"]);

  let lockA = false;
  let lockB = false;

  workerA.on("message", (message) => {
    if (message === "requestLockA") {
      if (!lockA) {
        lockA = true;
        workerA.send("grantLockA");
      }
    } else if (message === "requestLockB") {
      if (!lockB) {
        lockB = true;
        workerA.send("grantLockB");
      }
    }
  });

  workerB.on("message", (message) => {
    if (message === "requestLockA") {
      if (!lockA) {
        lockA = true;
        workerB.send("grantLockA");
      }
    } else if (message === "requestLockB") {
      if (!lockB) {
        lockB = true;
        workerB.send("grantLockB");
      }
    }
  });
} else {
  // Worker process
  const workerName = process.argv[2];

  process.send(`requestLock${workerName === "workerA" ? "A" : "B"}`);
  process.on("message", (message) => {
    if (message === `grantLock${workerName === "workerA" ? "A" : "B"}`) {
      console.log(
        `${workerName} acquired lock ${workerName === "workerA" ? "A" : "B"}`
      );

      setTimeout(() => {
        console.log(
          `${workerName} trying to acquire lock ${
            workerName === "workerA" ? "B" : "A"
          }`
        );
        process.send(`requestLock${workerName === "workerA" ? "B" : "A"}`);
      }, 1000);
    } else if (message === `grantLock${workerName === "workerA" ? "B" : "A"}`) {
      console.log(
        `${workerName} acquired lock ${workerName === "workerA" ? "B" : "A"}`
      );
      // Simulate deadlock by not releasing the locks
    }
  });
}
