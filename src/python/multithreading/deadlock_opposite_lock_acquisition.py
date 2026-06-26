"""
Deadlock demo: opposite lock acquisition order.

Intent:
- Show how two threads can deadlock when they acquire the same resources
  in different orders.
- worker1 acquires lock_a, then waits for lock_b.
- worker2 acquires lock_b, then waits for lock_a.
- The sleep calls make it likely that both threads acquire their first lock
  before either tries to acquire the second lock.

Expected default behavior:
- Program hangs.
- It does not print "All threads are done".

Fix:
- Make both workers acquire locks in the same order: lock_a first, then lock_b.
- To test the fix, use worker2_fixed instead of worker2.
"""

import threading
import time

lock_a = threading.Lock()
lock_b = threading.Lock()

def worker1():
    print("Starting worker1")

    with lock_a:
        print("worker1 acquired lock_a")
        time.sleep(0.1)

        print("worker1 waiting for lock_b")
        with lock_b:
            print("worker1 finished")

def worker2():
    print("Starting worker2")

    with lock_b:
        print("worker2 acquired lock_b")
        time.sleep(0.1)

        print("worker2 waiting for lock_a")
        with lock_a:
            print("worker2 finished")


# Fixed version: same lock order as worker1.
#
# def worker2_fixed():
#     print("Starting worker2")
#
#     with lock_a:
#         print("worker2 acquired lock_a")
#         time.sleep(0.1)
#
#         print("worker2 waiting for lock_b")
#         with lock_b:
#             print("worker2 finished")


thread_1 = threading.Thread(target=worker1)
thread_2 = threading.Thread(target=worker2)

# To fix it, change this line to:
# thread_2 = threading.Thread(target=worker2_fixed)

thread_1.start()
thread_2.start()

thread_1.join()
thread_2.join()

print("All threads are done")
