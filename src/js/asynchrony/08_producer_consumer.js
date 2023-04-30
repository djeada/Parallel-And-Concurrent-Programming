/*
This script demonstrates the producer-consumer pattern using Node.js's async and promises.
The producer creates items and puts them into a queue, while the consumer takes items from the queue and processes them.
This pattern is useful when you want to separate the creation and processing of items, allowing them to run concurrently.
*/

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class AsyncQueue {
  constructor() {
    this.queue = [];
    this.resolveQueue = [];
  }

  put(item) {
    if (this.resolveQueue.length > 0) {
      this.resolveQueue.shift()(item);
    } else {
      this.queue.push(item);
    }
  }

  async get() {
    if (this.queue.length > 0) {
      return this.queue.shift();
    } else {
      return new Promise((resolve) => this.resolveQueue.push(resolve));
    }
  }
}

async function producer(queue, producerId) {
  for (let i = 0; i < 5; ++i) {
    const item = `Item ${i} from producer ${producerId}`;
    queue.put(item);
    console.log(`Producer ${producerId} produced ${item}`);
    await sleep(Math.random() * 1500 + 500);
  }
}

async function consumer(queue, consumerId) {
  for (let i = 0; i < 5; ++i) {
    const item = await queue.get();
    console.log(`Consumer ${consumerId} consumed ${item}`);
    await sleep(Math.random() * 1500 + 500);
  }
}

async function main() {
  const queue = new AsyncQueue();

  const producers = [];
  for (let i = 0; i < 3; ++i) {
    producers.push(producer(queue, i));
  }

  const consumers = [];
  for (let i = 0; i < 3; ++i) {
    consumers.push(consumer(queue, i));
  }

  await Promise.all([...producers, ...consumers]);
}

main();
