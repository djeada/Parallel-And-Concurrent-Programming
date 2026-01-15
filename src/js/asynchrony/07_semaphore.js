/*
This script demonstrates the use of a semaphore to limit concurrent access to a 
resource in an asynchronous environment. A semaphore allows a specified number 
of tasks to access a resource simultaneously, while other tasks must wait until 
a slot becomes available.

In this example, we create a simple Semaphore class and use it to limit 
concurrent access to a simulated resource to a maximum of 3 tasks at a time.
*/

class Semaphore {
  constructor(maxConcurrent) {
    this.maxConcurrent = maxConcurrent;
    this.current = 0;
    this.queue = [];
  }

  async acquire() {
    if (this.current < this.maxConcurrent) {
      this.current++;
      return;
    }

    return new Promise((resolve) => {
      this.queue.push(resolve);
    });
  }

  release() {
    if (this.queue.length > 0) {
      const resolve = this.queue.shift();
      resolve();
    } else {
      this.current--;
    }
  }
}

const resourceSemaphore = new Semaphore(3);

async function limitedResource(taskId) {
  await resourceSemaphore.acquire();
  console.log(`Task ${taskId} is using the limited resource.`);
  await new Promise((resolve) =>
    setTimeout(resolve, Math.floor(Math.random() * 2000) + 500)
  );
  console.log(`Task ${taskId} is done using the limited resource.`);
  resourceSemaphore.release();
}

async function main() {
  const tasks = [];

  for (let i = 0; i < 10; i++) {
    tasks.push(limitedResource(i));
  }

  await Promise.all(tasks);
}

main();
