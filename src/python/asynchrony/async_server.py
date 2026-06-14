"""
Async HTTP Server Example

This script demonstrates building an asynchronous HTTP server using
asyncio's streams API. It handles multiple connections concurrently
within a single thread.

Key Concepts:
- asyncio.start_server(): Creates an async TCP server
- StreamReader/StreamWriter: Async streams for I/O
- Each connection is handled by a separate coroutine
- No threads needed for concurrent connections

Features:
- Handles HTTP GET requests
- Responds with a simple text message
- Gracefully handles connection lifecycle
- Demonstrates async request parsing
- Uses Content-Length and Connection: close so clients know when the response ends

Use Cases:
- Lightweight HTTP servers
- WebSocket servers
- Custom protocol servers
- High-concurrency network services

Note: For production use, consider frameworks like aiohttp, FastAPI,
or Starlette which provide more complete HTTP handling.

Run the server and test with: curl http://localhost:8080
Press Ctrl+C to stop the server.
"""

import asyncio
from http import HTTPStatus


class AsyncHTTPRequestHandler:
    """Simple async HTTP request handler."""

    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.headers = []

    async def handle_request(self):
        """Parse and handle an incoming HTTP request."""
        request_line = await asyncio.wait_for(self.reader.readline(), timeout=5)
        if not request_line:
            return

        try:
            method, path, version = request_line.rstrip().decode("latin1").split(" ")
        except ValueError:
            self.send_error(HTTPStatus.BAD_REQUEST)
            await self.writer.drain()
            return

        if method == "GET":
            await self.do_GET()
        else:
            self.send_error(HTTPStatus.NOT_IMPLEMENTED)

        await self.writer.drain()

    async def do_GET(self):
        """Handle GET requests."""
        body = b"Hello, this is an async server!"
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "text/plain")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Connection", "close")
        self.end_headers()
        self.writer.write(body)
        await self.writer.drain()

    def send_response(self, code, message=None):
        """Send HTTP response line."""
        phrase = message or HTTPStatus(code).phrase
        self.writer.write(f"HTTP/1.1 {code} {phrase}\r\n".encode("latin1"))

    def send_header(self, keyword, value):
        """Add a header to be sent."""
        self.headers.append(f"{keyword}: {value}\r\n".encode("latin1"))

    def end_headers(self):
        """Finish sending headers."""
        self.writer.write(b"".join(self.headers))
        self.writer.write(b"\r\n")

    def send_error(self, code):
        """Send an error response."""
        body = f"{code} {HTTPStatus(code).phrase}\n".encode("latin1")
        self.send_response(code)
        self.send_header("Content-Type", "text/plain")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Connection", "close")
        self.end_headers()
        self.writer.write(body)


async def serve(reader, writer):
    """Handle a single client connection."""
    handler = AsyncHTTPRequestHandler(reader, writer)
    try:
        await handler.handle_request()
    except asyncio.TimeoutError:
        handler.send_error(HTTPStatus.REQUEST_TIMEOUT)
        await writer.drain()
    finally:
        writer.close()
        await writer.wait_closed()


async def main():
    """Start the async HTTP server."""
    server = await asyncio.start_server(serve, "localhost", 8080)
    print("Server started on http://localhost:8080")
    print("Press Ctrl+C to stop")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped")
