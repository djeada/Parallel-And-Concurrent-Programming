const { fork } = require("child_process");
const { threadId } = require("worker_threads");

function foo() {
  console.log("Worker function");
  console.log(`Worker function process id: ${process.pid}`);
  console.log(`Worker thread id: ${threadId}`);

  console.log("Worker function finished");
}

if (process.argv[2] === "worker") {
  foo();
} else {
  console.log("Main function");
  console.log(`Main function process id: ${process.pid}`);
  console.log(`Main thread id: ${threadId}`);

  const workerProcess = fork(__filename, ["worker"]);
  workerProcess.on("exit", () => {
    console.log("Main function finished");
  });
}
