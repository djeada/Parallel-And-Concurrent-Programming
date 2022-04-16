import os
import multiprocessing
import threading


def foo():
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread id: {threading.currentThread().getName()}")


if __name__ == "__main__":

    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.currentThread().ident}")
    print(f"Main thread id: {threading.currentThread().getName()}")

    process = multiprocessing.Process(target=foo)
    process.start()
    process.join()
