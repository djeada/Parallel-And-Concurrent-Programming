/**
 * Modern C++20: Publish Data with Atomic wait/notify
 *
 * For a single ready flag, wait(false) avoids a hand-written polling loop.
 * Release/acquire publishes the payload; notify_one() wakes a blocked waiter.
 * This is a one-shot handoff: the flag never changes back to false.
 * Use a mutex and condition variable for predicates over several values.
 *
 * Compile: g++ -std=c++20 -pthread atomic_wait.cpp
 */

#include <array>
#include <atomic>
#include <iostream>
#include <numeric>
#include <thread>

int main() {
    std::array<int, 5> payload{};
    std::atomic<bool> ready{false};

    {
        // Start the publisher first: if creating the consumer fails, the
        // publisher can still finish and join without leaving a blocked reader.
        std::jthread producer([&] {
            payload = {1, 2, 3, 4, 5};
            ready.store(true, std::memory_order_release);
            ready.notify_one();
        });

        std::jthread consumer([&] {
            // Also works if the producer publishes before wait() starts.
            ready.wait(false, std::memory_order_acquire);
            const auto sum = std::accumulate(payload.begin(), payload.end(), 0);
            std::cout << "Payload sum: " << sum << '\n';
        });

    } // Both threads finish before payload and ready are destroyed.
}
