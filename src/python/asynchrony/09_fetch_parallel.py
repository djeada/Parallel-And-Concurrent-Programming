"""
This script demonstrates how to fetch multiple URLs in parallel using Python's
asyncio and http libraries. The script shows how to create multiple async
tasks, one for each URL to be fetched, and how to run them concurrently using
asyncio.gather(). This approach is beneficial when fetching multiple resources,
as it reduces the total time taken by making requests simultaneously, rather than
sequentially.

The script also includes a synchronous version of the fetch function to compare
the time taken for each approach.
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
    url_parsed = urlparse(url)
    conn = http.client.HTTPSConnection(url_parsed.netloc, timeout=10)
    conn.request("GET", url_parsed.path)
    response = conn.getresponse()
    content = response.read()
    print(f"Synchronously fetched content from {url}")
    return content.decode("utf-8")


def fetch_all_sync(urls):
    return [fetch_sync(url) for url in urls]


async def fetch_async(url):
    url_parsed = urlparse(url)
    conn = http.client.HTTPSConnection(url_parsed.netloc, timeout=10)

    async with asyncio.Lock():
        conn.request("GET", url_parsed.path)
        response = conn.getresponse()
        content = response.read()
        print(f"Async fetched content from {url}")
        return content.decode("utf-8")


async def fetch_all_async(urls):
    tasks = [asyncio.create_task(fetch_async(url)) for url in urls]
    return await asyncio.gather(*tasks)


def main():
    start_time_sync = time.time()
    contents_sync = fetch_all_sync(URLS)
    elapsed_time_sync = time.time() - start_time_sync
    print(f"\nSynchronous execution took {elapsed_time_sync} seconds.\n")

    start_time_async = time.time()
    contents_async = asyncio.run(fetch_all_async(URLS))
    elapsed_time_async = time.time() - start_time_async
    print(f"\nAsynchronous execution took {elapsed_time_async} seconds.\n")


if __name__ == "__main__":
    main()
