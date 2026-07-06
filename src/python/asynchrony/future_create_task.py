"""
create_task Example: Fetching independent data concurrently

Problem:
A dashboard needs data from several independent async sources:

- user profile
- recent orders
- recommendations

Each request takes time. If we await them one by one, the total time is the
sum of all waits.

With asyncio.create_task():
- we start multiple operations immediately
- they run concurrently while the event loop switches between them
- we can do other work before awaiting the results
- each Task stores its own result or exception

Important:
A Task is Future-like. You normally do not create Future objects manually
unless you are writing low-level code or adapting callback-based APIs.
"""

import asyncio
import time


async def fetch_user_profile(user_id: int) -> dict:
    print("Fetching user profile...")
    await asyncio.sleep(1)
    return {"id": user_id, "name": "Alice"}


async def fetch_recent_orders(user_id: int) -> list[str]:
    print("Fetching recent orders...")
    await asyncio.sleep(2)
    return ["order-1001", "order-1002"]


async def fetch_recommendations(user_id: int) -> list[str]:
    print("Fetching recommendations...")
    await asyncio.sleep(1.5)
    return ["book", "laptop stand", "coffee mug"]


async def render_static_header() -> str:
    """
    Simulate some other async work we can do while the requests are running.
    """
    print("Rendering static dashboard header...")
    await asyncio.sleep(0.3)
    return "<h1>Dashboard</h1>"


async def build_dashboard_sequentially(user_id: int) -> dict:
    """
    Slow approach.

    Each operation waits for the previous one to finish.
    Total time is roughly:

        1 + 2 + 1.5 + 0.3 = 4.8 seconds
    """
    start = time.perf_counter()

    user = await fetch_user_profile(user_id)
    orders = await fetch_recent_orders(user_id)
    recommendations = await fetch_recommendations(user_id)
    header = await render_static_header()

    elapsed = time.perf_counter() - start

    return {
        "user": user,
        "orders": orders,
        "recommendations": recommendations,
        "header": header,
        "elapsed": elapsed,
    }


async def build_dashboard_concurrently(user_id: int) -> dict:
    """
    Faster approach.

    We start the independent operations immediately.

    While those tasks are waiting on I/O, we can do other work.
    Then we await their results later.

    Total time is roughly the duration of the slowest operation,
    not the sum of all operations.
    """
    start = time.perf_counter()

    user_task = asyncio.create_task(fetch_user_profile(user_id))
    orders_task = asyncio.create_task(fetch_recent_orders(user_id))
    recommendations_task = asyncio.create_task(fetch_recommendations(user_id))

    header = await render_static_header()

    user, orders, recommendations = await asyncio.gather(
        user_task,
        orders_task,
        recommendations_task,
    )

    elapsed = time.perf_counter() - start

    return {
        "user": user,
        "orders": orders,
        "recommendations": recommendations,
        "header": header,
        "elapsed": elapsed,
    }


async def main():
    print("\n--- Sequential version ---")
    sequential_dashboard = await build_dashboard_sequentially(user_id=1)
    print(f"Sequential version took {sequential_dashboard['elapsed']:.2f} seconds")

    print("\n--- Concurrent version with create_task ---")
    concurrent_dashboard = await build_dashboard_concurrently(user_id=1)
    print(f"Concurrent version took {concurrent_dashboard['elapsed']:.2f} seconds")


if __name__ == "__main__":
    asyncio.run(main())
