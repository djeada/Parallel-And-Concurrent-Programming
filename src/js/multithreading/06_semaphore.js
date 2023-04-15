import threading
import time

# For signaling, the semaphore is initialized to 0; for mutual exclusion, the initial value is 1; for multiplexing, the initial value is a positive number greater than 1.
semaphore = threading.Semaphore(0)


def consumer():
    print("The begining of consumer function")
    semaphore.acquire()
    print(f"consumer got notified: the value is {item}")
    print("The end of consumer function")


def producer():
    global item

    print("The begining of producer function")
    time.sleep(1)

    item = 100

    print(f"producer send's the value {item}")
    semaphore.release()
    print("The end of producer function")


if __name__ == "__main__":
    producer_thread = threading.Thread(target=producer)
    consumer_thread = threading.Thread(target=consumer)

    for thread in (producer_thread, consumer_thread):
        thread.start()

    for thread in (producer_thread, consumer_thread):
        thread.join()

    print("The end of main function")
