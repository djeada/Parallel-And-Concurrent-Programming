const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
  threadId,
} = require("worker_threads");

function main() {
  console.log(`Main function process id: ${process.pid}`);
  console.log(`Main thread id: ${threadId}`);

  const worker = new Worker(__filename);
  worker.on("message", (message) => {
    console.log(message);
  });
  worker.on("exit", () => {
    console.log("Worker finished");
  });
}

if (isMainThread) {
  main();
} else {
  console.log("Worker function");
  console.log(`Worker function process id: ${process.pid}`);
  console.log(`Worker thread id: ${threadId}`);
  parentPort.postMessage(`Worker thread name: Worker (${threadId})`);
}
