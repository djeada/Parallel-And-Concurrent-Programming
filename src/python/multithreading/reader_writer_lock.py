"""
Reader-Writer Lock Example

This script demonstrates a Reader-Writer lock pattern for managing concurrent
access to shared data. Multiple readers can access data simultaneously, but
writers require exclusive access.

Key Concepts:
- Multiple readers can read concurrently (improves throughput)
- Writers have exclusive access (no readers or other writers)
- First reader acquires the write lock; last reader releases it
- Writers wait for all readers to finish

Use Cases:
- Caches with frequent reads and rare updates
- Configuration data accessed by many threads
- Database-like systems with read-heavy workloads

Implementation Notes:
- This implements a "readers-preference" lock
- May cause writer starvation under heavy read load
- For production, consider using a fair or writer-preference variant

Alternative: Python 3.13+ includes threading.RWLock. For older versions,
consider the 'readerwriterlock' package from PyPI.
"""

import threading
import time
import random


class ReaderWriterLock:
    """
    A Reader-Writer lock implementation.

    Allows multiple concurrent readers OR a single writer.
    """

    def __init__(self):
        self._data = 0
        self._lock = threading.RLock()
        self._readers = 0
        self._readers_lock = threading.Lock()

    def read(self, reader_id):
        """Acquire read access and return the current data."""
        # First reader acquires the write lock
        with self._readers_lock:
            self._readers += 1
            if self._readers == 1:
                self._lock.acquire()

        print(f"Reader {reader_id} reads data: {self._data}")
        time.sleep(random.uniform(0.5, 1))  # Simulate read time

        # Last reader releases the write lock
        with self._readers_lock:
            self._readers -= 1
            if self._readers == 0:
                self._lock.release()

    def write(self, writer_id):
        """Acquire exclusive write access and modify the data."""
        with self._lock:
            print(f"Writer {writer_id} is writing...")
            self._data += 1
            time.sleep(random.uniform(0.5, 1))  # Simulate write time
            print(f"Writer {writer_id} finished writing. New data: {self._data}")


def reader(rw_lock, reader_id, num_reads=5):
    """Reader thread function."""
    for _ in range(num_reads):
        rw_lock.read(reader_id)
        time.sleep(random.uniform(0.5, 1))


def writer(rw_lock, writer_id, num_writes=3):
    """Writer thread function."""
    for _ in range(num_writes):
        rw_lock.write(writer_id)
        time.sleep(random.uniform(0.5, 1))


def main():
    rw_lock = ReaderWriterLock()

    num_readers = 3
    num_writers = 2

    threads = []

    # Create reader threads
    for i in range(num_readers):
        t = threading.Thread(target=reader, args=(rw_lock, i))
        threads.append(t)

    # Create writer threads
    for i in range(num_writers):
        t = threading.Thread(target=writer, args=(rw_lock, i))
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("All reader/writer threads completed.")


if __name__ == "__main__":
    main()
