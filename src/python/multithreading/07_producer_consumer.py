import threading
import time

pause_event = threading.Event()


def long_function():
    i = 0
    while True:
        if pause_event.is_set():
            pause_event.wait()
        print(f"Executing step {i}")
        i += 1
        time.sleep(1)


def button_handler():
    while True:
        input("Press ENTER to toggle pause/resume:")
        if pause_event.is_set():
            pause_event.clear()
        else:
            pause_event.set()


if __name__ == "__main__":
    thread1 = threading.Thread(target=long_function)
    thread2 = threading.Thread(target=button_handler)

    thread1.start()
    thread2.start()

    thread1.join()
    thread2.join()
