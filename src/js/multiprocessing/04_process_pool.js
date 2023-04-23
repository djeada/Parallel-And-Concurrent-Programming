/**
 * This script demonstrates the use of the child_process module in Node.js to create
 * and manage a process pool. The process pool efficiently executes multiple tasks using
 * a fixed number of worker processes, allowing for parallelism and taking advantage of
 * multiple CPU cores. The child_process module is used when the tasks are CPU-bound,
 * which can benefit from parallel execution on multiple cores. This is in contrast to
 * thread-based parallelism, which is generally more suited for I/O-bound tasks.
 * In this example, we use the fork function from the child_process module
 * to manage the process pool. We submit tasks to the pool and collect the results as the tasks
 * are completed.
 */

const { fork } = require('child_process');
const os = require('os');

function worker(taskId) {
  return new Promise((resolve, reject) => {
    const workerScript = `
      process.on('message', (message) => {
        const { taskId } = message;
        const randomDuration = (min, max) => Math.random() * (max - min) + min;

        console.log(\`Task \${taskId} is starting...\`);

        setTimeout(() => {
          const result = taskId * 2;
          console.log(\`Task \${taskId} is finished. Result: \${result}\`);
          process.send({ taskId, result });
        }, randomDuration(1000, 3000));
      });
    `;

    const worker = fork('', {
      execArgv: ['-e', workerScript],
    });

    worker.send({ taskId });

    worker.on('message', (result) => {
      resolve(result);
    });

    worker.on('error', reject);
    worker.on('exit', (code) => {
      if (code !== 0) reject(new Error(`Worker stopped with exit code ${code}`));
    });
  });
}

function main() {
  const numTasks = 10;
  const numWorkers = os.cpus().length;
  const tasks = Array.from({ length: numTasks }, (_, i) => i);

  const taskQueue = tasks.slice();
  const runningWorkers = new Set();

  function processNextTask() {
    if (taskQueue.length === 0) {
      if (runningWorkers.size === 0) {
        console.log('All tasks completed.');
      }
      return;
    }

    if (runningWorkers.size >= numWorkers) {
      return;
    }

    const taskId = taskQueue.shift();
    const taskPromise = worker(taskId);

    runningWorkers.add(taskPromise);
    taskPromise
      .then((result) => {
        console.log(`Task ${result.taskId} result collected: ${result.result}`);
      })
      .catch((error) => {
        console.error('Error:', error);
      })
      .finally(() => {
        runningWorkers.delete(taskPromise);
        processNextTask();
      });

    processNextTask();
  }

  processNextTask();
}

main();
