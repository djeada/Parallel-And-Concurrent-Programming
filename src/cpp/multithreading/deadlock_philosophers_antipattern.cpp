/**
 * Dining Philosophers - Deadlock Antipattern
 *
 * ANTIPATTERN: Each philosopher grabs their LEFT fork first, then waits for
 * their RIGHT fork. When all philosophers grab their left fork at the same
 * time, nobody can acquire their right fork — circular wait — DEADLOCK.
 *
 * The four necessary conditions for deadlock (all present here):
 *   1. Mutual exclusion  — only one philosopher holds a fork at a time
 *   2. Hold-and-wait     — philosopher holds left fork while waiting for right
 *   3. No preemption     — a held fork cannot be forcibly taken away
 *   4. Circular wait     — philosopher 0 waits on 1, 1 on 2, ..., 4 on 0
 *
 * This example uses a manual start-gate so all philosophers grab their left
 * fork before anyone attempts their right — making the deadlock reproducible.
 * std::timed_mutex::try_lock_for() detects the deadlock instead of hanging.
 *
 * See scoped_lock_deadlock_avoidance.cpp for a solution using std::scoped_lock.
 *
 * WARNING: This code intentionally demonstrates a deadlock pattern!
 *
 * Compile: g++ -std=c++11 -pthread deadlock_philosophers_antipattern.cpp
 */

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

constexpr int kNumPhilosophers = 5;

// Five forks (one between each pair of adjacent philosophers)
std::timed_mutex forks[kNumPhilosophers];

// Start-gate: ensures ALL philosophers hold their left fork before any
// attempts the right fork, guaranteeing circular wait for demonstration.
std::mutex gate_mutex;
std::condition_variable gate_cv;
int philosophers_ready = 0;

void wait_for_all_holding_left() {
    std::unique_lock<std::mutex> lk(gate_mutex);
    ++philosophers_ready;
    if (philosophers_ready == kNumPhilosophers) {
        gate_cv.notify_all();
    } else {
        gate_cv.wait(lk, [] { return philosophers_ready == kNumPhilosophers; });
    }
}

void philosopher(int id) {
    int left_fork  = id;
    int right_fork = (id + 1) % kNumPhilosophers;

    // Step 1: grab left fork (always succeeds — no contention yet)
    forks[left_fork].lock();
    std::cout << "Philosopher " << id
              << " picked up fork " << left_fork << " (left).\n";

    // Step 2: synchronize — wait until all philosophers hold their left fork
    wait_for_all_holding_left();

    // Step 3: try to grab right fork — will DEADLOCK
    std::cout << "Philosopher " << id
              << " is waiting for fork " << right_fork << " (right)...\n";

    bool got_right = forks[right_fork].try_lock_for(std::chrono::seconds(2));

    if (got_right) {
        // This branch is NOT reached in the deadlock scenario
        std::cout << "Philosopher " << id << " is eating.\n";
        forks[right_fork].unlock();
    } else {
        std::cout << "DEADLOCK DETECTED: Philosopher " << id
                  << " timed out waiting for fork " << right_fork << "!\n";
    }

    forks[left_fork].unlock();
}

int main() {
    std::cout << "=== Dining Philosophers — DEADLOCK Demo ===\n";
    std::cout << "Each philosopher grabs their left fork, then waits for right.\n"
              << "Circular wait → DEADLOCK.\n\n";

    std::vector<std::thread> philosophers;
    philosophers.reserve(kNumPhilosophers);
    for (int i = 0; i < kNumPhilosophers; ++i) {
        philosophers.emplace_back(philosopher, i);
    }
    for (auto& p : philosophers) p.join();

    std::cout << "\nDeadlock demonstrated: " << kNumPhilosophers
              << " philosophers all held one fork and waited for the next.\n"
              << "Without timed locks they would wait forever.\n"
              << "See scoped_lock_deadlock_avoidance.cpp for the fix.\n";
    return 0;
}
