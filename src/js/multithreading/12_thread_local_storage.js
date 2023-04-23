// worker_threads_example.js
const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

let globalData;

function someWork() {
  globalData *= 2;
}

function worker(threadId) {
  // Store thread-specific data
  globalData = threadId;

  // Simulate some work
  someWork();

  // Read thread-specific data
  console.log(`Thread ${threadId} has value: ${globalData}`);
}

if (isMainThread) {
  const numThreads = 5;
  const workers = [];

  for (let i = 0; i < numThreads; i++) {
    workers.push(new Worker(__filename, { workerData: i }));
  }
} else {
  worker(workerData);
}
