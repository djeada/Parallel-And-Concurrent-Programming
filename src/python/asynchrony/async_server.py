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

        await self.read_headers()

        if method == "GET":
            await self.do_GET()
        else:
            self.send_error(HTTPStatus.NOT_IMPLEMENTED)
            await self.writer.drain()

    async def read_headers(self):
        """Read and discard HTTP request headers."""
        while True:
            line = await asyncio.wait_for(self.reader.readline(), timeout=5)
            if line in (b"\r\n", b"\n", b""):
                break

    async def do_GET(self):
        """Handle GET requests."""
        print("SOMEONE CALLED THE GET ENDPOINT")

        await asyncio.sleep(2)  # simulate slow async work

        body = b"Hello, this is an async server!\n"

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
        self.writer.write(
            f"HTTP/1.1 {code.value} {phrase}\r\n".encode("latin1")
        )

    def send_header(self, keyword, value):
        """Add a response header."""
        self.headers.append(
            f"{keyword}: {value}\r\n".encode("latin1")
        )

    def end_headers(self):
        """Finish sending headers."""
        self.writer.write(b"".join(self.headers))
        self.writer.write(b"\r\n")

    def send_error(self, code):
        """Send an error response."""
        body = f"{code.value} {code.phrase}\n".encode("latin1")

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
        try:
            handler.send_error(HTTPStatus.REQUEST_TIMEOUT)
            await writer.drain()
        except (ConnectionResetError, BrokenPipeError):
            pass

    except (ConnectionResetError, BrokenPipeError):
        # The client disconnected before the server finished responding.
        # This is normal for real network servers.
        pass

    finally:
        writer.close()
        try:
            await writer.wait_closed()
        except (ConnectionResetError, BrokenPipeError):
            pass


async def main():
    """Start the async HTTP server."""
    server = await asyncio.start_server(serve, "127.0.0.1", 8080)

    print("Server started on http://127.0.0.1:8080")
    print("Press Ctrl+C to stop")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped")
