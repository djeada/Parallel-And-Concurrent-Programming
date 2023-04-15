import time
from threading import Thread


class Scheduler:

    """
    Every n seconds, start executing the given function.
    """

    def __init__(self, n, function, *args, **kwargs):
        def target():
            while self.flag:
                function(*args, **kwargs)
                time.sleep(n)

        self.flag = True
        self.thread = Thread(target=target)
        self.thread.start()

    def stop(self):
        """
        Stop the new function calls from being scheduled.
        If the previous function has not yet completed, it will not be terminated.
        """
        self.flag = False
        self.thread.join()


scheduler = Scheduler(1, print, "It works")

# wait 10 seconds and stop the scheduler
time.sleep(10)
scheduler.stop()
