"""
This script demonstrates the use of the asyncio and http standard libraries to create
an asynchronous HTTP server in Python. The server listens for incoming HTTP
requests and responds with a simple text message.

The example shows how to set up an async server, handle incoming connections, and
respond to HTTP requests using the asyncio event loop.
"""

import asyncio
from http import HTTPStatus
import io


class AsyncHTTPRequestHandler:
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.headers = []

    async def handle_request(self):
        request_line = await self.reader.readline()
        if not request_line:
            return

        method, path, version = request_line.rstrip().decode("latin1").split(" ")

        if method == "GET":
            await self.do_GET()
        else:
            self.send_error(HTTPStatus.NOT_IMPLEMENTED)

        await self.writer.drain()

    async def do_GET(self):
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "text/plain")
        self.end_headers()
        self.writer.write(b"Hello, this is an async server!")
        await self.writer.drain()

    def send_response(self, code, message=None):
        self.writer.write(
            f"HTTP/1.1 {code} {message or HTTPStatus(code).phrase}\r\n".encode("latin1")
        )

    def send_header(self, keyword, value):
        self.headers.append(f"{keyword}: {value}\r\n".encode("latin1"))

    def end_headers(self):
        self.writer.write(b"".join(self.headers))
        self.writer.write(b"\r\n")


async def serve(reader, writer):
    handler = AsyncHTTPRequestHandler(reader, writer)
    await handler.handle_request()
    writer.close()
    await writer.wait_closed()


async def main():
    server = await asyncio.start_server(serve, "localhost", 8080)
    print("Server started on http://localhost:8080")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped")
