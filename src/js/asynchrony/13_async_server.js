/*
This example demonstrates how to create a simple HTTP server using Node.js
and its built-in 'http' module. The server listens for incoming requests and
asynchronously sends a response. This example shows the basics of creating
an async server and handling incoming requests.
*/

const http = require("http");

const server = http.createServer((req, res) => {
  console.log(`Received request: ${req.method} ${req.url}`);

  // Set response headers
  res.setHeader("Content-Type", "text/plain");

  // Asynchronously wait before sending a response
  setTimeout(() => {
    res.statusCode = 200;
    res.end("Hello, World!\n");
  }, 1000);
});

const port = 3000;
server.listen(port, () => {
  console.log(`Server listening on port ${port}`);
});
