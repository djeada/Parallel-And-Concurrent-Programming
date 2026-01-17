"""
Parallel Web Fetching Example

This script demonstrates how multithreading can significantly speed up I/O-bound
operations like fetching web pages. It compares sequential vs concurrent fetching
to show the performance benefits.

Key Concepts:
- I/O-bound tasks (network, disk) benefit greatly from threading
- Threads waiting for I/O don't block other threads
- Use Lock to protect shared data structures
- Threading is ideal for network operations despite Python's GIL

Performance:
- Sequential: Total time ≈ sum of all request times
- Concurrent: Total time ≈ max(individual request times)

Note: For CPU-bound tasks, consider multiprocessing instead due to the GIL.
For async I/O, consider asyncio as an alternative approach.
"""

import time
from threading import Thread, Lock

import requests

URLS = [
    "https://www.example.com",
    "https://www.example.org",
    "https://www.example.net",
    "https://www.example.edu",
]

results = {}
lock = Lock()


def fetch_url(url):
    """Fetch a URL and store the content length in results."""
    response = requests.get(url)
    content_length = len(response.content)

    with lock:
        results[url] = content_length
        print(f"URL: {url}, Content Length: {content_length}")


def non_concurrent_fetch():
    """Fetch all URLs sequentially for comparison."""
    non_concurrent_results = {}
    start_time = time.time()

    for url in URLS:
        response = requests.get(url)
        content_length = len(response.content)
        non_concurrent_results[url] = content_length
        print(f"Non-concurrent - URL: {url}, Content Length: {content_length}")

    print(f"Non-concurrent results: {non_concurrent_results}")
    print(f"Non-concurrent time taken: {time.time() - start_time:.2f} seconds")


def main():
    # Non-concurrent fetching
    non_concurrent_fetch()

    # Concurrent fetching
    start_time = time.time()
    threads = []

    for url in URLS:
        thread = Thread(target=fetch_url, args=(url,))
        thread.start()
        threads.append(thread)

    for thread in threads:
        thread.join()

    print(f"Concurrent results: {results}")
    print(f"Concurrent time taken: {time.time() - start_time:.2f} seconds")


if __name__ == "__main__":
    main()
