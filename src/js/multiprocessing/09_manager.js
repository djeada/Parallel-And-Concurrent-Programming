const { spawn } = require("child_process");
const fs = require("fs");

class Manager {
  constructor(numProcesses) {
    this.numProcesses = numProcesses;
    this.processes = [];
  }

  start() {
    for (let i = 0; i < this.numProcesses; i++) {
      this.createWorker(i);
    }
  }

  createWorker(id) {
    const worker = spawn("node", [
      "-e",
      `console.log("Process ${id} is writing to the shared file.");`,
    ]);
    worker.stdout.on("data", (data) => {
      fs.appendFileSync("shared_data.txt", data.toString());
      this.checkCompletion();
    });

    worker.stderr.on("data", (data) => {
      console.error(`Worker ${id} stderr: ${data}`);
    });

    worker.on("exit", (code) => {
      if (code !== 0) {
        console.error(`Worker ${id} exited with code ${code}`);
      } else {
        this.processes.push(worker);
        this.checkCompletion();
      }
    });
  }

  checkCompletion() {
    if (this.processes.length === this.numProcesses) {
      this.printResults();
    }
  }

  printResults() {
    console.log(
      "All processes have completed. The contents of the shared file are:"
    );
    const data = fs.readFileSync("shared_data.txt", "utf-8");
    console.log(data);
  }
}

const numProcesses = 3;
const manager = new Manager(numProcesses);
manager.start();
