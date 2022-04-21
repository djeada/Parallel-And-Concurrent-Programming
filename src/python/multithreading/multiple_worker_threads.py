import time
from threading import Thread


def foo(id):
    print("Function foo() starts, Thread id: ", id)
    time.sleep(1)
    print("Function foo() finishes, Thread id: ", id)


def bar(id):
    print("Function bar() starts, Thread id: ", id)
    time.sleep(0.1)
    print("Function bar() finishes, Thread id: ", id)


if __name__ == "__main__":
    print("Main thread starts")

    threads = []
    for i in range(5):
        threads.append(Thread(target=foo, args=(i,)))

    for i in range(5):
        threads.append(Thread(target=bar, args=(i,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("Main thread finishes")
