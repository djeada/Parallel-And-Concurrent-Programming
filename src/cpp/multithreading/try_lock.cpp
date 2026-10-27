/**
 * Modern C++20 example: scoped worker ownership with std::jthread.
 *
 * try_lock - Non-Blocking Lock Acquisition
 *
 * This example demonstrates unique_lock::try_lock(), which attempts to acquire
 * a lock without blocking. If the lock is unavailable, it returns false
 * immediately instead of waiting.
 *
 * Key concepts:
 * - try_lock() returns true if lock acquired, false otherwise
 * - Allows threads to do other work if lock is unavailable
 * - Useful for responsive systems or avoiding blocking
 * - Can lead to livelock if not used carefully (see livelock_antipattern.cpp)
 *
 * In this example, workers accumulate items before attempting to write
 * to a shared list, doing useful work while waiting.
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <syncstream>
#include <mutex>
#include <thread>

std::atomic<unsigned int> items_on_list{0};
std::mutex list_mutex;

void worker(const char* name) {
    int items_to_add = 0;

    while (items_on_list.load(std::memory_order_relaxed) <= 20) {
        std::unique_lock lock(list_mutex, std::defer_lock);
        if (items_to_add > 0 && lock.try_lock()) {
            // Got the lock - add accumulated items
            items_on_list.fetch_add(items_to_add, std::memory_order_relaxed);
            std::osyncstream(std::cout) << name << " added " << items_to_add << " item(s).\n";
            items_to_add = 0;

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        } else {
            // Lock unavailable - do other work instead of blocking
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++items_to_add;
            std::osyncstream(std::cout) << name << " found another item.\n";
        }
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();

    std::jthread alice(worker, "Alice");
    std::jthread bob(worker, "Bob");

    alice.join();
    bob.join();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    std::osyncstream(std::cout) << "Final count: " << items_on_list.load() << " items.\n";
    std::osyncstream(std::cout) << "Elapsed time: " << elapsed / 1000.0 << " seconds.\n";

    return 0;
}
