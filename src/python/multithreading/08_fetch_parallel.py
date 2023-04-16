"""
This script demonstrates how to use multithreading to fetch web pages in parallel
and compares it with non-concurrent fetching.
Multiple threads are created, each responsible for fetching a specific URL.

Each thread runs a function `fetch_url`, which fetches the content of the URL and
calculates its length. The results are stored in a shared dictionary, which is
protected by a Lock to avoid race conditions.
"""

import requests
from threading import Thread, Lock
import time

urls = [
    "https://www.example.com",
    "https://www.example.org",
    "https://www.example.net",
    "https://www.example.edu",
]

results = {}
lock = Lock()


def fetch_url(url):
    response = requests.get(url)
    content_length = len(response.content)

    with lock:
        results[url] = content_length
        print(f"URL: {url}, Content Length: {content_length}")


def non_concurrent_fetch():
    non_concurrent_results = {}
    start_time = time.time()

    for url in urls:
        response = requests.get(url)
        content_length = len(response.content)
        non_concurrent_results[url] = content_length
        print(f"Non-concurrent - URL: {url}, Content Length: {content_length}")

    print(f"Non-concurrent results: {non_concurrent_results}")
    print(f"Non-concurrent time taken: {time.time() - start_time:.2f} seconds")


if __name__ == "__main__":
    # Non-concurrent fetching
    non_concurrent_fetch()

    # Concurrent fetching
    start_time = time.time()
    threads = []

    for url in urls:
        thread = Thread(target=fetch_url, args=(url,))
        thread.start()
        threads.append(thread)

    for thread in threads:
        thread.join()

    print(f"Concurrent results: {results}")
    print(f"Concurrent time taken: {time.time() - start_time:.2f} seconds")
