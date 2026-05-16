/**
 * Deadlock Avoidance with std::scoped_lock
 *
 * When two threads each need to hold two mutexes, acquiring them in
 * inconsistent order creates a deadlock. std::scoped_lock (C++17) solves
 * this by atomically acquiring any number of mutexes using a deadlock-
 * avoidance algorithm — no matter what order they are listed.
 *
 * Key concepts:
 * - std::scoped_lock<M1, M2, ...> acquires ALL listed mutexes atomically
 * - Internally uses std::lock() which applies a deadlock-free algorithm
 * - The two workers pass resources in opposite order — yet no deadlock occurs
 *
 * See deadlock_philosophers_antipattern.cpp for the deadlocking version.
 *
 * Compile: g++ -std=c++17 -pthread scoped_lock_deadlock_avoidance.cpp
 */

#include <iostream>
#include <mutex>
#include <thread>

int item_count = 5000;

void worker(std::mutex& first_resource, std::mutex& second_resource) {
    while (true) {
        // Acquire BOTH mutexes atomically — order doesn't matter
        std::scoped_lock lock(first_resource, second_resource);

        if (item_count <= 0) break;
        --item_count;
    }
}

int main() {
    std::mutex resource_a, resource_b;

    // Alice locks A then B; Bob locks B then A.
    // scoped_lock's deadlock-avoidance algorithm makes this safe.
    std::thread alice(worker, std::ref(resource_a), std::ref(resource_b));
    std::thread bob(worker,   std::ref(resource_b), std::ref(resource_a));

    alice.join();
    bob.join();

    std::cout << "Workers completed without deadlock. "
              << "Remaining items: " << item_count << "\n";
    return 0;
}
