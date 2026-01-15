/*
This script demonstrates a simple distributed computing pattern using Node.js's
built-in 'net' module. It shows how a task server can dispatch tasks to workers
and how clients can receive results. 

Run this script with one of the following roles:
  - node 14_distributed_computing.js server   (starts the task server)
  - node 14_distributed_computing.js worker   (starts a worker that processes tasks)
  - node 14_distributed_computing.js client   (starts a client that receives results)
*/

const net = require("net");
const readline = require("readline");

class TaskServer {
  constructor(host, port) {
    this.host = host;
    this.port = port;
  }

  dispatchTasks() {
    const server = net.createServer((socket) => {
      console.log("Client connected");

      const intervalId = setInterval(() => {
        const task = Math.floor(Math.random() * 100) + 1;
        console.log(`Dispatching task: ${task}`);
        socket.write(`task ${task}\n`);
      }, 1000);

      socket.on("close", () => {
        clearInterval(intervalId);
        console.log("Client disconnected");
      });

      socket.on("error", (err) => {
        clearInterval(intervalId);
        console.error("Socket error:", err.message);
      });
    });

    server.listen(this.port, this.host);
    console.log(`Task server listening on ${this.host}:${this.port}`);
  }
}

class Worker {
  async performTask(host, port) {
    const client = net.createConnection({ host, port }, () => {
      console.log("Connected to server");
      const rl = readline.createInterface({ input: client });

      rl.on("line", async (line) => {
        const parts = line.split(" ");
        if (parts.length >= 2) {
          const task = parseInt(parts[1], 10);
          console.log(`Performing task: ${task}`);
          await sleep(Math.random() * 1500 + 500);
          const result = task * 2;
          console.log(`Completed task: ${task}, result: ${result}`);
        }
      });
    });

    client.on("error", (err) => {
      console.error("Connection error:", err.message);
    });
  }
}

class Client {
  async receiveResults(host, port) {
    const client = net.createConnection({ host, port }, () => {
      console.log("Connected to server");
      const rl = readline.createInterface({ input: client });

      rl.on("line", (line) => {
        const parts = line.split(" ");
        if (parts.length >= 2) {
          const result = parseInt(parts[1], 10);
          console.log(`Client received result: ${result}`);
        }
      });
    });

    client.on("error", (err) => {
      console.error("Connection error:", err.message);
    });
  }
}

async function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

function printUsage() {
  console.log("Usage: node 14_distributed_computing.js <role>");
  console.log("Roles: server, worker, client");
  process.exit(1);
}

async function main() {
  const role = process.argv[2];
  const host = "localhost";
  const port = 8888;

  if (!role || !["server", "worker", "client"].includes(role)) {
    printUsage();
  }

  if (role === "server") {
    const taskServer = new TaskServer(host, port);
    taskServer.dispatchTasks();
  } else if (role === "worker") {
    const worker = new Worker();
    await worker.performTask(host, port);
  } else if (role === "client") {
    const client = new Client();
    await client.receiveResults(host, port);
  }
}

main();
