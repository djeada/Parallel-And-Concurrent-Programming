/**
 * Deadlock - Dining Philosophers Problem
 *
 * This example demonstrates a classic deadlock scenario using the
 * Dining Philosophers problem. Deadlock occurs when threads wait for
 * resources held by each other in a circular dependency.
 *
 * Key concepts:
 * - Deadlock conditions: mutual exclusion, hold-and-wait, no preemption,
 *   circular wait
 * - This code uses std::scoped_lock which PREVENTS deadlock by acquiring
 *   multiple mutexes atomically
 *
 * Note: This is the "solved" version. Without scoped_lock, if each
 * philosopher grabbed one fork and waited for the other, deadlock would occur.
 */

#include <iostream>
#include <mutex>
#include <thread>

int item_count = 5000;

void worker(std::mutex& first_resource, std::mutex& second_resource) {
    while (item_count > 0) {
        // std::scoped_lock acquires BOTH mutexes atomically, preventing deadlock
        // It uses a deadlock-avoidance algorithm internally
        std::scoped_lock lock(first_resource, second_resource);

        if (item_count > 0) {
            --item_count;
        }
    }
}

int main() {
    std::mutex resource_a, resource_b;

    // Workers try to acquire resources in different orders
    // Without scoped_lock, this would cause deadlock!
    std::thread alice(worker, std::ref(resource_a), std::ref(resource_b));
    std::thread bob(worker, std::ref(resource_b), std::ref(resource_a));

    alice.join();
    bob.join();

    std::cout << "Workers completed without deadlock.\n";
    return 0;
}
