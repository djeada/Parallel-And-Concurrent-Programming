const { Sema } = require("async-sema");

const resourceSemaphore = new Sema(3);

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
