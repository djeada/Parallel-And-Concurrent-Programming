"""
Parallel Web Fetching Example

Compares sequential and threaded fetching for I/O-bound web requests.

Threading is useful here because network requests spend most of their time
waiting for I/O. While one thread waits, another can run.
"""

from concurrent.futures import ThreadPoolExecutor, as_completed
from time import perf_counter

import requests

URLS = [
    "https://httpbin.org/delay/1",
    "https://httpbin.org/delay/2",
    "https://httpbin.org/delay/3",
    "https://httpbin.org/delay/4",
]

REQUEST_TIMEOUT = 5
MAX_WORKERS = 4


def fetch_url(url: str) -> int | str:
    """Fetch a URL and return its response size in bytes."""
    try:
        response = requests.get(url, timeout=REQUEST_TIMEOUT)
        response.raise_for_status()
        return len(response.content)
    except requests.RequestException as exc:
        return f"ERROR: {exc}"


def fetch_sequential(urls: list[str]) -> dict[str, int | str]:
    """Fetch URLs one at a time."""
    return {url: fetch_url(url) for url in urls}


def fetch_concurrent(urls: list[str]) -> dict[str, int | str]:
    """Fetch URLs concurrently using threads."""
    results = {}

    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        future_to_url = {executor.submit(fetch_url, url): url for url in urls}

        for future in as_completed(future_to_url):
            url = future_to_url[future]
            results[url] = future.result()

    return results


def timed_run(label: str, fetcher, urls: list[str]) -> dict[str, int | str]:
    """Run a fetcher function and print its elapsed time."""
    start = perf_counter()
    results = fetcher(urls)
    elapsed = perf_counter() - start

    print(f"\n{label} results:")
    for url, result in results.items():
        print(f"  {url}: {result}")

    print(f"{label} time taken: {elapsed:.2f} seconds")
    return results


def main() -> None:
    timed_run("Sequential", fetch_sequential, URLS)
    timed_run("Concurrent", fetch_concurrent, URLS)


if __name__ == "__main__":
    main()
