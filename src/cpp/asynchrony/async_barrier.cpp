/**
 * C++20 Barrier Synchronization
 *
 * This example demonstrates std::barrier (C++20) for synchronizing
 * multiple tasks at a common point.
 *
 * Key concepts:
 * - std::barrier blocks threads until all reach the barrier
 * - arrive_and_wait() signals arrival and waits for all others
 * - Barrier is reusable - can be used for multiple phases
 * - Useful for phased parallel algorithms
 *
 * Compile with: g++ -std=c++20 -pthread async_barrier.cpp
 */

#include <barrier>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

class Worker {
public:
    Worker(int id, std::barrier<>* barrier)
        : id_(id), barrier_(barrier) {}

    void operator()() {
        phase1();
        barrier_->arrive_and_wait();  // Sync point
        phase2();
    }

private:
    void phase1() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 3);

        std::cout << "Worker " << id_ << " phase 1 starting\n";
        std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
        std::cout << "Worker " << id_ << " phase 1 done\n";
    }

    void phase2() {
        std::cout << "Worker " << id_ << " phase 2 starting\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Worker " << id_ << " phase 2 done\n";
    }

    int id_;
    std::barrier<>* barrier_;
};

int main() {
    constexpr int num_workers = 4;
    std::barrier sync_point(num_workers);

    std::vector<std::thread> workers;
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back(Worker(i, &sync_point));
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "All workers completed\n";
    return 0;
}
