"""
Distributed Worker Pool with Asyncio

Problem:
We have many independent jobs to process.

Example:
- resize many images
- crawl many URLs
- process many files
- run many independent calculations
- generate thumbnails
- parse logs

Bad approach:
- process all jobs on one machine, one after another
- total time grows with number of jobs

Distributed approach:
- one server owns the job queue
- multiple workers connect to the server
- each worker asks for work
- the server gives each worker one job at a time
- workers process jobs concurrently
- results are returned to the server

Why asyncio helps here:
- the server can handle many worker connections in one thread
- while one worker is processing a job, the server can talk to other workers
- network reads/writes do not block the whole server

Important:
asyncio does not magically make CPU-heavy work faster inside one Python process.
For real CPU-heavy work, workers should be separate processes or separate machines.
Here, workers are separate programs, so multiple workers can run independently.

Usage:

Terminal 1:
    python distributed_worker_pool.py server

Terminal 2:
    python distributed_worker_pool.py worker worker-1

Terminal 3:
    python distributed_worker_pool.py worker worker-2

Terminal 4:
    python distributed_worker_pool.py worker worker-3

Protocol:
- Worker sends: READY worker-name
- Server sends: JOB job-id duration
- Worker sends: RESULT job-id worker-name result
- Server sends: NO_JOB when all jobs are done
"""

import argparse
import asyncio
import random
import time


HOST = "127.0.0.1"
PORT = 8888


class TaskServer:
    """
    Central coordinator.

    The server owns the job queue.
    Workers connect over TCP and ask for jobs.
    """

    def __init__(self, host: str, port: int):
        self.host = host
        self.port = port

        # Imagine these are expensive jobs:
        # image files to resize, URLs to crawl, log files to parse, etc.
        self.jobs = asyncio.Queue()

        self.results = {}
        self.total_jobs = 12
        self.completed_jobs = 0
        self.started_at = None

    async def load_jobs(self) -> None:
        """
        Load jobs into the central queue.

        Each job has:
        - job_id
        - simulated_duration

        The duration lets us see that workers are doing different amounts of work.
        """
        for job_id in range(1, self.total_jobs + 1):
            duration = random.uniform(1.0, 3.0)
            await self.jobs.put((job_id, duration))

    async def run(self) -> None:
        """
        Start the TCP server.
        """
        await self.load_jobs()
        self.started_at = time.perf_counter()

        server = await asyncio.start_server(
            self.handle_worker,
            self.host,
            self.port,
        )

        print(f"Task server running on {self.host}:{self.port}")
        print(f"Loaded {self.total_jobs} jobs")
        print("Start one or more workers in other terminals.\n")

        async with server:
            await server.serve_forever()

    async def handle_worker(
        self,
        reader: asyncio.StreamReader,
        writer: asyncio.StreamWriter,
    ) -> None:
        """
        Handle one connected worker.

        asyncio.start_server automatically creates a separate coroutine for
        each connected worker, so many workers can be handled concurrently.
        """
        address = writer.get_extra_info("peername")
        print(f"Worker connected from {address}")

        try:
            while True:
                data = await reader.readline()

                if not data:
                    print(f"Worker {address} disconnected")
                    break

                message = data.decode().strip()
                parts = message.split()

                if not parts:
                    continue

                command = parts[0]

                if command == "READY":
                    worker_name = parts[1]
                    await self.send_job_or_stop(worker_name, writer)

                elif command == "RESULT":
                    job_id = int(parts[1])
                    worker_name = parts[2]
                    result = " ".join(parts[3:])

                    self.results[job_id] = result
                    self.completed_jobs += 1

                    print(
                        f"Received result for job {job_id} "
                        f"from {worker_name}: {result}"
                    )

                    if self.completed_jobs == self.total_jobs:
                        elapsed = time.perf_counter() - self.started_at
                        print("\nAll jobs completed.")
                        print(f"Total time: {elapsed:.2f} seconds")
                        print("Results:")
                        for completed_job_id in sorted(self.results):
                            print(
                                f"  job {completed_job_id}: "
                                f"{self.results[completed_job_id]}"
                            )
                        print(
                            "\nThe server will keep running, but new workers "
                            "will receive NO_JOB."
                        )

                else:
                    print(f"Unknown message from {address}: {message}")

        except ConnectionResetError:
            print(f"Worker {address} connection reset")

        finally:
            writer.close()
            await writer.wait_closed()

    async def send_job_or_stop(
        self,
        worker_name: str,
        writer: asyncio.StreamWriter,
    ) -> None:
        """
        Send one job to a worker, or tell it there is no work left.
        """
        if self.jobs.empty():
            writer.write(b"NO_JOB\n")
            await writer.drain()
            print(f"No job available for {worker_name}")
            return

        job_id, duration = await self.jobs.get()

        print(
            f"Dispatching job {job_id} "
            f"to {worker_name} "
            f"(duration={duration:.2f}s)"
        )

        writer.write(f"JOB {job_id} {duration}\n".encode())
        await writer.drain()


