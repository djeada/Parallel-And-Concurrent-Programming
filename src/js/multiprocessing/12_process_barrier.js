/*
This program demonstrates the use of a barrier with multiprocessing in Node.js.
A barrier is a synchronization primitive that allows multiple processes to
wait for each other at a certain point in their execution. When all processes
reach the barrier, they are allowed to continue executing.

In this example, we create a number of worker processes that perform some work,
and then wait at a barrier. Once all worker processes have reached the barrier,
they are allowed to continue and finish their execution.

Note that barriers can be useful in cases where the processes need to work in
lockstep or to ensure that certain resources are not accessed before all
processes are ready.
*/

const { fork } = require('child_process');
const fs = require('fs');

const numWorkers = 5;
const barrierPath = 'barrier.txt';

function worker(workerId) {
  // Simulate some work
  const sleepTime = Math.floor(Math.random() * 3) + 1;
  setTimeout(() => {
    console.log(`Worker ${workerId} finished initial work and is waiting at the barrier...`);

    // Wait at the barrier
    fs.readFile(barrierPath, (err, data) => {
      if (err) throw err;
      const barrierCount = parseInt(data, 10) - 1;
      fs.writeFile(barrierPath, barrierCount.toString(), (err) => {
        if (err) throw err;
        if (barrierCount === 0) {
          fs.unlink(barrierPath, (err) => {
            if (err) throw err;
          });
        }

        console.log(`Worker ${workerId} passed the barrier and is continuing execution...`);
      });
    });
  }, sleepTime * 1000);
}

if (!fs.existsSync(barrierPath)) {
  // Create a barrier with num_workers as the number of participants
  fs.writeFileSync(barrierPath, numWorkers.toString());
}

if (process.argv.length === 2) {
  // Start worker processes
  for (let i = 0; i < numWorkers; i++) {
    fork(__filename, ['worker', i]);
  }
} else {
  const workerId = parseInt(process.argv[3]);
  worker(workerId);
}
