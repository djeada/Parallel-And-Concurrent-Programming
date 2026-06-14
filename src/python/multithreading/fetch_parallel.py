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
- Always use timeouts for network requests so worker threads cannot hang forever

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
REQUEST_TIMEOUT = 5


def fetch_url(url):
    """Fetch a URL and store the content length in results."""
    try:
        response = requests.get(url, timeout=REQUEST_TIMEOUT)
        response.raise_for_status()
        result = len(response.content)
    except requests.RequestException as exc:
        result = f"ERROR: {exc}"

    with lock:
        results[url] = result
        print(f"URL: {url}, Result: {result}")


def non_concurrent_fetch():
    """Fetch all URLs sequentially for comparison."""
    non_concurrent_results = {}
    start_time = time.time()

    for url in URLS:
        try:
            response = requests.get(url, timeout=REQUEST_TIMEOUT)
            response.raise_for_status()
            result = len(response.content)
        except requests.RequestException as exc:
            result = f"ERROR: {exc}"
        non_concurrent_results[url] = result
        print(f"Non-concurrent - URL: {url}, Result: {result}")

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
