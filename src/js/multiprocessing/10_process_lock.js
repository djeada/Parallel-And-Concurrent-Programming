/*
This program demonstrates the use of a mutex (Lock) with process creation using fork() in Node.js.
A mutex (short for "mutual exclusion") is a synchronization primitive that is used to prevent
multiple processes from simultaneously accessing a shared resource, such as a file or
a variable in memory. In this example, we use a file-based lock to protect access
to a shared counter variable, ensuring that only one process can increment the counter at a time.
This prevents race conditions and ensures that the final value of the counter is correct.
*/

const fs = require('fs');
const { fork } = require('child_process');

const counterFile = 'counter.txt';
const lockFile = 'counter.lock';

function lock() {
  while (true) {
    try {
      fs.writeFileSync(lockFile, '');
      break;
    } catch (error) {
      if (error.code !== 'EEXIST') {
        throw error;
      }
      // Wait a moment and try again
      sleep(10);
    }
  }
}

function unlock() {
  fs.unlinkSync(lockFile);
}

function sleep(ms) {
  Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, ms);
}

function worker() {
  lock();

  console.log(`Process ${process.pid} is starting...`);
  sleep(1000); // Simulate some work

  const counter = fs.existsSync(counterFile) ? Number(fs.readFileSync(counterFile)) : 0;
  const newValue = counter + 1;
  fs.writeFileSync(counterFile, newValue);

  console.log(`Process ${process.pid} is finished. Counter: ${newValue}`);
  unlock();
}

if (process.argv[2] === 'worker') {
  worker();
} else {
  const numProcesses = 4;
  const children = [];

  // Initialize the counter file
  fs.writeFileSync(counterFile, '0');

  for (let i = 0; i < numProcesses; i++) {
    children.push(fork(__filename, ['worker']));
  }

  // Wait for all child processes to finish
  Promise.all(children.map(child => new Promise(resolve => child.on('exit', resolve))))
    .then(() => {
      const finalCounter = Number(fs.readFileSync(counterFile));
      console.log(`Final counter value: ${finalCounter}`);

      // Cleanup
      fs.unlinkSync(counterFile);
      if (fs.existsSync(lockFile)) {
        fs.unlinkSync(lockFile);
      }
    });
}
