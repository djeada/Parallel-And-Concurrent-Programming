const { Worker, isMainThread, parentPort } = require('worker_threads');
const os = require('os');

function main() {
  console.log(`Main function process id: ${process.pid}`);
  console.log(`Main thread id: ${os.threadId}`);

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
  parentPort.postMessage(`Worker thread id: ${os.threadId}`);
}

if (isMainThread) {
  main();
} else {
  workerFunction();
}