class Worker:
    """
    Worker process.

    A worker connects to the server, repeatedly asks for work,
    processes jobs, and sends results back.
    """

    def __init__(self, name: str, host: str, port: int):
        self.name = name
        self.host = host
        self.port = port

    async def run(self) -> None:
        reader, writer = await asyncio.open_connection(self.host, self.port)

        print(f"{self.name}: connected to server")

        try:
            while True:
                # Ask the server for a job.
                writer.write(f"READY {self.name}\n".encode())
                await writer.drain()

                data = await reader.readline()

                if not data:
                    print(f"{self.name}: server disconnected")
                    break

                message = data.decode().strip()
                parts = message.split()

                if parts[0] == "NO_JOB":
                    print(f"{self.name}: no more jobs, exiting")
                    break

                if parts[0] != "JOB":
                    print(f"{self.name}: unexpected message: {message}")
                    break

                job_id = int(parts[1])
                duration = float(parts[2])

                result = await self.process_job(job_id, duration)

                writer.write(
                    f"RESULT {job_id} {self.name} {result}\n".encode()
                )
                await writer.drain()

        finally:
            writer.close()
            await writer.wait_closed()

    async def process_job(self, job_id: int, duration: float) -> str:
        """
        Simulate doing expensive work.

        In real distributed computing, this could be:
        - resizing an image
        - downloading a URL
        - processing a file
        - running a model
        - transforming data
        """
        print(f"{self.name}: processing job {job_id}")

        await asyncio.sleep(duration)

        print(f"{self.name}: finished job {job_id}")

        return f"processed-in-{duration:.2f}s"


async def bad_local_processing_example() -> None:
    """
    BAD APPROACH: process every job locally and sequentially.

    This is commented out from main because it is only here to explain
    what problem the distributed version solves.

    If we have 12 jobs and each job takes around 2 seconds,
    this takes around 24 seconds.

    Adding workers does not help because there are no workers.
    One coroutine is doing all the work one job at a time.
    """

    # jobs = [(job_id, random.uniform(1.0, 3.0)) for job_id in range(1, 13)]
    #
    # start = time.perf_counter()
    #
    # for job_id, duration in jobs:
    #     print(f"Processing job {job_id} locally...")
    #     await asyncio.sleep(duration)
    #     print(f"Finished job {job_id}")
    #
    # elapsed = time.perf_counter() - start
    # print(f"Local sequential processing took {elapsed:.2f} seconds")


async def bad_server_design_example() -> None:
    """
    BAD SERVER DESIGN: wait for one worker to finish everything.

    This is also commented out because it shows a common design mistake.

    The mistake:
    - accept one worker
    - give that worker all jobs
    - only after that worker finishes, accept another worker

    That defeats the point of having multiple workers.

    A useful server must handle many worker connections concurrently.
    asyncio.start_server() helps because every connection is handled by
    its own coroutine.
    """

    # server_socket = ...
    #
    # while True:
    #     reader, writer = await accept_one_worker(server_socket)
    #
    #     # Problem:
    #     # This blocks the server's attention on one worker.
    #     # Other workers cannot be used effectively.
    #     await handle_this_worker_until_all_jobs_are_done(reader, writer)


async def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("role", choices=["server", "worker"])
    parser.add_argument(
        "worker_name",
        nargs="?",
        default="worker-1",
        help="Only used when role is 'worker'",
    )

    args = parser.parse_args()

    if args.role == "server":
        server = TaskServer(HOST, PORT)
        await server.run()

    elif args.role == "worker":
        worker = Worker(args.worker_name, HOST, PORT)
        await worker.run()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nStopped")
