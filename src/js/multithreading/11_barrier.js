const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');
const EventEmitter = require('events');

class Barrier {
  constructor(numThreads, sharedBuffer) {
    this.numThreads = numThreads;
    this.sharedBuffer = sharedBuffer;
    this.count = new Int32Array(sharedBuffer, 0, 1);
    this.emitter = new EventEmitter();
  }

  wait() {
    const count = Atomics.add(this.count, 0, 1);

    if (count === this.numThreads - 1) {
      this.emitter.emit('release');
    } else {
      return new Promise((resolve) => {
        this.emitter.once('release', resolve);
      });
    }
  }
}

async function worker(barrier, threadId) {
  console.log(`Thread ${threadId} is starting...`);

  const sleepDuration = Math.floor(Math.random() * 2000) + 1000;
  setTimeout(async () => {
    console.log(`Thread ${threadId} is waiting at the barrier...`);

    await barrier.wait(); // Wait for all threads to reach the barrier

    console.log(`Thread ${threadId} is resuming after the barrier...`);
    parentPort.postMessage('done');
  }, sleepDuration);
}

if (isMainThread) {
  const numThreads = 5;
  const sharedBuffer = new SharedArrayBuffer(4);
  const barrier = new Barrier(numThreads, sharedBuffer);
  const workers = [];

  let completedThreads = 0;

  for (let i = 0; i < numThreads; i++) {
    const worker = new Worker(__filename, { workerData: { sharedBuffer, threadId: i } });
    workers.push(worker);
    worker.on('message', (message) => {
      if (message === 'done') {
        completedThreads++;
        if (completedThreads === numThreads) {
          console.log('All threads have passed the barrier.');
        }
      }
    });
  }
} else {
  const barrier = new Barrier(workerData.numThreads, workerData.sharedBuffer);
  worker(barrier, workerData.threadId);
}
