/**
 * Latch Synchronization
 *
 * This example demonstrates a latch - a single-use synchronization barrier.
 * Unlike a barrier, a latch can only be used once and cannot be reset.
 *
 * Key concepts:
 * - Latch blocks threads until its counter reaches zero
 * - count_down() decrements the counter
 * - wait() blocks until the counter reaches zero
 * - Once released, threads can pass through immediately
 * - Useful when a set of tasks must complete before another set begins
 *
 * Note: C++20 provides std::latch, but this shows the underlying mechanism.
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Shared state
unsigned int task_counter = 1;
std::mutex counter_mutex;

// Latch implementation
std::mutex latch_mutex;
std::condition_variable latch_cv;
int latch_count = 10;

void simulate_work() {
    volatile unsigned long x = 0;
    for (unsigned long i = 0; i < 1'000'000; ++i) {
        ++x;
    }
}

// Worker A waits for the latch, then processes
void worker_a(int id) {
    simulate_work();

    // Wait until all Worker B threads have counted down
    {
        std::unique_lock<std::mutex> lock(latch_mutex);
        latch_cv.wait(lock, [] { return latch_count == 0; });
    }

    {
        std::scoped_lock lock(counter_mutex);
        task_counter *= 2;
        std::cout << "Worker A-" << id << " DOUBLED the counter.\n";
    }
}

// Worker B counts down the latch
void worker_b(int id) {
    simulate_work();

    {
        std::scoped_lock lock(counter_mutex);
        task_counter += 3;
        std::cout << "Worker B-" << id << " ADDED 3 to the counter.\n";
    }

    // Count down and possibly release waiting threads
    {
        std::scoped_lock lock(latch_mutex);
        if (--latch_count == 0) {
            latch_cv.notify_all();
        }
    }
}

int main() {
    std::vector<std::thread> workers;
    workers.reserve(10);

    // Launch alternating workers
    for (int i = 0; i < 10; i += 2) {
        workers.emplace_back(worker_a, i);
        workers.emplace_back(worker_b, i + 1);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "Final counter: " << task_counter << "\n";
    return 0;
}
