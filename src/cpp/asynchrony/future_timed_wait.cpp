/**
 * Timed Waits on std::future — wait_for, wait_until, future_status
 *
 * std::future::get() blocks indefinitely.  For responsive programs you
 * often need a deadline:
 *
 *   future_status::ready   — the result is available
 *   future_status::timeout — the deadline elapsed, task still running
 *   future_status::deferred — task was launched with std::launch::deferred
 *                             and has not started yet
 *
 * Key functions:
 *   f.wait_for(duration)  — relative timeout
 *   f.wait_until(tp)      — absolute time-point deadline
 *
 * Important: timed waits do NOT cancel the underlying task.
 * They only affect how long the *caller* blocks.  To cancel work,
 * you must use a separate mechanism (stop_token, atomic flag, etc.).
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 future_timed_wait.cpp
 */

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

// ── Demo 1: wait_for with future_status ────────────────────────────────────

void demo_wait_for() {
    std::cout << "=== Demo 1: wait_for / future_status ===\n";

    auto fut = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(500ms);
        return 42;
    });

    // Poll while doing other work
    for (int poll = 0; ; ++poll) {
        auto status = fut.wait_for(100ms);

        if (status == std::future_status::ready) {
            std::cout << "  Result ready after " << poll << " polls: "
                      << fut.get() << "\n\n";
            break;
        } else if (status == std::future_status::timeout) {
            std::cout << "  Poll " << poll << ": still running...\n";
        } else {
            // deferred — task hasn't started yet
            std::cout << "  Poll " << poll << ": deferred (not started)\n";
        }
    }
}

// ── Demo 2: wait_until with an absolute deadline ───────────────────────────

void demo_wait_until() {
    std::cout << "=== Demo 2: wait_until with deadline ===\n";

    // Task takes 600 ms; we give it 400 ms → times out
    auto deadline = std::chrono::steady_clock::now() + 400ms;

    auto fut = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(600ms);
        return 99;
    });

    auto status = fut.wait_until(deadline);

    if (status == std::future_status::ready) {
        std::cout << "  Finished in time: " << fut.get() << "\n\n";
    } else {
        std::cout << "  Deadline exceeded — task still running\n";
        // Must still collect the result to avoid UB in future destructor
        std::cout << "  Waiting for final result: " << fut.get() << "\n\n";
    }
}

// ── Demo 3: future_status::deferred ────────────────────────────────────────
// With std::launch::deferred the function runs lazily in the CALLER'S thread
// when .get() or .wait() is first called.
// wait_for(0ms) returns deferred — a useful way to detect this.

void demo_deferred_detection() {
    std::cout << "=== Demo 3: detecting deferred launch ===\n";

    auto fut = std::async(std::launch::deferred, []() {
        std::cout << "  [deferred task] running now (in caller's thread)\n";
        return 7;
    });

    // Non-blocking check
    auto status = fut.wait_for(0ms);
    if (status == std::future_status::deferred) {
        std::cout << "  Task is deferred — hasn't started yet\n";
        std::cout << "  Calling .get() will run it right here...\n";
    }

    int v = fut.get();  // runs the lambda in this thread, right now
    std::cout << "  Deferred result: " << v << "\n\n";
}

// ── Demo 4: timeout does NOT cancel the task ──────────────────────────────
// This is a common misconception.  Demonstrate with an atomic flag.

void demo_timeout_no_cancel() {
    std::cout << "=== Demo 4: timeout ≠ cancellation ===\n";

    std::atomic<bool> task_finished{false};

    auto fut = std::async(std::launch::async, [&task_finished]() {
        std::this_thread::sleep_for(600ms);
        task_finished = true;
        return 0;
    });

    auto status = fut.wait_for(200ms);
    std::cout << "  wait_for returned: "
              << (status == std::future_status::timeout ? "timeout" : "ready")
              << "\n";
    std::cout << "  task_finished right after timeout = "
              << std::boolalpha << task_finished.load() << "\n";
    std::cout << "  (Task is still running in background!)\n";
    std::cout << "  Collecting result...\n";
    fut.get();  // wait for it
    std::cout << "  task_finished after .get() = " << task_finished.load() << "\n\n";
    std::cout << "  To actually stop the task, use a stop flag or std::stop_token.\n\n";
}

int main() {
    demo_wait_for();
    demo_wait_until();
    demo_deferred_detection();
    demo_timeout_no_cancel();
    return 0;
}
