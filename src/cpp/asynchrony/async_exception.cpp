/**
 * Exception Propagation Through std::future
 *
 * When a task launched with std::async (or packaged_task/promise) throws,
 * the exception is captured and re-thrown when .get() is called on the future.
 * This is the standard mechanism for surfacing errors from background threads.
 *
 * Covered patterns:
 *   1. Exception from std::async re-thrown at .get()
 *   2. Multiple tasks — each future independently carries success or failure
 *   3. promise::set_exception() — explicit exception injection
 *   4. Unhandled exception inside a detached thread (crash — antipattern note)
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 async_exception.cpp
 */

#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

// ── Demo 1: basic exception propagation via std::async ────────────────────

void demo_basic_propagation() {
    std::cout << "=== Demo 1: exception propagated through future ===\n";

    auto fut = std::async(std::launch::async, []() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        throw std::runtime_error("something went wrong in the task");
        return 0;  // unreachable
    });

    try {
        int result = fut.get();  // re-throws the exception here
        std::cout << "  result = " << result << "\n";  // not reached
    } catch (const std::runtime_error& e) {
        std::cout << "  caught: " << e.what() << "\n\n";
    }
}

// ── Demo 2: multiple tasks, some succeed some fail ─────────────────────────

int risky_task(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (id % 2 == 0) {
        throw std::runtime_error("task " + std::to_string(id) + " failed");
    }
    return id * 10;
}

void demo_multiple_tasks() {
    std::cout << "=== Demo 2: multiple tasks, mixed success / failure ===\n";

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 6; ++i) {
        futures.push_back(std::async(std::launch::async, risky_task, i));
    }

    for (int i = 0; i < (int)futures.size(); ++i) {
        try {
            std::cout << "  task " << i << " result = " << futures[i].get() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  task " << i << " exception: " << e.what() << "\n";
        }
    }
    std::cout << "\n";
}

// ── Demo 3: promise::set_exception — explicit injection ───────────────────

void demo_promise_exception() {
    std::cout << "=== Demo 3: promise::set_exception ===\n";

    std::promise<int> p;
    std::future<int> f = p.get_future();

    std::thread worker([&p]() {
        try {
            // simulate work that can fail
            throw std::overflow_error("overflow in worker");
        } catch (...) {
            // capture and forward the current exception into the promise
            p.set_exception(std::current_exception());
        }
    });
    worker.join();

    try {
        f.get();
    } catch (const std::overflow_error& e) {
        std::cout << "  caught from promise: " << e.what() << "\n\n";
    }
}

// ── Demo 4: exception_ptr — store and rethrow anywhere ────────────────────

void demo_exception_ptr_transport() {
    std::cout << "=== Demo 4: std::exception_ptr transport ===\n";

    std::exception_ptr ep;

    // Background thread captures exception into exception_ptr
    std::thread t([&ep]() {
        try {
            throw std::logic_error("logic error in thread");
        } catch (...) {
            ep = std::current_exception();
        }
    });
    t.join();

    // Main thread rethrows it
    if (ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception& e) {
            std::cout << "  rethrown on main thread: " << e.what() << "\n\n";
        }
    }
}

// ── NOTE: antipattern — unhandled exception in detached thread ────────────
//
//   std::thread([]{ throw std::runtime_error("boom"); }).detach();
//
// An unhandled exception in a std::thread calls std::terminate().
// There is no way to catch it from the creating thread.
// Always use futures, promises, or exception_ptr to propagate errors
// out of background threads.

int main() {
    demo_basic_propagation();
    demo_multiple_tasks();
    demo_promise_exception();
    demo_exception_ptr_transport();

    std::cout << "All demos complete.\n";
    return 0;
}
