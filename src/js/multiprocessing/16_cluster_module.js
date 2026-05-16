/*
 * Cluster Module
 *
 * Node.js is single-threaded, but the cluster module lets one primary
 * process fork multiple worker processes that all share the same TCP port.
 * The OS (or Node's round-robin scheduler) distributes incoming connections
 * across workers automatically — no manual load balancer needed.
 *
 * This is the canonical way to scale a Node.js HTTP server across CPU cores.
 *
 * Key concepts:
 * - cluster.isPrimary / cluster.isWorker
 * - Shared TCP port: workers bind the same port independently
 * - Worker crash detection and automatic restart
 * - shuttingDown flag prevents restart loops during graceful shutdown
 * - Graceful shutdown: SIGTERM workers, wait for in-flight requests
 */

"use strict";

const cluster = require("cluster");
const http = require("http");
const os = require("os");

const PORT = 3141;
const POOL_SIZE = Math.min(os.availableParallelism?.() ?? os.cpus().length, 4);

// ---------------------------------------------------------------------------
// Worker: HTTP server sharing the port with all other workers
// ---------------------------------------------------------------------------
if (cluster.isWorker) {
  const server = http.createServer((req, res) => {
    res.writeHead(200, { "Content-Type": "text/plain" });
    res.end(`Worker PID ${process.pid} handled this request\n`);
  });

  server.listen(PORT, () => {
    console.log(`  [Worker ${process.pid}] Listening on port ${PORT}`);
  });

  // Graceful shutdown: stop accepting new connections, finish in-flight ones.
  process.on("SIGTERM", () => {
    console.log(`  [Worker ${process.pid}] SIGTERM received — draining connections`);
    server.close(() => {
      console.log(`  [Worker ${process.pid}] Shutdown complete`);
      process.exit(0);
    });
  });

// ---------------------------------------------------------------------------
// Primary: fork workers, handle crash/restart, send test requests, shutdown
// ---------------------------------------------------------------------------
} else {
  let shuttingDown = false;

  console.log("=== Cluster Module Demo ===\n");
  console.log(`Forking ${POOL_SIZE} workers (port ${PORT})...\n`);

  for (let i = 0; i < POOL_SIZE; i++) cluster.fork();

  // Restart crashed workers — but not during graceful shutdown.
  cluster.on("exit", (worker, code, signal) => {
    if (shuttingDown) {
      console.log(`[Primary] Worker ${worker.process.pid} exited (shutdown)`);
    } else {
      console.log(`[Primary] Worker ${worker.process.pid} crashed (code=${code} signal=${signal}) — restarting`);
      cluster.fork();
    }
  });

  // Send NUM_REQUESTS requests to demonstrate load-balancing across workers.
  const sendRequests = (n) => {
    return new Promise((resolve) => {
      let completed = 0;
      for (let i = 0; i < n; i++) {
        http.get(`http://localhost:${PORT}`, (res) => {
          let body = "";
          res.on("data", (chunk) => { body += chunk; });
          res.on("end", () => {
            console.log(`  [Request ${i}] → ${body.trim()}`);
            if (++completed === n) resolve();
          });
        }).on("error", (err) => {
          console.error(`  [Request ${i}] error: ${err.message}`);
          if (++completed === n) resolve();
        });
      }
    });
  };

  // Intentionally kill one worker to demonstrate the restart logic.
  const killOneWorker = () => {
    const [first] = Object.values(cluster.workers);
    if (first) {
      console.log(`\n[Primary] Killing worker ${first.process.pid} to test crash recovery...`);
      first.process.kill("SIGKILL");
    }
  };

  const gracefulShutdown = () => {
    console.log("\n[Primary] Initiating graceful shutdown...");
    shuttingDown = true;
    for (const worker of Object.values(cluster.workers)) {
      worker.process.kill("SIGTERM");
    }
    // Exit after all workers have exited.
    cluster.on("exit", () => {
      if (Object.keys(cluster.workers).length === 0) {
        console.log("[Primary] All workers exited — done.");
        process.exit(0);
      }
    });
  };

  // Sequence: wait for startup → requests → crash demo → more requests → shutdown
  setTimeout(async () => {
    console.log("[Primary] Sending initial requests (observe load balancing)...");
    await sendRequests(POOL_SIZE + 2);

    killOneWorker();
    await new Promise((r) => setTimeout(r, 800)); // let restart happen

    console.log("\n[Primary] Sending more requests (one worker was restarted)...");
    await sendRequests(POOL_SIZE);

    gracefulShutdown();
  }, 1200);
}
