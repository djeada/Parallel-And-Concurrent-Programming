from threading import Thread
import time

COUNTER = 1


def foo(multiplier):
    global COUNTER
    while COUNTER < 10000:
        time.sleep(0.1)
        COUNTER *= multiplier
        print(f"The COUNTER get's multiplied by {multiplier}")


if __name__ == "__main__":
    threads = []
    for i in range(10):
        threads.append(Thread(target=foo, args=(i + 1,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"The final value of COUNTER is: {COUNTER}")
