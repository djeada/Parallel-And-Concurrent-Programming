"""
Async Parallel URL Fetching

This script demonstrates fetching multiple URLs concurrently using asyncio.
It compares synchronous (sequential) and asynchronous (parallel) approaches
to show the performance benefits of async I/O.

Key Concepts:
- asyncio.gather(): Run multiple coroutines concurrently
- asyncio.create_task(): Schedule coroutines for concurrent execution
- Async I/O reduces total time from sum(times) to max(times)

Performance:
- Synchronous: Each request waits for the previous one to complete
- Asynchronous: All requests run concurrently, total time ≈ slowest request

Note: This example uses the standard library http.client for simplicity.
For production async HTTP, consider aiohttp or httpx libraries which
provide true async I/O without the need for locks.

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
    conn.request("GET", url_parsed.path or "/")
    response = conn.getresponse()
    content = response.read()
    print(f"Synchronously fetched content from {url}")
    return content.decode("utf-8")


def fetch_all_sync(urls):
    """Fetch all URLs synchronously (sequential)."""
    return [fetch_sync(url) for url in urls]


async def fetch_async(url):
    """
    Fetch URL asynchronously.

    Note: http.client is blocking, so we use a Lock to prevent interleaving.
    For true async HTTP, use aiohttp or httpx.
    """
    url_parsed = urlparse(url)
    conn = http.client.HTTPSConnection(url_parsed.netloc, timeout=10)

    async with asyncio.Lock():
        conn.request("GET", url_parsed.path or "/")
        response = conn.getresponse()
        content = response.read()
        print(f"Async fetched content from {url}")
        return content.decode("utf-8")


async def fetch_all_async(urls):
    """Fetch all URLs asynchronously (concurrent)."""
    tasks = [asyncio.create_task(fetch_async(url)) for url in urls]
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
