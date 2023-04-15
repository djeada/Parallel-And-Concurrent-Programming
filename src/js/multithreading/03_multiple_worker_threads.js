const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

function foo(id) {
  console.log(`Function foo() starts, Thread id: ${id}`);
  setTimeout(() => {
    console.log(`Function foo() finishes, Thread id: ${id}`);
    parentPort.postMessage("done");
  }, 1000);
}

function bar(id) {
  console.log(`Function bar() starts, Thread id: ${id}`);
  setTimeout(() => {
    console.log(`Function bar() finishes, Thread id: ${id}`);
    parentPort.postMessage("done");
  }, 100);
}

function main() {
  console.log("Main thread starts");

  const threads = [];
  for (let i = 0; i < 5; i++) {
    threads.push(
      new Worker(__filename, {
        workerData: { func: foo.toString(), args: [i] },
      })
    );
  }

  for (let i = 0; i < 5; i++) {
    threads.push(
      new Worker(__filename, {
        workerData: { func: bar.toString(), args: [i] },
      })
    );
  }

  let finishedThreads = 0;
  threads.forEach((thread) => {
    thread.on("message", (message) => {
      if (message === "done") {
        finishedThreads++;
        if (finishedThreads === threads.length) {
          console.log("Main thread finishes");
        }
      }
    });
  });
}

if (isMainThread) {
  main();
} else {
  const func = eval(`(${workerData.func})`);
  func(...workerData.args);
}
