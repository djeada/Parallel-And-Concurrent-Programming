// main-worker.js
const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  // Main thread
  const numThreads = 5;
  const sharedBuffer = new SharedArrayBuffer(8);
  const counterView = new Int32Array(sharedBuffer, 0, 1);
  const doneCounterView = new Int32Array(sharedBuffer, 4, 1);
  counterView[0] = 1;

  (async () => {
    const workers = [];

    for (let i = 0; i < numThreads; i++) {
      const worker = new Worker(__filename); // Use the same file as the worker
      worker.on("message", (message) => {
        if (message.type === "print") {
          console.log(message.text);
        } else if (message.type === "done") {
          doneCounterView[0]++;
          if (doneCounterView[0] === numThreads) {
            console.log(`The final value of COUNTER is: ${counterView[0]}`);
          }
        }
      });
      worker.on("error", (error) => console.error(error));
      worker.postMessage({ type: "init", id: i, sharedBuffer });
      workers.push(worker);
    }
  })();
} else {
  // Worker thread
  let id;
  let sharedBuffer;
  let counterView;

  parentPort.on("message", async (message) => {
    if (message.type === "init") {
      id = message.id;
      sharedBuffer = message.sharedBuffer;
      counterView = new Int32Array(sharedBuffer, 0, 1);

      for (let i = 0; i < 10; i++) {
        const localCounter = counterView[0];
        const multipliedCounter = localCounter * (id + 1);
        await new Promise((resolve) => setTimeout(resolve, 100)); // Add a delay
        parentPort.postMessage({
          type: "print",
          text: `The COUNTER gets multiplied by ${id + 1}`,
        });
        counterView[0] = multipliedCounter;
      }

      parentPort.postMessage({ type: "done" });
      parentPort.close(); // Close the communication channel and allow the worker to exit gracefully
    }
  });
}
