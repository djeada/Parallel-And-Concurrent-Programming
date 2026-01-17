/**
 * Semaphore
 *
 * This example demonstrates a counting semaphore - a synchronization
 * primitive that controls access to a limited pool of resources.
 *
 * Key concepts:
 * - Semaphore has an internal counter
 * - acquire() decrements counter (blocks if counter is 0)
 * - release() increments counter (wakes one waiting thread)
 * - Unlike mutex, semaphore can be released by a different thread
 * - Useful for limiting concurrent access to a resource pool
 *
 * Note: C++20 provides std::counting_semaphore, but this shows the mechanism.
 */

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class Semaphore {
public:
    explicit Semaphore(unsigned long initial_count)
        : count_(initial_count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }

    void release() {
        {
            std::scoped_lock lock(mutex_);
            ++count_;
        }
        cv_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    unsigned long count_;
};

// Simulate a shared printer with limited access (1 at a time)
Semaphore printer(1);

void office_worker(int id) {
    // Acquire access to the printer
    printer.acquire();

    std::cout << "Worker " << id << " is using the printer...\n";

    // Simulate printing (random 1-3 seconds)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1000, 3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));

    std::cout << "Worker " << id << " finished printing.\n";

    // Release the printer for others
    printer.release();
}

int main() {
    std::vector<std::thread> workers;

    for (int i = 0; i < 5; ++i) {
        workers.emplace_back(office_worker, i);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "All workers finished.\n";
    return 0;
}
