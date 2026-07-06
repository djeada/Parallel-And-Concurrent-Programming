"""
Low-Level Asyncio Example: What actually happens when execution switches?

This example shows that asyncio does NOT mean:

    "Python runs multiple functions at the same time."

Instead, asyncio means:

    "One coroutine runs until it reaches await.
     Then it pauses.
     Then the event loop runs another coroutine."

Important ideas:
- There is one main thread.
- Only one coroutine executes Python code at a time.
- A coroutine gives up control only at await points.
- If a coroutine never awaits, it blocks the others.
- async is useful when tasks spend time waiting for I/O.

Run this file and watch:
- the thread ID stays the same
- execution switches only around await
- CPU-heavy code blocks the event loop
"""

import asyncio
import threading
import time


START_TIME = time.perf_counter()


def log(message: str) -> None:
    """
    Print:
    - elapsed time
    - current thread
    - current asyncio task name

    This makes it easier to see what is running.
    """
    elapsed = time.perf_counter() - START_TIME
    thread_id = threading.get_ident()
    thread_name = threading.current_thread().name

    try:
        task = asyncio.current_task()
        task_name = task.get_name() if task else "no-task"
    except RuntimeError:
        task_name = "no-event-loop"

    print(
        f"{elapsed:6.2f}s | "
        f"thread={thread_name}:{thread_id} | "
        f"task={task_name:<12} | "
        f"{message}"
    )


async def fake_network_request(name: str, delay: float) -> str:
    """
    Simulate an async network request.

    In real code, this could be:
    - waiting for an HTTP response
    - waiting for a database query
    - waiting for a WebSocket message
    - waiting for a file operation

    The key point:
    While this coroutine is waiting, the event loop can run another coroutine.
    """

    log(f"{name}: started")

    log(f"{name}: doing a little CPU work before the first await")
    for _ in range(3_000_000):
        pass

    log(f"{name}: reached await asyncio.sleep({delay})")

    # LOW-LEVEL IDEA:
    #
    # This is where the coroutine pauses.
    #
    # It says to the event loop:
    #
    #   "I cannot continue for {delay} seconds.
    #    You may run something else now."
    #
    # The thread is NOT blocked here.
    # The event loop can resume another ready coroutine.
    await asyncio.sleep(delay)

    log(f"{name}: resumed after await")

    log(f"{name}: doing a little CPU work after the await")
    for _ in range(3_000_000):
        pass

    log(f"{name}: finished")

    return f"{name} result"


async def good_async_demo() -> None:
    """
    GOOD ASYNC VERSION:

    Three independent operations are started together.

    They still run on one thread.
    They still execute Python code one at a time.

    But when one coroutine reaches await, another coroutine can continue.
    """

    log("good_async_demo: starting")

    results = await asyncio.gather(
        fake_network_request("request-A", 3),
        fake_network_request("request-B", 2),
        fake_network_request("request-C", 1),
    )

    log(f"good_async_demo: results = {results}")


async def bad_no_await_task(name: str) -> None:
    """
    BAD ASYNC CODE:

    This function is async, but it does not await while doing expensive work.

    That means it does NOT give control back to the event loop.

    While this loop runs, no other asyncio task can make progress.
    """

    log(f"{name}: started CPU-heavy work")

    # This blocks the event loop.
    #
    # Even though this function is async, there is no await inside this loop.
    # Therefore, other coroutines cannot run until this loop finishes.
    total = 0
    for i in range(80_000_000):
        total += i

    log(f"{name}: finished CPU-heavy work, total={total}")


async def small_task(name: str) -> None:
    """
    This task wants to run every second.

    In the bad demo, it will be delayed because another coroutine blocks the
    event loop with CPU work.
    """

    for i in range(3):
        log(f"{name}: tick {i}")
        await asyncio.sleep(1)

    log(f"{name}: finished")


async def bad_cpu_blocking_demo() -> None:
    """
    BAD VERSION:

    This demonstrates an important rule:

        async does not help if your coroutine keeps the CPU busy
        and never reaches an await.

    The small_task should tick every second, but it cannot run while
    bad_no_await_task is using the event-loop thread.
    """

    log("bad_cpu_blocking_demo: starting")

    await asyncio.gather(
        bad_no_await_task("cpu-heavy-task"),
        small_task("small-task"),
    )

    log("bad_cpu_blocking_demo: finished")


def blocking_sync_sleep(name: str, delay: float) -> None:
    """
    Synchronous blocking function.

    This blocks the current thread.
    If called directly inside async code, it blocks the event loop.
    """

    log(f"{name}: starting blocking time.sleep({delay})")

    # BAD INSIDE ASYNC CODE:
    #
    # This blocks the whole event-loop thread.
    # No coroutine can run while this function sleeps.
    time.sleep(delay)

    log(f"{name}: finished blocking time.sleep({delay})")


async def bad_blocking_call_inside_async() -> None:
    """
    BAD VERSION:

    Calling blocking synchronous code directly inside async code blocks
    the entire event loop.
    """

    log("bad_blocking_call_inside_async: starting")

    await asyncio.gather(
        async_wrapper_that_blocks(),
        small_task("small-task"),
    )

    log("bad_blocking_call_inside_async: finished")


async def async_wrapper_that_blocks() -> None:
    """
    This function is async, but it calls blocking_sync_sleep directly.

    That blocks the event loop thread.
    """

    log("async_wrapper_that_blocks: about to call blocking function")

    # BAD:
    # This prevents all other coroutines from running for 3 seconds.
    blocking_sync_sleep("blocking-call", 3)

    log("async_wrapper_that_blocks: returned from blocking function")


async def better_blocking_call_with_to_thread() -> None:
    """
    BETTER VERSION:

    If you must call blocking synchronous code, move it to a helper thread
    with asyncio.to_thread().

    Notice:
    The event loop remains responsive while the blocking function runs
    in another thread.
    """

    log("better_blocking_call_with_to_thread: starting")

    await asyncio.gather(
        asyncio.to_thread(blocking_sync_sleep, "blocking-call-in-thread", 3),
        small_task("small-task"),
    )

    log("better_blocking_call_with_to_thread: finished")


async def main() -> None:
    print("\n--- GOOD ASYNC DEMO: execution switches at await points ---")
    await good_async_demo()

    print("\n--- BAD DEMO: async function without await blocks others ---")
    await bad_cpu_blocking_demo()

    print("\n--- BAD DEMO: blocking sync call inside async code ---")
    await bad_blocking_call_inside_async()

    print("\n--- BETTER DEMO: move blocking sync call to a helper thread ---")
    await better_blocking_call_with_to_thread()


if __name__ == "__main__":
    asyncio.run(main())
