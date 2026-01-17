/**
 * Simple Mutex
 *
 * This example demonstrates the basic use of std::mutex to protect
 * shared data from concurrent access.
 *
 * Key concepts:
 * - mutex.lock() acquires exclusive access
 * - mutex.unlock() releases the mutex
 * - Only one thread can hold the mutex at a time
 * - Other threads block at lock() until it's available
 *
 * Note: Prefer std::scoped_lock or std::lock_guard for automatic
 * unlock via RAII (see race_condition.cpp for example).
 */

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

unsigned int task_count = 0;
std::mutex counter_mutex;

void worker() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread " << std::this_thread::get_id()
                  << " is planning...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Protect the critical section
        counter_mutex.lock();
        ++task_count;
        counter_mutex.unlock();
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);

    alice.join();
    bob.join();

    std::cout << "Completed " << task_count << " tasks.\n";
    return 0;
}
