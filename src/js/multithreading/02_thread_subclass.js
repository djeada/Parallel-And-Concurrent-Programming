const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

class MyThread {
  constructor(func, ...args) {
    this.worker = new Worker(__filename, {
      workerData: { func: func.toString(), args: args },
    });
    this.worker.on("message", (message) => {
      console.log(message);
    });
    this.worker.on("exit", () => {
      console.log(`${func.name} finished`);
    });
  }

  join(callback) {
    this.worker.on("exit", callback);
  }
}

function myFunction() {
  console.log("my_function goes to sleep");
  setTimeout(() => {
    console.log("my_function wakes up");
    parentPort.postMessage("my_function started");
  }, 5000);
}

function main() {
  const threadA = new MyThread(myFunction);
  const threadB = new MyThread(() => {
    console.log("it's me, the lambda function");
    parentPort.postMessage("lambda started");
  });

  // Wait for both threads to finish
  let finishedThreads = 0;
  const onThreadFinished = () => {
    finishedThreads++;
    if (finishedThreads === 2) {
      console.log("Main thread finished");
    }
  };

  threadA.join(onThreadFinished);
  threadB.join(onThreadFinished);
}

if (isMainThread) {
  main();
} else {
  const func = eval(`(${workerData.func})`);
  func(...workerData.args);
}
