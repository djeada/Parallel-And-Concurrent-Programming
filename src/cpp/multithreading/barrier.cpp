/**
 * Barrier Synchronization
 *
 * This example demonstrates a barrier - a synchronization primitive that
 * blocks threads until a specified number of threads have reached it.
 * All threads then proceed simultaneously.
 *
 * Key concepts:
 * - Barriers synchronize a group of threads at a specific point
 * - All threads must reach the barrier before any can proceed
 * - Useful for phased computations where all threads must complete
 *   one phase before starting the next
 *
 * Note: C++20 provides std::barrier, but this shows the underlying mechanism.
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/**
 * A reusable barrier that blocks threads until count threads arrive.
 * Uses generation counting to support multiple wait cycles.
 */
class Barrier {
public:
    explicit Barrier(std::size_t count)
        : threshold_(count), count_(count), generation_(0) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        auto current_gen = generation_;

        if (--count_ == 0) {
            // Last thread to arrive - reset and release all
            ++generation_;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            // Wait until generation changes (all threads arrived)
            cv_.wait(lock, [this, current_gen] {
                return current_gen != generation_;
            });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    const std::size_t threshold_;
    std::size_t count_;
    std::size_t generation_;
};

// Shared state protected by mutex
unsigned int task_counter = 1;
std::mutex counter_mutex;

// Simulate some CPU work
void simulate_work() {
    volatile unsigned long x = 0;
    for (unsigned long i = 0; i < 1'000'000; ++i) {
        ++x;
    }
}

void worker_a(Barrier& barrier, int id) {
    simulate_work();

    // Wait for ALL threads to reach this point
    barrier.wait();

    // After barrier: all threads proceed together
    {
        std::scoped_lock lock(counter_mutex);
        task_counter *= 2;
        std::cout << "Worker A-" << id << " DOUBLED the counter.\n";
    }
}

void worker_b(Barrier& barrier, int id) {
    simulate_work();

    {
        std::scoped_lock lock(counter_mutex);
        task_counter += 3;
        std::cout << "Worker B-" << id << " ADDED 3 to the counter.\n";
    }

    // Wait for ALL threads to reach this point
    barrier.wait();
}

int main() {
    constexpr int num_threads = 10;
    Barrier sync_point(num_threads);

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    // Launch alternating worker types
    for (int i = 0; i < num_threads; i += 2) {
        workers.emplace_back(worker_a, std::ref(sync_point), i);
        workers.emplace_back(worker_b, std::ref(sync_point), i + 1);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "Final counter value: " << task_counter << "\n";

    return 0;
}
