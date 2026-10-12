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
  port: Number(process.env.PORT || 8888),
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
        role: null,
        activeTask: null,
      };

      // Dispatch only to registered workers, at most one task per worker.
      clientInfo.intervalId = setInterval(() => {
        if (clientInfo.role !== "worker" || clientInfo.activeTask !== null) return;
        const task = {
          id: ++this.taskId,
          data: Math.floor(Math.random() * 100) + 1,
          type: "compute",
          timestamp: Date.now(),
        };
        
        try {
          clientInfo.activeTask = task.id;
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
          if (clientInfo.role === null) {
            if (result.type !== "register" || !["worker", "client"].includes(result.role)) {
              socket.destroy();
              return;
            }
            clientInfo.role = result.role;
            console.log(`  Registered ${result.role}: ${clientId}`);
            return;
          }
          if (clientInfo.role !== "worker" || clientInfo.activeTask === null ||
              result.taskId !== clientInfo.activeTask) return;
          clientInfo.activeTask = null;
          console.log(`  Received result from ${clientId}:`, result);
          // Result clients subscribe to computations, not incoming tasks.
          for (const info of this.clients.values()) {
            if (info.role === "client" && !info.socket.destroyed) {
              info.socket.write(JSON.stringify(result) + "\n");
            }
          }
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
      console.log(`Listening on ${this.host}:${this.server.address().port}`);
      console.log("Waiting for workers to connect...\n");
    });

    this.server.on("error", (err) => {
      console.error("Server error:", err.message);
      process.exitCode = 1;
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
        this.client.write(JSON.stringify({ type: "register", role: "worker" }) + "\n");
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
    if (!this.client.destroyed) this.client.write(JSON.stringify(result) + "\n");
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
        this.client.write(JSON.stringify({ type: "register", role: "client" }) + "\n");
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
    const shutdown = () => {
      console.log("\nShutting down...");
      cleanup();
    };
    process.once("SIGINT", shutdown);
    process.once("SIGTERM", shutdown);
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

if (require.main === module) main().catch((err) => {
  console.error("Fatal error:", err.message);
  process.exit(1);
});

module.exports = { TaskServer, Worker, Client };
