/**
 * Exception Handling Across Thread Boundaries
 *
 * Exceptions thrown in a std::thread do NOT propagate to the spawning
 * thread — an uncaught exception in a thread calls std::terminate.
 * This example shows two safe ways to propagate exceptions back to the
 * calling thread.
 *
 * Key concepts:
 * - std::exception_ptr can capture any exception in-flight
 * - std::current_exception() grabs the active exception
 * - std::rethrow_exception() re-throws it in another thread
 * - std::future (from std::async or std::packaged_task) does this automatically
 *
 * See also: src/cpp/asynchrony/promise_future.cpp for the async approach.
 *
 * Compile: g++ -std=c++11 -pthread exception_in_thread.cpp
 */

#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

// ─── ANTIPATTERN: uncaught exception terminates the program ───────────────────
//
// The code below is intentionally left commented out.
// Uncomment it to observe std::terminate being called:
//
//   std::thread bad([] { throw std::runtime_error("crash"); });
//   bad.join();  // Program terminates before reaching this line!

// ─── Pattern 1: manual propagation with std::exception_ptr ───────────────────

void risky_worker(std::exception_ptr& out_exc) {
    try {
        throw std::runtime_error("Error inside worker thread");
    } catch (...) {
        out_exc = std::current_exception();  // Capture, don't re-throw yet
    }
}

void demo_exception_ptr() {
    std::cout << "=== Manual exception_ptr propagation ===\n";

    std::exception_ptr captured;
    std::thread worker(risky_worker, std::ref(captured));
    worker.join();

    // Re-throw in the main thread where it can be handled normally
    if (captured) {
        try {
            std::rethrow_exception(captured);
        } catch (const std::exception& e) {
            std::cout << "Caught from worker thread: " << e.what() << "\n";
        }
    }
}

// ─── Pattern 2: automatic propagation via std::async / std::future ───────────
//
// std::future stores any exception thrown by the async callable and
// re-throws it automatically when get() is called.

void demo_future_exception() {
    std::cout << "\n=== Automatic propagation via std::future ===\n";

    auto future = std::async(std::launch::async, []() -> int {
        throw std::logic_error("Async worker failed!");
        return 42;  // Never reached
    });

    try {
        int result = future.get();  // Re-throws the stored exception
        std::cout << "Result: " << result << "\n";
    } catch (const std::exception& e) {
        std::cout << "Caught from async: " << e.what() << "\n";
    }
}

// ─── Pattern 3: multiple workers, collect all exceptions ─────────────────────

void demo_multiple_exceptions() {
    std::cout << "\n=== Multiple workers, collect all errors ===\n";
    constexpr int kWorkers = 4;

    std::vector<std::exception_ptr> errors(kWorkers);
    std::vector<std::thread> threads;

    for (int i = 0; i < kWorkers; ++i) {
        threads.emplace_back([i, &errors] {
            try {
                if (i % 2 == 0) {
                    throw std::runtime_error("Worker " + std::to_string(i) +
                                             " failed");
                }
                std::cout << "Worker " << i << " succeeded.\n";
            } catch (...) {
                errors[i] = std::current_exception();
            }
        });
    }

    for (auto& t : threads) t.join();

    for (int i = 0; i < kWorkers; ++i) {
        if (errors[i]) {
            try {
                std::rethrow_exception(errors[i]);
            } catch (const std::exception& e) {
                std::cout << "Error from worker " << i << ": " << e.what()
                          << "\n";
            }
        }
    }
}

int main() {
    demo_exception_ptr();
    demo_future_exception();
    demo_multiple_exceptions();
    return 0;
}
