const { fork } = require("child_process");
const { threadId } = require("worker_threads");
const numProcesses = 3; // number of child processes to create

function task(processNum) {
  console.log(`Task ${processNum} started`);
  console.log(`Worker function process id: ${process.pid}`);
  console.log(`Worker function thread id: ${threadId}`);
  // Do some task here
  console.log(`Task ${processNum} finished`);
}

if (process.argv[2] === "worker") {
  const processNum = process.argv[3];
  task(processNum);
} else {
  console.log("Main function");
  console.log(`Main function process id: ${process.pid}`);
  console.log(`Main thread id: ${threadId}`);

  for (let i = 1; i <= numProcesses; i++) {
    const workerProcess = fork(__filename, ["worker", i.toString()]);
    workerProcess.on("exit", () => {
      console.log(`Worker ${i} finished`);
      if (i === numProcesses) {
        console.log("Main function finished");
      }
    });
  }
}
