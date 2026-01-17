/**
 * Race Condition with Mutex Solution
 *
 * This example demonstrates how to use a mutex to prevent race conditions.
 * Compare with simple_data_race.cpp to see the problem this solves.
 *
 * Key concepts:
 * - std::mutex provides mutual exclusion
 * - std::scoped_lock automatically acquires and releases mutex (RAII)
 * - Only one thread can hold the mutex at a time
 * - Order of execution becomes deterministic within critical sections
 *
 * Without the mutex, the final value would be unpredictable.
 * With the mutex, operations happen in sequence, guaranteeing correctness.
 */

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

unsigned int task_counter = 1;
std::mutex counter_mutex;

void simulate_work() {
    volatile unsigned long x = 0;
    for (unsigned long i = 0; i < 1'000'000; ++i) {
        ++x;
    }
}

void worker_a() {
    simulate_work();

    // Lock mutex before modifying shared state
    std::scoped_lock lock(counter_mutex);
    task_counter *= 2;
    std::cout << "Worker A DOUBLED the counter.\n";
}

void worker_b() {
    simulate_work();

    // Lock mutex before modifying shared state
    std::scoped_lock lock(counter_mutex);
    task_counter += 3;
    std::cout << "Worker B ADDED 3 to the counter.\n";
}

int main() {
    std::vector<std::thread> workers;

    // Create alternating A and B workers
    for (int i = 0; i < 10; i += 2) {
        workers.emplace_back(worker_a);
        workers.emplace_back(worker_b);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "Final counter value: " << task_counter << "\n";
    return 0;
}
