/**
 * C++20 Coroutine Generator
 *
 * This example demonstrates implementing an async generator using
 * C++20 coroutines. Values are produced lazily on demand.
 *
 * Key concepts:
 * - co_yield suspends and produces a value
 * - Coroutine handle allows resumption from suspension point
 * - promise_type defines coroutine behavior
 * - Lazy evaluation: values generated only when requested
 *
 * Coroutines enable writing async code that looks synchronous,
 * making complex control flow easier to understand.
 *
 * Compile with: g++ -std=c++20 -fcoroutines coroutine_generator.cpp
 */

#include <chrono>
#include <coroutine>
#include <iostream>
#include <random>
#include <thread>

template <typename T>
class Generator {
public:
    struct promise_type {
        T value_;

        Generator get_return_object() {
            return Generator{Handle::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            value_ = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle h) : handle_(h) {}

    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // Move-only
    Generator(Generator&& other) : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    Generator& operator=(Generator&& other) {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    bool next() {
        handle_.resume();
        return !handle_.done();
    }

    T value() { return handle_.promise().value_; }

private:
    Handle handle_;
};

Generator<int> async_numbers(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100, 500);

    for (int i = 0; i < count; ++i) {
        // Simulate async work
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
        co_yield i * i;  // Suspend and produce value
    }
}

int main() {
    std::cout << "Generating squares lazily:\n";

    auto gen = async_numbers(5);
    while (gen.next()) {
        std::cout << "Got: " << gen.value() << "\n";
    }

    std::cout << "Done\n";
    return 0;
}
