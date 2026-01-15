/*
 * Async HTTP Server
 * 
 * This script demonstrates how to create an async HTTP server using Node.js.
 * The server handles multiple concurrent requests asynchronously and shows
 * various async patterns for request handling.
 * 
 * Key concepts:
 * - Async request handlers
 * - Route handling patterns
 * - Simulated async operations (database, external API)
 * - Graceful shutdown
 * - Error handling in async contexts
 */

"use strict";

const http = require("http");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// Simulated async operations
const fetchUserFromDb = async (userId) => {
  await sleep(500); // Simulate DB query
  return {
    id: userId,
    name: `User ${userId}`,
    email: `user${userId}@example.com`,
  };
};

const fetchDataFromExternalApi = async () => {
  await sleep(1000); // Simulate external API call
  return {
    timestamp: new Date().toISOString(),
    data: Array.from({ length: 5 }, (_, i) => ({ id: i, value: Math.random() })),
  };
};

// Route handlers
const routes = {
  "/": async (req, res) => {
    res.writeHead(200, { "Content-Type": "text/plain" });
    res.end("Welcome to the Async Server!\n\nAvailable routes:\n- GET /\n- GET /user/:id\n- GET /data\n- GET /slow\n");
  },

  "/user": async (req, res, userId) => {
    try {
      const user = await fetchUserFromDb(userId || 1);
      res.writeHead(200, { "Content-Type": "application/json" });
      res.end(JSON.stringify(user, null, 2));
    } catch (error) {
      res.writeHead(500, { "Content-Type": "application/json" });
      res.end(JSON.stringify({ error: error.message }));
    }
  },

  "/data": async (req, res) => {
    try {
      const data = await fetchDataFromExternalApi();
      res.writeHead(200, { "Content-Type": "application/json" });
      res.end(JSON.stringify(data, null, 2));
    } catch (error) {
      res.writeHead(500, { "Content-Type": "application/json" });
      res.end(JSON.stringify({ error: error.message }));
    }
  },

  "/slow": async (req, res) => {
    const delayMs = 3000;
    console.log(`  Processing slow request (${delayMs}ms delay)...`);
    await sleep(delayMs);
    res.writeHead(200, { "Content-Type": "text/plain" });
    res.end(`Slow response after ${delayMs}ms\n`);
  },
};

// Request router
const handleRequest = async (req, res) => {
  const url = new URL(req.url, `http://${req.headers.host}`);
  const path = url.pathname;

  console.log(`[${new Date().toISOString()}] ${req.method} ${path}`);

  // Match routes
  if (path === "/" || path === "") {
    await routes["/"]?.(req, res);
  } else if (path.startsWith("/user")) {
    const userId = path.split("/")[2] || "1";
    await routes["/user"]?.(req, res, userId);
  } else if (path === "/data") {
    await routes["/data"]?.(req, res);
  } else if (path === "/slow") {
    await routes["/slow"]?.(req, res);
  } else {
    res.writeHead(404, { "Content-Type": "application/json" });
    res.end(JSON.stringify({ error: "Not Found", path }));
  }
};

const main = () => {
  const PORT = process.env.PORT || 3000;
  const HOST = "127.0.0.1";

  const server = http.createServer(async (req, res) => {
    try {
      await handleRequest(req, res);
    } catch (error) {
      console.error("Request error:", error);
      if (!res.headersSent) {
        res.writeHead(500, { "Content-Type": "application/json" });
        res.end(JSON.stringify({ error: "Internal Server Error" }));
      }
    }
  });

  // Graceful shutdown
  const shutdown = () => {
    console.log("\nShutting down server...");
    server.close(() => {
      console.log("Server closed");
      process.exit(0);
    });
  };

  process.on("SIGINT", shutdown);
  process.on("SIGTERM", shutdown);

  server.listen(PORT, HOST, () => {
    console.log("=== Async HTTP Server ===");
    console.log(`Server running at http://${HOST}:${PORT}/`);
    console.log("\nTest with:");
    console.log(`  curl http://localhost:${PORT}/`);
    console.log(`  curl http://localhost:${PORT}/user/42`);
    console.log(`  curl http://localhost:${PORT}/data`);
    console.log(`  curl http://localhost:${PORT}/slow`);
    console.log("\nPress Ctrl+C to stop\n");
  });
};

main();
