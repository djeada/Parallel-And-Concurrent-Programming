const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

if (isMainThread) {
  const sharedArrayBuffer = new SharedArrayBuffer(4 * Int32Array.BYTES_PER_ELEMENT);
  const sharedData = new Int32Array(sharedArrayBuffer);
  sharedData[0] = 0; // data
  sharedData[1] = 0; // readers
  sharedData[2] = 0; // writers
  sharedData[3] = 0; // waiting writers

  const numReaders = 3;
  const numWriters = 2;

  for (let i = 0; i < numReaders; i++) {
    new Worker(__filename, { workerData: { type: 'reader', threadId: i, sharedArrayBuffer } });
  }

  for (let i = 0; i < numWriters; i++) {
    new Worker(__filename, { workerData: { type: 'writer', threadId: i, sharedArrayBuffer } });
  }
} else {
  const { type, threadId, sharedArrayBuffer } = workerData;
  const sharedData = new Int32Array(sharedArrayBuffer);

  if (type === 'reader') {
    while (true) {
      while (Atomics.load(sharedData, 2) > 0) {
        Atomics.wait(sharedData, 2, 0, 1000);
      }

      Atomics.add(sharedData, 1, 1);
      console.log(`Reader ${threadId} reads data: ${sharedData[0]}`);
      Atomics.add(sharedData, 1, -1);
      Atomics.notify(sharedData, 2, 1);

      Atomics.wait(sharedData, 0, 0, 1000);
    }
  } else if (type === 'writer') {
    while (true) {
      Atomics.add(sharedData, 3, 1);
      while (Atomics.load(sharedData, 2) > 0 || Atomics.load(sharedData, 1) > 0) {
        Atomics.wait(sharedData, 3, 0, 1000);
      }
      Atomics.add(sharedData, 3, -1);

      Atomics.add(sharedData, 2, 1);
      console.log(`Writer ${threadId} is writing...`);
      sharedData[0]++;
      console.log(`Writer ${threadId} finished writing. New data: ${sharedData[0]}`);
      Atomics.add(sharedData, 2, -1);
      Atomics.notify(sharedData, 3, 1);

      Atomics.wait(sharedData, 0, 0, 1000);
    }
  }
}
