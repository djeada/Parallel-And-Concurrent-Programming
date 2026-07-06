"""
Running CPU-Bound Work Without Freezing the Asyncio Event Loop

Problem:
Asyncio is great when tasks spend time waiting for I/O.

But CPU-bound work is different.

CPU-bound work means Python is actively using the CPU:
- image processing
- video encoding
- data compression
- parsing huge files
- number crunching
- machine learning preprocessing
- cryptographic hashing
- large calculations

If you run CPU-heavy code directly inside an async function, it blocks the
event loop. While the CPU-bound function is running, no other coroutine can run.

Solution:
Use ProcessPoolExecutor to run CPU-bound functions in separate processes.

Why processes?
- asyncio normally runs on one thread
- CPU-heavy Python code holds the GIL
- threads usually do not help much for pure Python CPU-heavy work
- separate processes can run on separate CPU cores

Important distinction:
- Use asyncio directly for async I/O
- Use asyncio.to_thread() for blocking I/O
- Use ProcessPoolExecutor for CPU-bound work

This script shows:
1. What goes wrong when CPU-bound work runs directly in the event loop
2. How ProcessPoolExecutor keeps the event loop responsive
3. That CPU work runs in separate processes
4. That the asyncio heartbeat keeps ticking while CPU work happens elsewhere
"""

import asyncio
import concurrent.futures
import os
import threading
import time


START_TIME = time.perf_counter()


def log(message: str) -> None:
    """
    Print process ID, thread ID, and elapsed time.

    This helps us see where code is running.

    In the asyncio event loop:
    - process ID stays the same
    - thread is usually MainThread

    In ProcessPoolExecutor:
    - process ID is different
    - each worker runs in a separate Python process
    """
    elapsed = time.perf_counter() - START_TIME
    process_id = os.getpid()
    thread_id = threading.get_ident()
    thread_name = threading.current_thread().name

    print(
        f"{elapsed:6.2f}s | "
        f"pid={process_id} | "
        f"thread={thread_name}:{thread_id} | "
        f"{message}",
        flush=True,
    )


def cpu_bound_function(task_id: int) -> tuple[int, int]:
    """
    CPU-bound function.

    This function intentionally does heavy arithmetic.

    Important:
    This is a normal synchronous function, not an async function.

    It should not run directly inside the event loop.
    Instead, we send it to ProcessPoolExecutor.
    """
    log(f"CPU task {task_id} started")

    total = 0

    for i in range(30_000_000):
        total += i * i

    log(f"CPU task {task_id} finished")

    return task_id, total


async def heartbeat(name: str, seconds: int = 8) -> None:
    """
    Show whether the event loop is still responsive.

    If the event loop is healthy, this prints regularly.

    If CPU-bound code blocks the event loop, the heartbeat freezes.
    """
    for i in range(seconds * 2):
        log(f"{name}: heartbeat {i}")
        await asyncio.sleep(0.5)


async def bad_run_cpu_work_directly() -> None:
    """
    BAD EXAMPLE:

    This runs CPU-bound work directly inside the async event loop.

    Even though this function is async, the CPU-bound function itself does not
    await anything. It keeps the CPU busy.

    Result:
    The heartbeat cannot run while this CPU work is happening.
    The whole event loop is frozen.
    """
    log("BAD demo started")

    # This blocks the event loop.
    #
    # While this function is running:
    # - no heartbeat
    # - no network handling
    # - no timers
    # - no other coroutines
    # - no responsiveness
    result = cpu_bound_function(task_id=1)

    log(f"BAD demo result: {result}")
    log("BAD demo finished")


async def run_bad_demo() -> None:
    """
    Run the broken version.

    The heartbeat is scheduled at the same time as the CPU work,
    but it will not keep ticking because the event loop gets blocked.
    """
    print("\n--- BAD: CPU-bound work blocks the event loop ---")

    await asyncio.gather(
        bad_run_cpu_work_directly(),
        heartbeat("bad-demo-heartbeat", seconds=5),
    )


async def good_run_cpu_work_in_process_pool() -> None:
    """
    GOOD EXAMPLE:

    Run CPU-bound functions in separate worker processes.

    The asyncio event loop remains free to keep doing async work.
    """
    log("GOOD demo started")

    loop = asyncio.get_running_loop()

    with concurrent.futures.ProcessPoolExecutor(max_workers=3) as executor:
        tasks = []

        for task_id in range(1, 7):
            future = loop.run_in_executor(
                executor,
                cpu_bound_function,
                task_id,
            )

            tasks.append(future)

        log("CPU tasks submitted to process pool")

        results = await asyncio.gather(*tasks)

    log(f"GOOD demo results: {len(results)} results received")
    log("GOOD demo finished")


async def run_good_demo() -> None:
    """
    Run the correct version.

    The heartbeat should continue ticking while CPU-heavy work runs in
    separate processes.
    """
    print("\n--- GOOD: CPU-bound work runs in separate processes ---")

    await asyncio.gather(
        good_run_cpu_work_in_process_pool(),
        heartbeat("good-demo-heartbeat", seconds=8),
    )


async def bad_async_cpu_function() -> None:
    """
    Another common mistake.

    Marking a function as async does not automatically make it non-blocking.

    This function is async, but the loop below contains no await.
    Therefore, once it starts running, it blocks the event loop until done.
    """

    # BAD:
    #
    # total = 0
    #
    # for i in range(100_000_000):
    #     total += i * i
    #
    # return total

    pass


async def to_thread_note() -> None:
    """
    asyncio.to_thread() is useful, but mostly for blocking I/O.

    Example use cases:
    - calling a blocking file API
    - calling a synchronous HTTP library
    - calling input()
    - using a legacy blocking SDK

    For pure Python CPU-bound work, ProcessPoolExecutor is usually better.
    """

    # Good for blocking I/O:
    #
    # result = await asyncio.to_thread(blocking_file_read)
    #
    # Usually not ideal for pure Python CPU-heavy work:
    #
    # result = await asyncio.to_thread(cpu_bound_function, 1)

    pass


async def main() -> None:
    log("Program started")

    # The bad demo is useful for teaching, but it can make the program feel
    # frozen because that is exactly the problem it demonstrates.
    #
    # Uncomment this when you want to show what goes wrong.
    #
    # await run_bad_demo()

    await run_good_demo()

    log("Program finished")


if __name__ == "__main__":
    # Required on Windows and macOS when using ProcessPoolExecutor.
    #
    # Child processes need to import this module safely.
    # Without this guard, new processes may accidentally re-run top-level code.
    asyncio.run(main())
