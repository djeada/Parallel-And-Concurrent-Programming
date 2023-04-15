const { Worker, isMainThread, parentPort } = require('worker_threads');

if (isMainThread) {
  // Main thread
  const numWorkers = 4;
  const workers = [];
  const sharedCounter = new SharedArrayBuffer(4);
  const counterView = new Int32Array(sharedCounter);
  counterView[0] = 0;

  for (let i = 0; i < numWorkers; i++) {
    const worker = new Worker(__filename); // Use the same file as the worker
    worker.on('message', (message) => {
      if (message.type === 'print') {
        console.log(message.text);
      }
    });
    worker.on('error', (error) => console.error(error));
    worker.postMessage({ type: 'init', id: i, sharedCounter });
    workers.push(worker);
  }

  let currentWorker = 0;
  function grantMutex() {
    workers[currentWorker].postMessage({ type: 'grantMutex' });
    currentWorker = (currentWorker + 1) % numWorkers;
  }

  workers.forEach((worker, index) => {
    worker.on('message', (msg) => {
      if (msg.type === 'releaseMutex') {
        grantMutex();
      }
    });
  });

  grantMutex(); // Grant mutex to the first worker
} else {
  // Worker thread
  let id;
  let sharedCounter;
  let counterView;

  parentPort.on('message', async (message) => {
    if (message.type === 'init') {
      id = message.id;
      sharedCounter = message.sharedCounter;
      counterView = new Int32Array(sharedCounter);
      parentPort.postMessage({ type: 'requestMutex' });
    } else if (message.type === 'grantMutex') {
      for (let i = 0; i < 100; i++) {
        parentPort.postMessage({ type: 'print', text: `Worker id: ${id}, Counter: ${counterView[0]}` });
        Atomics.add(counterView, 0, 1);
        await new Promise((resolve) => setTimeout(resolve, 50));
      }
      parentPort.postMessage({ type: 'releaseMutex' });
    }
  });
}
