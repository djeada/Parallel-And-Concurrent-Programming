/*
This program demonstrates the difference between synchronous and asynchronous
approaches to handling I/O-bound tasks using Node.js. The synchronous version
runs the tasks sequentially using blocking functions, while the asynchronous
version takes advantage of Promises and async/await to run the tasks concurrently.
This allows for better resource utilization and improved performance when dealing
with I/O-bound tasks.

The program contains two functions, syncMain() and asyncMain(), which
demonstrate the synchronous and asynchronous approaches, respectively. The
execution time for each approach is measured and displayed to show the performance
difference.
*/

const { promisify } = require('util');
const sleep = promisify(setTimeout);

function syncTask(taskId, sleepTime) {
  console.log(`Task ${taskId} started.`);
  const start = Date.now();
  while (Date.now() - start < sleepTime * 1000);
  console.log(`Task ${taskId} finished.`);
}

function syncMain() {
  const startTime = Date.now();

  syncTask(1, 2);
  syncTask(2, 1);

  const elapsedTime = (Date.now() - startTime) / 1000;
  console.log(`Synchronous execution took ${elapsedTime} seconds.`);
}

async function asyncTask(taskId, sleepTime) {
  console.log(`Task ${taskId} started.`);
  await sleep(sleepTime * 1000);
  console.log(`Task ${taskId} finished.`);
}

async function asyncMain() {
  const startTime = Date.now();

  const task1 = asyncTask(1, 2);
  const task2 = asyncTask(2, 1);
  await Promise.all([task1, task2]);

  const elapsedTime = (Date.now() - startTime) / 1000;
  console.log(`Asynchronous execution took ${elapsedTime} seconds.`);
}

syncMain();
asyncMain();
