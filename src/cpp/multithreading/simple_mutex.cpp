/**
 * Modern C++20: Scoped Mutex Ownership
 *
 * This example demonstrates the basic use of std::mutex to protect
 * shared data from concurrent access.
 *
 * Key concepts:
 * - std::scoped_lock acquires the mutex and releases it on scope exit
 * - std::jthread owns each worker and joins on scope exit
 * - Only one thread can hold the mutex at a time
 * - Other threads block at lock() until it's available
 *
 * Compile: g++ -std=c++20 -pthread simple_mutex.cpp
 */

#include <chrono>
#include <iostream>
#include <syncstream>
#include <mutex>
#include <thread>

unsigned int task_count = 0;
std::mutex counter_mutex;

void worker() {
    for (int i = 0; i < 5; ++i) {
        std::osyncstream(std::cout) << "Thread " << std::this_thread::get_id()
                  << " is planning...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Protect the critical section
        std::scoped_lock lock(counter_mutex);
        ++task_count;
    }
}

int main() {
    {
        std::jthread alice(worker);
        std::jthread bob(worker);
    } // Both workers finish before the counter is read.

    std::osyncstream(std::cout) << "Completed " << task_count << " tasks.\n";
    return 0;
}
