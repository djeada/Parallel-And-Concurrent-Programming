const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  const numWorkers = 10;
  const workers = [];

  const sharedCounter = new SharedArrayBuffer(4);
  const counterView = new Int32Array(sharedCounter);
  counterView[0] = 1;

  const sharedSemaphore = new SharedArrayBuffer(4);
  const semaphoreView = new Int32Array(sharedSemaphore);
  semaphoreView[0] = 1;

  const sharedDoneCounter = new SharedArrayBuffer(4);
  const doneCounterView = new Int32Array(sharedDoneCounter);
  doneCounterView[0] = 0;

  for (let i = 0; i < numWorkers; i++) {
    const worker = new Worker(__filename);
    worker.on("message", (message) => {
      if (message.type === "print") {
        console.log(message.text);
      }
    });
    worker.on("error", (error) => console.error(error));
    worker.postMessage({ type: "init", id: i, sharedCounter, sharedSemaphore, sharedDoneCounter, numWorkers });
    workers.push(worker);
  }
} else {
  let id;
  let sharedCounter;
  let counterView;
  let sharedSemaphore;
  let semaphoreView;
  let sharedDoneCounter;
  let doneCounterView;
  let numWorkers;

  parentPort.on("message", (message) => {
    if (message.type === "init") {
      id = message.id;
      sharedCounter = message.sharedCounter;
      counterView = new Int32Array(sharedCounter);
      sharedSemaphore = message.sharedSemaphore;
      semaphoreView = new Int32Array(sharedSemaphore);
      sharedDoneCounter = message.sharedDoneCounter;
      doneCounterView = new Int32Array(sharedDoneCounter);
      numWorkers = message.numWorkers;

      for (let i = 0; i < 10; i++) {
        setTimeout(() => {
          Atomics.wait(semaphoreView, 0, 0);
          Atomics.sub(semaphoreView, 0, 1);
          const localCounter = counterView[0];
          const newCounter = localCounter * (id + 1);
          parentPort.postMessage({ type: "print", text: `The COUNTER gets multiplied by ${id + 1}` });
          counterView[0] = newCounter;
          Atomics.add(semaphoreView, 0, 1);
          Atomics.notify(semaphoreView, 0, 1);

          if (i === 9) {
            Atomics.add(doneCounterView, 0, 1);
            if (Atomics.load(doneCounterView, 0) === numWorkers) {
              parentPort.postMessage({ type: "print", text: `The final value of COUNTER is: ${counterView[0]}` });
            }
          }
        }, 100 * i);
      }
    }
  });
}
