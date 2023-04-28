const { fork } = require('child_process');

const SEMAPHORE_LIMIT = 3;
let semaphore = SEMAPHORE_LIMIT;

function sleep(ms, cb) {
  setTimeout(cb, ms);
}

function worker(id) {
  function tryAcquireSemaphore() {
    if (semaphore <= 0) {
      sleep(100, tryAcquireSemaphore);
    } else {
      semaphore--;

      console.log(`Worker ${id} starts working.`);
      sleep(Math.floor(Math.random() * 1000) + 1000, () => {
        console.log(`Worker ${id} finished working.`);

        semaphore++;
      });
    }
  }

  tryAcquireSemaphore();
}

if (process.argv[2] === 'worker') {
  worker(parseInt(process.argv[3]));
} else {
  const numWorkers = 5;
  for (let i = 0; i < numWorkers; i++) {
    fork(__filename, ['worker', i]);
  }
}
