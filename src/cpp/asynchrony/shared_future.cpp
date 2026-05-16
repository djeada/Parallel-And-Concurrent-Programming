/**
 * std::shared_future — Broadcasting One Result to Many Consumers
 *
 * Key difference from std::future:
 * - std::future is move-only and can only be .get()-ed ONCE.
 * - std::shared_future is copyable; every copy can call .get()
 *   independently and receives the same value.
 *
 * Use cases:
 * - One computation whose result is needed by N downstream tasks
 * - Fan-out: dispatch work after a common initialization step
 * - "All workers wait for the signal, then proceed simultaneously"
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 shared_future.cpp
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

// ── Demo 1: shared_future obtained by sharing a regular future ─────────────

int compute_config() {
    std::cout << "  [config] computing...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return 42;
}

void demo_shared_broadcast() {
    std::cout << "=== Demo 1: shared_future broadcast ===\n";

    // Create the source future and share it
    std::shared_future<int> sf =
        std::async(std::launch::async, compute_config).share();  // returns shared_future

    constexpr int num_workers = 5;
    std::vector<std::future<void>> workers;

    for (int i = 0; i < num_workers; ++i) {
        // Each lambda captures a *copy* of the shared_future — all valid
        workers.push_back(std::async(std::launch::async, [sf, i]() {
            int cfg = sf.get();  // blocks until ready, then returns same value
            std::cout << "  Worker " << i << " got config = " << cfg << "\n";
        }));
    }

    for (auto& w : workers) w.get();
    std::cout << "  All workers done.\n\n";
}

// ── Demo 2: promise + shared_future start-gate ──────────────────────────────
// A common pattern: a "ready" signal shared among many threads.

void demo_start_gate() {
    std::cout << "=== Demo 2: promise-based start gate ===\n";

    std::promise<void> ready_promise;
    std::shared_future<void> ready_signal = ready_promise.get_future().share();

    constexpr int num_racers = 4;
    std::vector<std::future<void>> racers;

    for (int i = 0; i < num_racers; ++i) {
        racers.push_back(std::async(std::launch::async, [ready_signal, i]() {
            std::cout << "  Racer " << i << " ready, waiting for start...\n";
            ready_signal.get();  // wait for the signal
            std::cout << "  Racer " << i << " GO!\n";
        }));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "  --- Starting all racers ---\n";
    ready_promise.set_value();  // broadcast to all waiting racers

    for (auto& r : racers) r.get();
    std::cout << "  Race done.\n\n";
}

// ── Demo 3: std::future vs std::shared_future — the double-get pitfall ──────
// Calling .get() twice on a std::future throws std::future_error.
// On a std::shared_future it is safe.

void demo_double_get() {
    std::cout << "=== Demo 3: double .get() — future vs shared_future ===\n";

    // std::future — second .get() throws
    {
        auto fut = std::async(std::launch::async, []{ return 7; });
        int v1 = fut.get();
        std::cout << "  future first  .get() = " << v1 << "\n";
        try {
            int v2 = fut.get();  // second call on exhausted future
            (void)v2;
        } catch (const std::future_error& e) {
            std::cout << "  future second .get() threw: " << e.what() << "\n";
        }
    }

    // std::shared_future — multiple .get() calls are fine
    {
        std::shared_future<int> sf =
            std::async(std::launch::async, []{ return 7; }).share();
        std::cout << "  shared_future first  .get() = " << sf.get() << "\n";
        std::cout << "  shared_future second .get() = " << sf.get() << "\n";
    }

    std::cout << "\n";
}

int main() {
    demo_shared_broadcast();
    demo_start_gate();
    demo_double_get();
    return 0;
}
