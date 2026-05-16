/**
 * std::counting_semaphore and std::binary_semaphore (C++20)
 *
 * C++20 provides two standard semaphore types:
 * - std::counting_semaphore<N> : permits up to N concurrent acquisitions
 * - std::binary_semaphore      : alias for counting_semaphore<1>
 *
 * Key operations:
 * - acquire()          : decrement counter; block if it reaches zero
 * - release(n=1)       : increment counter by n; wake waiting threads
 * - try_acquire()      : non-blocking attempt; returns false if count is zero
 * - try_acquire_for()  : attempt with timeout
 *
 * Key difference from mutex:
 * - A semaphore can be released by a DIFFERENT thread than the one that
 *   acquired it — enabling producer/consumer signaling patterns.
 *
 * Compare with semaphore.cpp which shows a manual implementation.
 *
 * Compile: g++ -std=c++20 -pthread std_semaphore.cpp
 */

#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

// ─── Example 1: counting_semaphore as a bounded resource pool ────────────────
//
// Model: only 3 workers may be in the "server room" simultaneously.

std::counting_semaphore<3> room_capacity{3};

void server_room_worker(int id) {
    std::cout << "Worker " << id << " waiting to enter room...\n";
    room_capacity.acquire();
    std::cout << "Worker " << id << " entered room.\n";

    std::this_thread::sleep_for(300ms);  // Simulate work inside

    std::cout << "Worker " << id << " leaving room.\n";
    room_capacity.release();
}

void demo_counting_semaphore() {
    std::cout << "=== counting_semaphore<3>: 7 workers, room fits 3 ===\n";
    std::vector<std::thread> workers;
    for (int i = 1; i <= 7; ++i) {
        workers.emplace_back(server_room_worker, i);
    }
    for (auto& w : workers) w.join();
}

// ─── Example 2: binary_semaphore for one-shot signaling ──────────────────────
//
// Model: consumer waits for producer to finish computing a result.
// Unlike mutex, the semaphore starts at 0 (not signaled).

std::binary_semaphore result_ready{0};  // starts at 0 = "not ready"
int computed_value = 0;

void producer_task() {
    std::cout << "Producer: computing...\n";
    std::this_thread::sleep_for(500ms);
    computed_value = 99;
    std::cout << "Producer: result ready.\n";
    result_ready.release();  // signal consumer — different thread!
}

void consumer_task() {
    result_ready.acquire();  // block until producer signals
    std::cout << "Consumer: received value = " << computed_value << "\n";
}

void demo_binary_semaphore() {
    std::cout << "\n=== binary_semaphore: producer signals consumer ===\n";
    std::thread prod(producer_task);
    std::thread cons(consumer_task);
    prod.join();
    cons.join();
}

int main() {
    demo_counting_semaphore();
    demo_binary_semaphore();
    return 0;
}
