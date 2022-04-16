import multiprocessing
import time


class Producer(multiprocessing.Process):
    def __init__(self, queue, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.queue = queue

    def run(self):
        for i in range(5):
            self.queue.put(i)
            print(f"The producer put {i} to the queue")
            time.sleep(1)


class Consumer(multiprocessing.Process):
    def __init__(self, queue, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.queue = queue

    def run(self):
        while True:
            print("Consumer is going to examine the queue every 1 second")
            time.sleep(1)
            try:
                value = self.queue.get(False)
                print(f"The consumer got {value} from the queue")
            except:
                print("The queue is empty")


if __name__ == "__main__":
    queue = multiprocessing.Queue()
    producer = Producer(queue)
    consumer = Consumer(queue)

    for process in (producer, consumer):
        process.start()

    print("The child processes are given 10 seconds to finish their work")
    time.sleep(10)

    for process in (producer, consumer):
        if process.is_alive():
            process.terminate()

    print("All processes are done")
