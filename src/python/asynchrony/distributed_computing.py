"""
Distributed Computing with Asyncio

This script demonstrates a simple distributed computing pattern using asyncio.
A server dispatches tasks to connected workers, which process them concurrently.

Key Concepts:
- asyncio.start_server(): Create async TCP servers
- asyncio.open_connection(): Connect workers to the server
- StreamReader/StreamWriter: Async network I/O
- Role-based architecture (server and worker)

Architecture:
1. Server: Dispatches tasks to connected workers
2. Workers: Receive tasks, process them, return results

Why Asyncio for Distributed Computing:
- Handles many connections efficiently in a single thread
- Low overhead compared to thread-per-connection
- Natural fit for I/O-bound network operations
- Event-driven architecture scales well

Usage:
    python distributed_computing.py server   # Start the task server
    python distributed_computing.py worker   # Connect a worker (in another terminal)

Note: This is an educational example. Production systems should use
frameworks like Celery, Dask, or Ray for distributed computing.
The protocol is line-based: server sends "task N", worker replies "result M".
"""

import argparse
import asyncio
import random


class TaskServer:
    """Server that dispatches tasks to connected workers."""

    def __init__(self, host, port):
        self.host = host
        self.port = port

    async def dispatch_tasks(self):
        """Start server and dispatch tasks to workers."""
        server = await asyncio.start_server(self.handle_worker, self.host, self.port)
        print(f"Task server started on {self.host}:{self.port}")

        async with server:
            await server.serve_forever()

    async def handle_worker(self, reader, writer):
        """Handle a connected worker by sending tasks and reading results."""
        addr = writer.get_extra_info("peername")
        print(f"Worker connected from {addr}")

        try:
            while True:
                task = random.randint(1, 100)
                print(f"Dispatching task: {task}")
                writer.write(f"task {task}\n".encode())
                await writer.drain()

                data = await asyncio.wait_for(reader.readline(), timeout=5)
                if not data:
                    break
                message = data.decode().strip()
                if not message.startswith("result "):
                    print(f"Unexpected worker response from {addr}: {message}")
                    break
                result = int(message.split()[1])
                print(f"Worker {addr} returned result: {result}")
                await asyncio.sleep(1)
        except (ConnectionResetError, asyncio.TimeoutError):
            print(f"Worker {addr} disconnected")
        finally:
            writer.close()
            await writer.wait_closed()


class Worker:
    """Worker that connects to server and processes tasks."""

    async def perform_task(self, host, port):
        """Connect to server and process tasks."""
        reader, writer = await asyncio.open_connection(host, port)
        print(f"Connected to server at {host}:{port}")

        try:
            while True:
                data = await reader.readline()
                if not data:
                    break
                message = data.decode().strip()
                task = int(message.split()[1])
                print(f"Performing task: {task}")
                await asyncio.sleep(random.uniform(0.5, 2))
                result = task * 2
                writer.write(f"result {result}\n".encode())
                await writer.drain()
                print(f"Completed task: {task}, sent result: {result}")
        except ConnectionResetError:
            print("Server disconnected")
        finally:
            writer.close()
            await writer.wait_closed()


async def main(role):
    """Run the specified role."""
    host = "localhost"
    port = 8888

    if role == "server":
        task_server = TaskServer(host, port)
        await task_server.dispatch_tasks()
    elif role == "worker":
        worker = Worker()
        await worker.perform_task(host, port)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Distributed computing example")
    parser.add_argument(
        "role",
        choices=["server", "worker"],
        help="Role to run: server or worker",
    )
    args = parser.parse_args()

    try:
        asyncio.run(main(args.role))
    except KeyboardInterrupt:
        print("\nStopped")
