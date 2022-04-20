import threading
import os
from threading import Thread


def foo():
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread name: {threading.currentThread().getName()}")


if __name__ == "__main__":

    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.currentThread().ident}")
    print(f"Main thread name: {threading.currentThread().getName()}")

    thread = Thread(name="Example_thread", target=foo)
    thread.start()

    thread.join()
