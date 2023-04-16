const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread) {
  const numThreads = 5;
  const sharedBuffer = new SharedArrayBuffer(12);
  const counterView = new Int32Array(sharedBuffer, 0, 1);
  const mutexView = new Int32Array(sharedBuffer, 4, 1);
  const doneCounterView = new Int32Array(sharedBuffer, 8, 1);
  counterView[0] = 1;
  mutexView[0] = 0;

  const workers = [];

  for (let i = 0; i < numThreads; i++) {
    const worker = new Worker(__filename);
    worker.on("message", (message) => {
      if (message.type === "print") {
        console.log(message.text);
      } else if (message.type === "done") {
        doneCounterView[0]++;
        if (doneCounterView[0] === numThreads) {
          console.log(`The final value of COUNTER is: ${counterView[0]}`);
        }
      }
    });
    worker.on("error", (error) => console.error(error));
    worker.postMessage({ type: "init", id: i, sharedBuffer });
    workers.push(worker);
  }
} else {
  let id;
  let sharedBuffer;
  let counterView;
  let mutexView;

  parentPort.on("message", (message) => {
    if (message.type === "init") {
      id = message.id;
      sharedBuffer = message.sharedBuffer;
      counterView = new Int32Array(sharedBuffer, 0, 1);
      mutexView = new Int32Array(sharedBuffer, 4, 1);

      for (let i = 0; i < 10; i++) {
        while (Atomics.compareExchange(mutexView, 0, 0, 1) === 1) {
          Atomics.wait(mutexView, 0, 1);
        }
        const localCounter = counterView[0];
        const multipliedCounter = localCounter * (id + 1);
        parentPort.postMessage({
          type: "print",
          text: `The COUNTER gets multiplied by ${id + 1}`,
        });
        counterView[0] = multipliedCounter;
        Atomics.store(mutexView, 0, 0);
        Atomics.notify(mutexView, 0, 1);
      }

      parentPort.postMessage({ type: "done" });
    }
  });
}
