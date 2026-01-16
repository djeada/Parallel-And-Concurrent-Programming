/*
 * Distributed Computing Pattern with TCP Sockets
 * 
 * This script demonstrates a simple distributed computing pattern using Node.js's
 * built-in 'net' module. It shows how to implement:
 * - A task server that dispatches work to workers
 * - Workers that process tasks and return results
 * - Clients that receive computation results
 * 
 * Usage:
 *   node 14_distributed_computing.js server   - Start the task server
 *   node 14_distributed_computing.js worker   - Start a worker
 *   node 14_distributed_computing.js client   - Start a result client
 * 
 * Key concepts:
 * - TCP socket communication
 * - Task distribution patterns
 * - Async message handling
 * - Connection lifecycle management
 */

"use strict";

const net = require("net");
const readline = require("readline");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const CONFIG = {
  host: "127.0.0.1",
  port: 8888,
  taskIntervalMs: 1000,
};

class TaskServer {
  constructor(host, port) {
    this.host = host;
    this.port = port;
    this.clients = new Map();
    this.taskId = 0;
    this.server = null;
  }

  start() {
    this.server = net.createServer((socket) => {
      const clientId = `${socket.remoteAddress}:${socket.remotePort}`;
      console.log(`Client connected: ${clientId}`);

      const clientInfo = {
        socket,
        intervalId: null,
      };

      // Start dispatching tasks to this client
      clientInfo.intervalId = setInterval(() => {
        const task = {
          id: ++this.taskId,
          data: Math.floor(Math.random() * 100) + 1,
          type: "compute",
          timestamp: Date.now(),
        };
        
        try {
          socket.write(JSON.stringify(task) + "\n");
          console.log(`  Dispatched task ${task.id} (data: ${task.data}) to ${clientId}`);
        } catch (err) {
          console.error(`  Error sending to ${clientId}:`, err.message);
        }
      }, CONFIG.taskIntervalMs);

      this.clients.set(clientId, clientInfo);

      // Handle incoming results from workers
      const rl = readline.createInterface({ input: socket });
      rl.on("line", (line) => {
        try {
          const result = JSON.parse(line);
          console.log(`  Received result from ${clientId}:`, result);
        } catch (err) {
          console.log(`  Received non-JSON message from ${clientId}: ${line}`);
          console.log(`  Parse error: ${err.message}`);
        }
      });

      socket.on("close", () => {
        clearInterval(clientInfo.intervalId);
        this.clients.delete(clientId);
        console.log(`Client disconnected: ${clientId}`);
      });

      socket.on("error", (err) => {
        clearInterval(clientInfo.intervalId);
        this.clients.delete(clientId);
        console.error(`Client error (${clientId}):`, err.message);
      });
    });

    this.server.listen(this.port, this.host, () => {
      console.log("=== Task Server ===");
      console.log(`Listening on ${this.host}:${this.port}`);
      console.log("Waiting for workers to connect...\n");
    });

    this.server.on("error", (err) => {
      console.error("Server error:", err.message);
    });
  }

  stop() {
    this.clients.forEach((info) => {
      clearInterval(info.intervalId);
      info.socket.destroy();
    });
    this.server?.close();
    console.log("Server stopped");
  }
}

class Worker {
  constructor(host, port) {
    this.host = host;
    this.port = port;
    this.processedCount = 0;
  }

  async connect() {
    return new Promise((resolve, reject) => {
      this.client = net.createConnection({ host: this.host, port: this.port }, () => {
        console.log("=== Worker ===");
        console.log(`Connected to server at ${this.host}:${this.port}`);
        console.log("Waiting for tasks...\n");
        resolve();
      });

      const rl = readline.createInterface({ input: this.client });

      rl.on("line", async (line) => {
        try {
          const task = JSON.parse(line);
          await this.processTask(task);
        } catch (err) {
          console.error("Error processing task:", err.message);
        }
      });

      this.client.on("error", (err) => {
        console.error("Connection error:", err.message);
        reject(err);
      });

      this.client.on("close", () => {
        console.log("Connection closed");
        console.log(`Total tasks processed: ${this.processedCount}`);
      });
    });
  }

  async processTask(task) {
    console.log(`  Processing task ${task.id} (data: ${task.data})...`);
    
    // Simulate computation
    await sleep(Math.random() * 1000 + 500);
    
    const result = {
      taskId: task.id,
      result: task.data * 2,
      processedAt: Date.now(),
    };
    
    this.processedCount++;
    console.log(`  Completed task ${task.id}: ${task.data} * 2 = ${result.result}`);
    
    // Send result back to server
    this.client.write(JSON.stringify(result) + "\n");
  }
}

class Client {
  constructor(host, port) {
    this.host = host;
    this.port = port;
    this.receivedCount = 0;
  }

  async connect() {
    return new Promise((resolve, reject) => {
      this.client = net.createConnection({ host: this.host, port: this.port }, () => {
        console.log("=== Result Client ===");
        console.log(`Connected to server at ${this.host}:${this.port}`);
        console.log("Receiving tasks/results...\n");
        resolve();
      });

      const rl = readline.createInterface({ input: this.client });

      rl.on("line", (line) => {
        try {
          const data = JSON.parse(line);
          this.receivedCount++;
          console.log(`  [${this.receivedCount}] Received:`, data);
        } catch (err) {
          console.log(`  Received non-JSON message: ${line}`);
          console.log(`  Parse error: ${err.message}`);
        }
      });

      this.client.on("error", (err) => {
        console.error("Connection error:", err.message);
        reject(err);
      });

      this.client.on("close", () => {
        console.log("Connection closed");
        console.log(`Total messages received: ${this.receivedCount}`);
      });
    });
  }
}

const printUsage = () => {
  console.log("Usage: node 14_distributed_computing.js <role>");
  console.log("\nRoles:");
  console.log("  server  - Start the task server");
  console.log("  worker  - Start a worker that processes tasks");
  console.log("  client  - Start a client that receives data");
  process.exit(1);
};

const main = async () => {
  const role = process.argv[2];

  if (!role || !["server", "worker", "client"].includes(role)) {
    printUsage();
  }

  // Graceful shutdown handler
  const setupShutdown = (cleanup) => {
    process.on("SIGINT", () => {
      console.log("\nShutting down...");
      cleanup();
      process.exit(0);
    });
  };

  if (role === "server") {
    const server = new TaskServer(CONFIG.host, CONFIG.port);
    setupShutdown(() => server.stop());
    server.start();
  } else if (role === "worker") {
    const worker = new Worker(CONFIG.host, CONFIG.port);
    setupShutdown(() => worker.client?.destroy());
    await worker.connect();
  } else if (role === "client") {
    const client = new Client(CONFIG.host, CONFIG.port);
    setupShutdown(() => client.client?.destroy());
    await client.connect();
  }
};

main().catch((err) => {
  console.error("Fatal error:", err.message);
  process.exit(1);
});
