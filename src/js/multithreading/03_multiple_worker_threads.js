const { Worker, isMainThread, parentPort, workerData } = require("worker_threads");

function foo(x) {
  console.log(`Function foo() starts, Thread id: ${workerData.threadId}, arg: ${x}`);
  setTimeout(() => {
    console.log(`Function foo() finishes, Thread id: ${workerData.threadId}, arg: ${x}`);
  }, 1000);
}

function bar(x) {
  console.log(`Function bar() starts, Thread id: ${workerData.threadId}, arg: ${x}`);
  setTimeout(() => {
    console.log(`Function bar() finishes, Thread id: ${workerData.threadId}, arg: ${x}`);
  }, 100);
}

if (isMainThread) {
  console.log("Main thread starts");

  const threads = [];

  for (let i = 0; i < 5; i++) {
    threads.push(new Worker(__filename, { workerData: { functionName: "foo", args: [i], threadId: i } }));
  }

  for (let i = 5; i < 10; i++) {
    threads.push(new Worker(__filename, { workerData: { functionName: "bar", args: [i], threadId: i } }));
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
  const { functionName, args } = workerData;

  if (functionName === "foo") {
    foo(...args);
  } else if (functionName === "bar") {
    bar(...args);
  }
}
