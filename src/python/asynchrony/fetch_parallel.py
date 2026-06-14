"""
Async Parallel URL Fetching

This script demonstrates fetching multiple URLs concurrently using asyncio.
It compares synchronous fetching with an asyncio program that offloads a
blocking standard-library HTTP client to worker threads.

Key Concepts:
- asyncio.to_thread(): Run blocking I/O without blocking the event loop
- asyncio.gather(): Collect multiple awaitable results
- asyncio.timeout(): Bound each operation so tasks cannot hang forever

Performance:
- Synchronous: Each request waits for the previous one to complete
- Concurrent: Requests overlap, total time is closer to the slowest request

Good Practice:
- http.client is blocking. Calling it directly inside async def would block the
  event loop, which is an anti-pattern.
- This example uses asyncio.to_thread() as a safe bridge for blocking I/O.
- For production async HTTP, prefer aiohttp or httpx.AsyncClient.

URLs used are public APIs that are generally available for testing.
"""

import asyncio
import http.client
from urllib.parse import urlparse
import time

URLS = [
    "https://httpbin.org/get",
    "https://api.github.com",
    "https://www.example.com",
]


def fetch_sync(url):
    """Fetch URL synchronously (blocking)."""
    url_parsed = urlparse(url)
    conn = http.client.HTTPSConnection(url_parsed.netloc, timeout=10)
    try:
        conn.request("GET", url_parsed.path or "/")
        response = conn.getresponse()
        content = response.read()
        print(f"Fetched {len(content)} bytes from {url}")
        return len(content)
    finally:
        conn.close()


def fetch_all_sync(urls):
    """Fetch all URLs synchronously (sequential)."""
    return [fetch_sync(url) for url in urls]


async def fetch_concurrently(url):
    """Fetch a URL without blocking the event loop."""
    try:
        async with asyncio.timeout(15):
            return await asyncio.to_thread(fetch_sync, url)
    except Exception as exc:
        print(f"Failed to fetch {url}: {exc}")
        return None


async def fetch_all_async(urls):
    """Fetch all URLs asynchronously (concurrent)."""
    tasks = [asyncio.create_task(fetch_concurrently(url)) for url in urls]
    return await asyncio.gather(*tasks)


def main():
    """Compare synchronous and asynchronous fetching."""
    # Synchronous approach
    start_time_sync = time.time()
    fetch_all_sync(URLS)
    elapsed_time_sync = time.time() - start_time_sync
    print(f"\nSynchronous execution took {elapsed_time_sync:.2f} seconds.\n")

    # Asynchronous approach
    start_time_async = time.time()
    asyncio.run(fetch_all_async(URLS))
    elapsed_time_async = time.time() - start_time_async
    print(f"\nAsynchronous execution took {elapsed_time_async:.2f} seconds.\n")


if __name__ == "__main__":
    main()
