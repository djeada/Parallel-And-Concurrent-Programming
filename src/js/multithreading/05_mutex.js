import time
from threading import Thread, Lock

mutex = Lock()


def foo(id):
    mutex.acquire()  # comment out this line of code to test what happens without the mutex
    for _ in range(100):
        print("Thread id:", id)
        time.sleep(0.05)
    mutex.release()  # comment out this line of code to test what happens without the mutex


if __name__ == "__main__":
    print("MAIN FUNCTION STARTS")
    threads = []
    for i in range(4):
        threads.append(Thread(target=foo, args=(i,)))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    print("THE END")
