/**
 * Modern C++20 example: scoped worker ownership with std::jthread.
 *
 * Data Race Prevention with Scoped Locking
 *
 * This example demonstrates how a mutex prevents data races on shared state.
 * Compare with simple_data_race_antipattern.cpp to see the problem this solves.
 *
 * Key concepts:
 * - std::mutex provides mutual exclusion
 * - std::scoped_lock automatically acquires and releases mutex (RAII)
 * - Only one thread can hold the mutex at a time
 * - Each update is serialized; the order between workers is unspecified
 *
 * Without the mutex, conflicting updates cause undefined behavior.
 * With the mutex, the final value still depends on the order of additions and
 * multiplications. Use std_latch.cpp to study ordering between groups of work.
 */

#include <iostream>
#include <syncstream>
#include <mutex>
#include <thread>
#include <vector>

unsigned int task_counter = 1;
std::mutex counter_mutex;

void simulate_work() {
    // Encourage different schedules without a volatile busy-work loop.
    std::this_thread::yield();
}

void worker_a() {
    simulate_work();

    // Lock mutex before modifying shared state
    std::scoped_lock lock(counter_mutex);
    task_counter *= 2;
    std::osyncstream(std::cout) << "Worker A DOUBLED the counter.\n";
}

void worker_b() {
    simulate_work();

    // Lock mutex before modifying shared state
    std::scoped_lock lock(counter_mutex);
    task_counter += 3;
    std::osyncstream(std::cout) << "Worker B ADDED 3 to the counter.\n";
}

int main() {
    std::vector<std::jthread> workers;

    // Create alternating A and B workers
    for (int i = 0; i < 10; i += 2) {
        workers.emplace_back(worker_a);
        workers.emplace_back(worker_b);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::osyncstream(std::cout) << "Final counter value: " << task_counter << "\n";
    return 0;
}
