"""
Async Parallel URL Fetching

This script demonstrates fetching multiple URLs concurrently using asyncio.
It compares synchronous fetching with an asyncio program that offloads a
blocking standard-library HTTP client to worker threads.

Key Concepts:
- asyncio.to_thread(): Run blocking I/O without blocking the event loop
- asyncio.gather(): Collect multiple awaitable results
- asyncio.timeout(): Limit how long a coroutine awaits a request

Performance:
- Synchronous: Each request waits for the previous one to complete
- Concurrent: Requests overlap, total time is closer to the slowest request

Good Practice:
- http.client is blocking. Calling it directly inside async def would block the
  event loop, which is an anti-pattern.
- This example uses asyncio.to_thread() as a safe bridge for blocking I/O.
- Cancelling the await cannot stop a thread already doing I/O. Its socket timeout
  limits individual blocking waits, not the total request time; asyncio.run()
  still waits for executor threads to finish before exiting.
- For production async HTTP, prefer aiohttp or httpx.AsyncClient.

URLs used are public APIs that are generally available for testing.
"""

import asyncio
import http.client
from urllib.parse import urlsplit
import time

URLS = [
    "https://httpbin.org/get",
    "https://api.github.com",
    "https://www.example.com",
]


def fetch_sync(url):
    """Fetch URL synchronously (blocking)."""
    url_parsed = urlsplit(url)
    if url_parsed.scheme not in {"http", "https"} or not url_parsed.hostname:
        raise ValueError("Expected an HTTP or HTTPS URL with a hostname")
    connection_type = (
        http.client.HTTPSConnection if url_parsed.scheme == "https"
        else http.client.HTTPConnection
    )
    conn = connection_type(url_parsed.hostname, port=url_parsed.port, timeout=10)
    target = url_parsed.path or "/"
    if url_parsed.query:
        target += "?" + url_parsed.query
    try:
        conn.request("GET", target, headers={"User-Agent": "concurrency-examples"})
        response = conn.getresponse()
        if not 200 <= response.status < 300:
            raise RuntimeError(f"HTTP {response.status}: {response.reason}")
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
    start_time_sync = time.perf_counter()
    fetch_all_sync(URLS)
    elapsed_time_sync = time.perf_counter() - start_time_sync
    print(f"\nSynchronous execution took {elapsed_time_sync:.2f} seconds.\n")

    # Asynchronous approach
    start_time_async = time.perf_counter()
    asyncio.run(fetch_all_async(URLS))
    elapsed_time_async = time.perf_counter() - start_time_async
    print(f"\nAsynchronous execution took {elapsed_time_async:.2f} seconds.\n")


if __name__ == "__main__":
    main()
