class AsyncBarrier {
  constructor(numTasks) {
    this.numTasks = numTasks;
    this.counter = 0;
    this.resolveBarrier = null;
    this.barrierPromise = new Promise((resolve) => {
      this.resolveBarrier = resolve;
    });
  }

  async wait() {
    this.counter++;
    if (this.counter >= this.numTasks) {
      this.resolveBarrier();
    }
    await this.barrierPromise;
  }
}

async function task(id, barrier) {
  console.log(`Task ${id} starting...`);
  await new Promise((resolve) => setTimeout(resolve, Math.random() * 1000));
  console.log(`Task ${id} finished...`);

  await barrier.wait();
  console.log(`Task ${id} after the barrier...`);
}

async function main() {
  const numTasks = 5;
  const barrier = new AsyncBarrier(numTasks);

  const tasks = Array.from({ length: numTasks }, (_, i) => task(i, barrier));
  await Promise.all(tasks);
}

main();
