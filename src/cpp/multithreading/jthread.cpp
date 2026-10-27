/**
 * C++20 std::jthread and Cooperative Cancellation
 *
 * std::jthread is an improved std::thread that:
 * - Automatically joins on destruction (prevents std::terminate on missed join)
 * - Supports cooperative cancellation via std::stop_token
 *
 * Key concepts:
 * - jthread destructor calls request_stop() then join() automatically
 * - stop_token is passed as the FIRST parameter if the function accepts one
 * - stop_requested() checks whether a stop has been requested
 * - stop_callback runs in the thread requesting stop (or during registration
 *   if stop has already been requested); synchronize access to worker state
 * - stop_source / stop_token can be shared across multiple threads
 *
 * Compile: g++ -std=c++20 -pthread jthread.cpp
 */

#include <chrono>
#include <iostream>
#include <syncstream>
#include <stop_token>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

// ─── Example 1: auto-join on scope exit ───────────────────────────────────────

void background_task(int id) {
    for (int i = 0; i < 5; ++i) {
        std::osyncstream(std::cout) << "Task " << id << " step " << i << "\n";
        std::this_thread::sleep_for(200ms);
    }
}

void demo_auto_join() {
    std::osyncstream(std::cout) << "=== jthread auto-join demo ===\n";
    {
        std::jthread t1(background_task, 1);
        std::jthread t2(background_task, 2);
        // No explicit join() needed - both threads join when t1, t2 go out of scope
    }
    std::osyncstream(std::cout) << "Both tasks complete (auto-joined).\n\n";
}

// ─── Example 2: cooperative cancellation with stop_token ──────────────────────

void monitor(std::stop_token stop_token, const char* name, int interval_ms) {
    int ticks = 0;
    while (!stop_token.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        if (!stop_token.stop_requested()) {
            std::osyncstream(std::cout) << name << " tick " << ++ticks << "\n";
        }
    }
    std::osyncstream(std::cout) << name << " stopped after " << ticks << " ticks.\n";
}

void demo_stop_token() {
    std::osyncstream(std::cout) << "=== stop_token cooperative cancellation ===\n";

    // jthread passes stop_token as first argument automatically
    std::jthread fast(monitor, "Fast", 200);
    std::jthread slow(monitor, "Slow", 500);

    std::this_thread::sleep_for(1500ms);

    std::osyncstream(std::cout) << "Requesting stops...\n";
    fast.request_stop();
    slow.request_stop();
    // Destructors join - no explicit join() needed
}

// ─── Example 3: stop_callback for notification on cancellation ───────────────

void worker_with_notification(std::stop_token stop_token, int id) {
    // The requester invokes this callback; it does not run on this worker.
    std::stop_callback notification(stop_token, [id] {
        std::osyncstream(std::cout) << "Worker " << id << ": stop callback triggered.\n";
    });

    int steps = 0;
    while (!stop_token.stop_requested()) {
        ++steps;
        std::this_thread::sleep_for(100ms);
    }
    std::osyncstream(std::cout) << "Worker " << id << " completed " << steps << " steps.\n";
}

void demo_stop_callback() {
    std::osyncstream(std::cout) << "\n=== stop_callback demo ===\n";

    std::vector<std::jthread> workers;
    for (int i = 0; i < 3; ++i) {
        workers.emplace_back(worker_with_notification, i);
    }

    std::this_thread::sleep_for(500ms);
    std::osyncstream(std::cout) << "Stopping all workers...\n";

    for (auto& w : workers) {
        w.request_stop();
    }
    // Workers auto-join when workers vector is destroyed
}

int main() {
    demo_auto_join();
    demo_stop_token();
    demo_stop_callback();
    return 0;
}
