const { Semaphore } = require("async-mutex");

async function resourceLimitedOperation(id) {
  console.log(`Worker ${id} is entering the resource-limited operation`);
  await new Promise((resolve) =>
    setTimeout(resolve, Math.random() * 1500 + 500)
  );
  console.log(`Worker ${id} is leaving the resource-limited operation`);
}

async function worker(sem, id) {
  for (let i = 0; i < 3; i++) {
    await sem.runExclusive(async () => {
      await resourceLimitedOperation(id);
    });
    await new Promise((resolve) =>
      setTimeout(resolve, Math.random() * 500 + 500)
    );
  }
  console.log(`End of worker ${id} function`);
}

async function taskGenerator() {
  const sem = new Semaphore(2); // Limit concurrency to 2
  const nWorkers = 5;
  const workers = Array.from({ length: nWorkers }, (_, i) => worker(sem, i));
  await Promise.all(workers);
}

(async () => {
  await taskGenerator();
  console.log("The End");
})();
