/**
 * std::barrier — C++20 Reusable Phase Synchronization
 *
 * std::barrier blocks threads at a synchronization point until the expected
 * number of threads arrive. Unlike std::latch, it automatically resets after
 * each phase, making it ideal for iterative algorithms.
 *
 * Key operations:
 * - arrive_and_wait()  : signal arrival and block until all threads arrive
 * - arrive()           : signal arrival without blocking (returns token)
 * - arrive_and_drop()  : leave the barrier permanently (reduces expected count)
 *
 * Completion function:
 * - Optionally passed to the barrier constructor
 * - Called once (by one thread) when all participants have arrived
 * - Runs before any thread is released for the next phase
 * - Useful for aggregating results or printing phase summaries
 *
 * Compare with barrier.cpp which shows a manual implementation.
 *
 * Compile: g++ -std=c++20 -pthread std_barrier.cpp
 */

#include <barrier>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

constexpr int kWorkers = 4;
constexpr int kPhases  = 3;

// Shared workspace — each worker writes to its own slot, no mutex needed
std::vector<int> results(kWorkers, 0);

int main() {
    int phase = 0;  // Captured by reference in completion lambda

    // Completion function: called once when all workers finish each phase.
    // Runs with the barrier internally locked — safe to access shared state.
    std::barrier sync(kWorkers, [&phase] {
        int total = std::accumulate(results.begin(), results.end(), 0);
        std::cout << "  ─── Phase " << ++phase
                  << " complete. Sum = " << total << " ───\n";
    });

    std::vector<std::thread> workers;
    workers.reserve(kWorkers);

    for (int id = 0; id < kWorkers; ++id) {
        workers.emplace_back([id, &sync] {
            for (int p = 1; p <= kPhases; ++p) {
                // Compute this worker's contribution for the phase
                results[id] = id * p;
                std::cout << "Worker " << id
                          << " finished phase " << p
                          << " (result = " << results[id] << ")\n";

                // Wait for all workers; completion function runs here
                sync.arrive_and_wait();
            }
            std::cout << "Worker " << id << " done.\n";
        });
    }

    for (auto& w : workers) w.join();

    std::cout << "\nAll phases complete.\n";
    return 0;
}
