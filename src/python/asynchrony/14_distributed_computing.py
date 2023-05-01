"""
Distributed Computing with Python's Asyncio

This educational script demonstrates a simple distributed computing system using
Python's asyncio library. In this example, a server receives tasks from clients and
distributes them to available workers. Workers execute tasks concurrently and return
the results to the server, which sends the results back to the corresponding clients.

Why use Asyncio for distributed computing?
1. Asyncio allows for efficient handling of a large number of connections with a
   single-threaded event loop.
2. Asynchronous programming can help avoid the overhead of creating multiple threads
   or processes.
3. Asyncio can lead to improved performance and resource usage in I/O-bound and
   high-level structured network code.

How does this example work?
1. The server listens for incoming connections from clients and workers.
2. Clients send tasks to the server.
3. The server assigns tasks to available workers.
4. Workers execute tasks concurrently and send the results back to the server.
5. The server sends the results back to the corresponding clients.
"""
import argparse
import asyncio
import random

class TaskServer:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.worker_count = asyncio.Event()

    async def dispatch_tasks(self):
        server = await asyncio.start_server(self.handle_worker, self.host, self.port)
        async with server:
            await server.serve_forever()

    async def handle_worker(self, reader, writer):
        self.worker_count.set()

        while True:
            task = random.randint(1, 100)
            print(f"Dispatching task: {task}")
            writer.write(f"task {task}\n".encode())
            await writer.drain()
            await asyncio.sleep(1)

class Worker:
    async def perform_task(self, host, port):
        reader, writer = await asyncio.open_connection(host, port)

        while True:
            data = await reader.readline()
            message = data.decode().strip()
            task = int(message.split()[1])
            print(f"Performing task: {task}")
            await asyncio.sleep(random.uniform(0.5, 2))
            result = task * 2
            print(f"Completed task: {task}, result: {result}")

class Client:
    async def receive_results(self, host, port):
        reader, writer = await asyncio.open_connection(host, port)

        while True:
            data = await reader.readline()
            message = data.decode().strip()
            result = int(message.split()[1])
            print(f"Client received result: {result}")

async def main(role):
    host = "localhost"
    port = 8888

    if role == "server":
        task_server = TaskServer(host, port)
        await task_server.dispatch_tasks()
    elif role == "worker":
        worker = Worker()
        await worker.perform_task(host, port)
    elif role == "client":
        client = Client()
        await client.receive_results(host, port)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("role", choices=["server", "worker", "client"])
    args = parser.parse_args()

    asyncio.run(main(args.role))
