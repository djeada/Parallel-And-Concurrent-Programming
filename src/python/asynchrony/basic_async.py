"""
Basic Async Example: Waiting for many I/O operations on one thread

Problem:
Imagine we are building a user dashboard.

To build the dashboard, we need to fetch:

- user profile
- recent orders
- recommendations

These operations are independent and I/O-bound. In real applications, they
could be HTTP requests, database queries, cache lookups, or calls to other
services.

Synchronous version:
- fetch profile
- then fetch orders
- then fetch recommendations
- total time is roughly the sum of all waits

Asynchronous version:
- start all operations
- while one operation is waiting, the event loop runs another one
- total time is roughly the slowest single operation

Important:
This program prints the thread ID to show that asyncio concurrency happens
on one thread. Asyncio is concurrency, not parallel CPU execution.

When async helps:
- network requests
- database queries
- WebSocket connections
- queues
- timers
- many slow I/O operations

When async does NOT help much:
- heavy CPU work
- image processing
- video encoding
- big mathematical calculations

For CPU-bound work, use multiprocessing, native extensions, or worker processes.
"""

import asyncio
import threading
import time


def log(message: str) -> None:
    """
    Print a message with the current thread ID.

    This helps show that the async version is still running on one thread.
    """
    thread_id = threading.get_ident()
    thread_name = threading.current_thread().name
    print(f"[thread={thread_name}:{thread_id}] {message}")


# ---------------------------------------------------------------------
# Synchronous version
# ---------------------------------------------------------------------

def fetch_user_profile_sync(user_id: int) -> dict:
    log("Fetching user profile...")
    time.sleep(1)
    return {"id": user_id, "name": "Alice"}


def fetch_recent_orders_sync(user_id: int) -> list[str]:
    log("Fetching recent orders...")
    time.sleep(2)
    return ["order-1001", "order-1002"]


def fetch_recommendations_sync(user_id: int) -> list[str]:
    log("Fetching recommendations...")
    time.sleep(1.5)
    return ["book", "keyboard", "coffee mug"]


def build_dashboard_sync(user_id: int) -> dict:
    """
    BAD FOR I/O-BOUND WORK:

    Each operation blocks the whole program while it waits.

    While fetch_user_profile_sync() is sleeping, the program cannot start
    fetching orders or recommendations.

    Total time is roughly:

        1.0 + 2.0 + 1.5 = 4.5 seconds
    """

    start = time.perf_counter()

    log("Starting synchronous dashboard build")

    profile = fetch_user_profile_sync(user_id)
    orders = fetch_recent_orders_sync(user_id)
    recommendations = fetch_recommendations_sync(user_id)

    elapsed = time.perf_counter() - start

    log(f"Synchronous dashboard finished in {elapsed:.2f} seconds")

    return {
        "profile": profile,
        "orders": orders,
        "recommendations": recommendations,
    }


# ---------------------------------------------------------------------
# Asynchronous version
# ---------------------------------------------------------------------

async def fetch_user_profile_async(user_id: int) -> dict:
    log("Fetching user profile...")
    await asyncio.sleep(1)
    return {"id": user_id, "name": "Alice"}


async def fetch_recent_orders_async(user_id: int) -> list[str]:
    log("Fetching recent orders...")
    await asyncio.sleep(2)
    return ["order-1001", "order-1002"]


async def fetch_recommendations_async(user_id: int) -> list[str]:
    log("Fetching recommendations...")
    await asyncio.sleep(1.5)
    return ["book", "keyboard", "coffee mug"]


async def build_dashboard_async(user_id: int) -> dict:
    """
    GOOD FOR I/O-BOUND WORK:

    These operations are independent, so we can wait for them concurrently.

    While one coroutine is waiting, the event loop can continue running
    another coroutine.

    Total time is roughly the slowest operation:

        max(1.0, 2.0, 1.5) = 2.0 seconds

    Notice:
    The thread ID printed by each coroutine is still the same thread.
    """

    start = time.perf_counter()

    log("Starting async dashboard build")

    profile, orders, recommendations = await asyncio.gather(
        fetch_user_profile_async(user_id),
        fetch_recent_orders_async(user_id),
        fetch_recommendations_async(user_id),
    )

    elapsed = time.perf_counter() - start

    log(f"Async dashboard finished in {elapsed:.2f} seconds")

    return {
        "profile": profile,
        "orders": orders,
        "recommendations": recommendations,
    }


# ---------------------------------------------------------------------
# Common mistake
# ---------------------------------------------------------------------

async def bad_async_function() -> None:
    """
    BAD ASYNC CODE:

    This function is marked async, but it uses time.sleep().

    time.sleep() blocks the entire thread.
    Since the event loop runs on that same thread, this blocks all other
    async tasks too.

    Uncomment this to demonstrate what NOT to do.
    """

    # log("Bad async function started")
    #
    # time.sleep(2)
    #
    # log("Bad async function finished")

    pass


async def correct_async_function() -> None:
    """
    GOOD ASYNC CODE:

    asyncio.sleep() does not block the event loop.
    It pauses this coroutine and lets other coroutines run.
    """

    log("Correct async function started")

    await asyncio.sleep(2)

    log("Correct async function finished")


async def async_main() -> None:
    await build_dashboard_async(user_id=1)


if __name__ == "__main__":
    log("Program started")

    print("\n--- Synchronous version ---")
    build_dashboard_sync(user_id=1)

    print("\n--- Asynchronous version ---")
    asyncio.run(async_main())

    log("Program finished")
