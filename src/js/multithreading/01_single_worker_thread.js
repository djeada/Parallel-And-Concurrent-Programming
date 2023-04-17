const { Worker, isMainThread, threadId, parentPort } = require('worker_threads');

function main() {
  console.log(`Main function process id: ${process.pid}`);
  console.log(`Main thread id: ${threadId}`);

  const worker = new Worker(__filename);

  worker.on('message', (message) => {
    console.log(message);
  });

  worker.on('exit', () => {
    console.log('Worker thread exited.');
  });
}

function workerFunction() {
  parentPort.postMessage(`Worker function`);
  parentPort.postMessage(`Worker function process id: ${process.pid}`);
  parentPort.postMessage(`Worker thread id: ${threadId}`);
}

if (isMainThread) {
  main();
} else {
  workerFunction();
}
