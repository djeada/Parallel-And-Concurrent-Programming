"""
Deadlock demo: two worker threads join each other.

Intent:
- Show how deadlock can happen even without explicit Lock objects.
- thread_1 runs worker1.
- thread_2 runs worker2.
- worker1 waits for thread_2 to finish.
- worker2 waits for thread_1 to finish.
- Since each thread is waiting for the other, neither can finish.

Expected default behavior:
- Program hangs.
- It does not print "All threads are done".

Fix:
- Worker threads should do their own work and return.
- The main thread should be responsible for calling join().
- To test the fix, use worker1_fixed and worker2_fixed instead.
"""

import threading
import time

thread_1 = thread_2 = None

def worker1():
    print("Starting worker1")
    time.sleep(0.1)

    print("worker1 waiting for worker2")
    thread_2.join()

    print("Finishing worker1")

def worker2():
    print("Starting worker2")
    time.sleep(0.1)

    print("worker2 waiting for worker1")
    thread_1.join()

    print("Finishing worker2")


# Fixed versions: workers do not join each other.
#
# def worker1_fixed():
#     print("Starting worker1")
#     time.sleep(0.1)
#     print("Finishing worker1")
#
# def worker2_fixed():
#     print("Starting worker2")
#     time.sleep(0.1)
#     print("Finishing worker2")


thread_1 = threading.Thread(target=worker1)
thread_2 = threading.Thread(target=worker2)

# To fix it, change the two lines above to:
# thread_1 = threading.Thread(target=worker1_fixed)
# thread_2 = threading.Thread(target=worker2_fixed)

thread_1.start()
thread_2.start()

thread_1.join()
thread_2.join()

print("All threads are done")
