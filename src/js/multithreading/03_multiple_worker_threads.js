const { Thread, Worker, isMainThread, parentPort, workerData } = require("worker_threads");

function foo(id) {
  console.log("Function foo() starts, Thread id: ", id);
  setTimeout(() => {
    console.log("Function foo() finishes, Thread id: ", id);
  }, 1000);
}

function bar(id) {
  console.log("Function bar() starts, Thread id: ", id);
  setTimeout(() => {
    console.log("Function bar() finishes, Thread id: ", id);
  }, 100);
}

if (isMainThread) {
  console.log("Main thread starts");

  const threads = [];

  for (let i = 0; i < 5; i++) {
    threads.push(new Worker(__filename, { workerData: { target: foo, args: [i] } }));
  }

  for (let i = 0; i < 5; i++) {
    threads.push(new Worker(__filename, { workerData: { target: bar, args: [i] } }));
  }

  let finishedThreads = 0;

  for (const thread of threads) {
    thread.on("exit", () => {
      finishedThreads++;
      if (finishedThreads === threads.length) {
        console.log("Main thread finishes");
      }
    });
  }
} else {
  const { target, args } = workerData;
  target(...args);
}
