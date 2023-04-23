// thread_pool_example.js
const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

function worker(taskId) {
  console.log(`Task ${taskId} is starting...`);
  const sleepDuration = Math.floor(Math.random() * 3000) + 1000; // Simulate some work
  setTimeout(() => {
    const result = taskId * 2;
    console.log(`Task ${taskId} is finished. Result: ${result}`);
    parentPort.postMessage({ taskId, result });
  }, sleepDuration);
}

if (isMainThread) {
  const numTasks = 10;
  const numWorkers = 3;
  const taskQueue = Array.from({ length: numTasks }, (_, i) => i);
  const workers = [];
  let completedTasks = 0;

  function createWorker() {
    const taskId = taskQueue.shift();
    if (taskId === undefined) return;

    const worker = new Worker(__filename, { workerData: taskId });
    worker.on('message', ({ taskId, result }) => {
      console.log(`Task ${taskId} result collected: ${result}`);
      completedTasks++;
      if (completedTasks < numTasks) {
        createWorker();
      }
    });
  }

  for (let i = 0; i < numWorkers; i++) {
    createWorker();
  }
} else {
  worker(workerData);
}
