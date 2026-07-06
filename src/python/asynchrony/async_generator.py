"""
Async Generator Example: Streaming paginated API results

Problem:
Imagine an API returns data page by page. Each page takes time to fetch.

Without an async generator:
- We fetch every page first
- We store everything in memory
- The caller waits until all data is downloaded before processing starts

With an async generator:
- We fetch one page at a time
- We yield items as soon as they arrive
- The caller can start processing immediately
- We do not need to keep the full dataset in memory

This is useful for:
- Paginated APIs
- Database cursors
- WebSocket messages
- Log/event streams
- Large datasets
"""

import asyncio


# Simulated remote API pages
PAGES = {
    1: ["user-1", "user-2", "user-3"],
    2: ["user-4", "user-5", "user-6"],
    3: ["user-7", "user-8"],
}


async def fetch_page(page_number: int) -> list[str]:
    """
    Simulate fetching one page from a slow remote API.
    """
    print(f"Fetching page {page_number}...")
    await asyncio.sleep(1)

    return PAGES.get(page_number, [])


async def fetch_all_users() -> list[str]:
    """
    Traditional approach.

    This waits until all pages are fetched before returning anything.
    """
    users = []
    page_number = 1

    while True:
        page = await fetch_page(page_number)

        if not page:
            break

        users.extend(page)
        page_number += 1

    return users


async def stream_users():
    """
    Async generator approach.

    This yields users as soon as each page arrives.
    The caller does not need to wait for all pages first.
    """
    page_number = 1

    while True:
        page = await fetch_page(page_number)

        if not page:
            break

        for user in page:
            yield user

        page_number += 1


async def process_user(user: str) -> None:
    """
    Simulate async processing of one user.
    """
    await asyncio.sleep(0.2)
    print(f"Processed {user}")


async def demo_without_async_generator():
    print("\n--- Without async generator ---")

    users = await fetch_all_users()

    print("All users downloaded. Now processing...")

    for user in users:
        await process_user(user)


async def demo_with_async_generator():
    print("\n--- With async generator ---")

    async for user in stream_users():
        await process_user(user)


async def main():
    await demo_without_async_generator()
    await demo_with_async_generator()


if __name__ == "__main__":
    asyncio.run(main())
