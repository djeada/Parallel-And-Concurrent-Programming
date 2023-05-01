const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  // Main thread
  const worker = new Worker(__filename); // Execute the same file as a worker

  worker.on("message", (message) => {
    console.log(message);
  });

  worker.postMessage({ interval: 1000, start: true });

  setTimeout(() => {
    worker.postMessage({ start: false });
  }, 10000);
} else {
  // Worker thread
  let timer;

  parentPort.on("message", (message) => {
    const { interval, start } = message;

    if (start) {
      timer = setInterval(() => {
        parentPort.postMessage("It works");
      }, interval);
    } else {
      clearInterval(timer);
      parentPort.close();
    }
  });
}
