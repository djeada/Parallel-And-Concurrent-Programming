const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

const numProducers = 3;
const numConsumers = 3;
const queueSize = 10;
const maxItems = 10;

if (isMainThread) {
  const buffer = new SharedArrayBuffer(Int32Array.BYTES_PER_ELEMENT * (queueSize + 4));
  const queue = new Int32Array(buffer);

  const workers = [];

  for (let i = 0; i < numProducers + numConsumers; i++) {
    const worker = new Worker(__filename, {
      workerData: {
        buffer,
        role: i < numProducers ? 'producer' : 'consumer',
        index: i < numProducers ? i : i - numProducers,
      },
    });
    worker.on('message', (message) => {
      const { role, item } = message;
      console.log(
        `${role.charAt(0).toUpperCase() + role.slice(1)} ${
          role === 'producer' ? item.producerIndex + 1 : item.consumerIndex + 1
        } ${role === 'producer' ? 'produced' : 'consumed'} item ${item.value}`
      );

      if (role === 'producer') {
        Atomics.add(queue, queueSize + 3, 1);
      } else if (role === 'consumer') {
        Atomics.add(queue, queueSize + 3, -1);
      }

      if (Atomics.load(queue, queueSize + 3) === maxItems) {
        workers.forEach((worker) => worker.terminate());
      }
    });

    workers.push(worker);
  }
} else {
  const { buffer, role, index } = workerData;
  const queue = new Int32Array(buffer);

  if (role === 'producer') {
    produce(index);
  } else if (role === 'consumer') {
    consume(index);
  }

  async function produce(producerIndex) {
    if (Atomics.load(queue, queueSize + 3) >= maxItems) return;
    const item = Math.floor(Math.random() * 100) + 1;
    put(item);
    parentPort.postMessage({ role: 'producer', item: { producerIndex, value: item } });
    await new Promise((resolve) => setTimeout(resolve, Math.random() * 500));
    produce(producerIndex);
  }

  async function consume(consumerIndex) {
    if (Atomics.load(queue, queueSize + 3) <= 0) return;
    const item = get();
    parentPort.postMessage({ role: 'consumer', item: { consumerIndex, value: item } });
    await new Promise((resolve) => setTimeout(resolve, Math.random() * 500));
    consume(consumerIndex);
  }

  function put(item) {
    while (Atomics.load(queue, queueSize) >= queueSize - 1) {
      // Wait for space in the queue
    }

    Atomics.add(queue, queueSize, 1);
    Atomics.store(queue, (queue[queueSize + 1] + queueSize - 1) % queueSize
, item);
Atomics.add(queue, queueSize + 1, 1);

}

function get() {
while (Atomics.load(queue, queueSize) <= 0) {
// Wait for items in the queue
}



Atomics.add(queue, queueSize, -1);
const item = Atomics.load(queue, (queue[queueSize + 2] + queueSize - 1) % queueSize);
Atomics.add(queue, queueSize + 2, 1);

return item;

}
}

