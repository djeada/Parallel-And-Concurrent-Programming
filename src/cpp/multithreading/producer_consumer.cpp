/**
 * Modern C++20: Bounded Producer-Consumer Queue with Explicit Closure
 *
 * A capacity limit applies backpressure. close() wakes all waiters, consumers
 * drain accepted items, and an empty optional signals completion. Every int
 * remains a valid task: there is no magic poison-pill value.
 * std::jthread owns the workers; stop-aware waits also allow cancellation.
 *
 * Compile: g++ -std=c++20 -pthread producer_consumer.cpp
 */

#include "../common/bounded_queue.hpp"

#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <stop_token>
#include <syncstream>
#include <thread>

using TaskQueue = cpp_example::BoundedQueue<int>;

int main() {
    TaskQueue queue(32); // Construct shared state before workers that refer to it.
    {
        const auto consume = [&queue](std::stop_token stop, int id) {
            int tasks_completed = 0;
            while (const auto task = queue.take_task(stop)) {
                tasks_completed += *task;
            }
            std::osyncstream(std::cout) << "Consumer " << id << ": completed "
                                      << tasks_completed << " tasks.\n";
        };
        std::jthread consumer1(consume, 1);
        std::jthread consumer2(consume, 2);
        std::jthread producer([&queue](std::stop_token stop) {
            for (int i = 0; i < 10'000; ++i) {
                if (!queue.add_task(1, stop)) break;
            }
        });

        // Completion is required before closing the queue: join() deliberately
        // waits without requesting cancellation, preserving every produced task.
        producer.join();
        queue.close();
    } // Consumers drain the closed queue and join before it is destroyed.
    std::cout << "All tasks completed.\n";
    return 0;
}
