/**
 * C++20 Coroutines — co_await with a minimal Task<T>
 *
 * This example shows how co_await differs from co_yield (see coroutine_generator.cpp).
 *
 *   co_yield  → suspend, emit a value, resume when caller asks for next
 *   co_await  → suspend until an *awaitable* signals completion, then
 *               continue with the awaitable's result
 *   co_return → finish the coroutine, provide a final value
 *
 * We implement a minimal Task<T> type:
 *   - Supports co_return T
 *   - Can be co_awaited by another coroutine (chaining)
 *   - Propagates exceptions through the coroutine chain
 *
 * ⚠️  EDUCATIONAL NOTE: This Task<T> is intentionally minimal.
 * It runs synchronously on the calling thread for simplicity.
 * Production coroutine frameworks (cppcoro, Asio, stdexec) have
 * schedulers, executors, thread-safety, and cancellation support.
 * Use this file to understand the mechanics, not as a template.
 *
 * Compile:
 *   g++ -std=c++20 -O1 coroutine_task.cpp
 */

#include <coroutine>
#include <exception>
#include <iostream>
#include <stdexcept>

// ── Task<T>: a simple synchronous coroutine return type ───────────────────

template <typename T>
class Task {
public:
    // ── promise_type tells the compiler how to build Task<T> ────────────
    struct promise_type {
        T result_{};
        std::exception_ptr exception_;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Start suspended so the caller controls when to run
        std::suspend_always initial_suspend() noexcept { return {}; }

        // Stay suspended at end so we can read the result
        std::suspend_always final_suspend()   noexcept { return {}; }

        void return_value(T v) { result_ = std::move(v); }

        void unhandled_exception() { exception_ = std::current_exception(); }
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Task(Handle h) : handle_(h) {}

    ~Task() { if (handle_) handle_.destroy(); }

    // Move-only
    Task(Task&& o) noexcept : handle_(o.handle_) { o.handle_ = nullptr; }
    Task& operator=(Task&&) = delete;
    Task(const Task&)       = delete;
    Task& operator=(const Task&) = delete;

    // Run to completion (used by the outermost caller)
    T run() {
        handle_.resume();
        if (handle_.promise().exception_)
            std::rethrow_exception(handle_.promise().exception_);
        return handle_.promise().result_;
    }

    // ── Awaitable interface — lets another coroutine do: co_await task ──
    bool await_ready() const noexcept { return false; }

    // When awaited: run this task's coroutine to completion, then resume caller
    void await_suspend(std::coroutine_handle<> caller) {
        handle_.resume();   // run inner coroutine
        caller.resume();    // immediately resume outer coroutine
    }

    T await_resume() {
        if (handle_.promise().exception_)
            std::rethrow_exception(handle_.promise().exception_);
        return handle_.promise().result_;
    }

private:
    Handle handle_;
};

// ── Demo: coroutine chain ──────────────────────────────────────────────────

Task<int> step_a() {
    std::cout << "  step_a: starting\n";
    co_return 10;
}

Task<int> step_b(int x) {
    std::cout << "  step_b: received " << x << ", doubling it\n";
    co_return x * 2;
}

Task<int> pipeline() {
    std::cout << "  pipeline: awaiting step_a\n";
    int a = co_await step_a();  // step_a runs, returns 10

    std::cout << "  pipeline: awaiting step_b\n";
    int b = co_await step_b(a);  // step_b runs with 10, returns 20

    std::cout << "  pipeline: combining results\n";
    co_return a + b;  // 10 + 20 = 30
}

// ── Demo: exception propagation through co_await chain ────────────────────

Task<int> may_throw() {
    throw std::runtime_error("error in inner coroutine");
    co_return 0;  // unreachable — suppresses no-co_return warning
}

Task<int> outer_catches() {
    try {
        int v = co_await may_throw();
        (void)v;
    } catch (const std::runtime_error& e) {
        std::cout << "  outer_catches: caught \"" << e.what() << "\"\n";
    }
    co_return -1;
}

int main() {
    std::cout << "=== Demo 1: coroutine chain (step_a -> step_b -> pipeline) ===\n";
    int result = pipeline().run();
    std::cout << "  final result: " << result << "\n\n";

    std::cout << "=== Demo 2: exception propagation through co_await ===\n";
    outer_catches().run();

    return 0;
}
