/**
 * std::latch — C++20 One-Shot Countdown Synchronization
 *
 * std::latch is a single-use synchronization point. Its internal counter
 * starts at a given value; once it reaches zero, all waiting threads are
 * released permanently. Unlike std::barrier it cannot be reset or reused.
 *
 * Key operations:
 * - count_down()       : decrement the counter (non-blocking)
 * - wait()             : block until the counter reaches zero
 * - arrive_and_wait()  : count_down() + wait() combined
 * - try_wait()         : non-blocking check whether counter is zero
 *
 * Common use cases:
 * - "Ready gate": all workers signal ready; main thread waits, then starts them
 * - "Done gate": main thread waits until all workers have finished a phase
 *
 * Compare with barrier.cpp (manual impl) and std_barrier.cpp (C++20 reusable).
 *
 * Compile: g++ -std=c++20 -pthread std_latch.cpp
 */

#include <chrono>
#include <iostream>
#include <latch>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

// Scenario: a coordinator waits until all workers have loaded their data,
// then signals them all to begin simultaneously via a second latch.
void worker(int id, std::latch& loaded_latch, std::latch& start_latch) {
    // Simulate variable-length data loading
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    std::cout << "Worker " << id << ": data loaded.\n";

    // Signal the coordinator that this worker is ready
    loaded_latch.count_down();

    // Block here until the coordinator releases everyone
    start_latch.wait();

    std::cout << "Worker " << id << ": processing!\n";
    std::this_thread::sleep_for(50ms);
    std::cout << "Worker " << id << ": done.\n";
}

int main() {
    constexpr int kWorkers = 5;

    std::latch loaded(kWorkers);  // Workers count down as they finish loading
    std::latch start(1);          // Coordinator counts down once to release all

    std::vector<std::thread> workers;
    workers.reserve(kWorkers);
    for (int i = 1; i <= kWorkers; ++i) {
        workers.emplace_back(worker, i, std::ref(loaded), std::ref(start));
    }

    // Wait until every worker has finished loading
    loaded.wait();
    std::cout << "\nAll workers ready — releasing!\n\n";

    // Release all workers to start processing at the same time
    start.count_down();

    for (auto& w : workers) w.join();

    std::cout << "\nAll workers finished.\n";
    return 0;
}
