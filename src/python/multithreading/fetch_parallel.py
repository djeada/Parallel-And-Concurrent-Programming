import urllib.request
import urllib.parse
import urllib.error
import pathlib
import os
import threading
import concurrent.futures
import time
import urllib3
import queue


def read_url(url, queue):
    try:
        data = urllib.request.urlopen(url, None, 15).read()
        print("Fetched %s from %s" % (len(data), url))
        queue.put(data)
    except Exception as e:
        print(e)


def fetch_parallel(urls):
    result = queue.Queue()
    threads = [threading.Thread(target=read_url, args=(url, result)) for url in urls]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    return result


urls = (
    "https://github.com/djeada/Od-C-do-Cpp#L-warto%C5%9Bci-i-r-warto%C5%9Bci",
    "https://github.com/djeada/Od-C-do-Cpp#L-warto%C5%9Bci-i-r-warto%C5%9Bci",
    "https://github.com/djeada/Od-C-do-Cpp#L-warto%C5%9Bci-i-r-warto%C5%9Bci",
)
res = fetch_parallel(urls)
reslist = []
while not res.empty():
    reslist.append(res.get_nowait())  # or get, doesn't matter here
# print (reslist)
