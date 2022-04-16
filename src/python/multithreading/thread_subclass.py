import time
from threading import Thread


class MyThread(Thread):
    def __init__(self, function, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.function = function
        self.start()

    def run(self):
        print(f"{self.function.__name__} started")
        self.function()
        print(f"{self.function.__name__} finished")


def my_function():
    print("function goes to sleep")
    time.sleep(5)
    print("function wakes up")


if __name__ == "__main__":
    thread_a = MyThread(my_function)
    thread_b = MyThread(lambda: print("it's me lambda"))

    for thread in (thread_a, thread_b):
        thread.join()

    print("main thread finished")
