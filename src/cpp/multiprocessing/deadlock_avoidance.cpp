/**
 * C++20 + POSIX: consistent lock order with scoped release.
 * Both processes acquire lock_a then lock_b, eliminating circular wait.
 * Compare deadlock_antipattern.cpp for intentionally opposite lock orders.
 * std::scoped_lock's multi-mutex algorithm requires try_lock(); these semaphore
 * owners instead use nested lock_guards with an explicit global order.
 * Compile: g++ -std=c++20 -pthread deadlock_avoidance.cpp
 */
#include "../common/posix.hpp"

#include <iostream>
#include <mutex>

int main() try {
    posix_example::Semaphore lock_a(1, "lock_a");
    posix_example::Semaphore lock_b(1, "lock_b");
    const auto work = [&](const char* name) {
        std::lock_guard first(lock_a);
        std::lock_guard second(lock_b);
        std::cout << name << ": acquired lock_a and lock_b; done\n";
    };
    posix_example::ChildGroup children(1);
    children.spawn([&] { work("Process A"); });
    work("Process B");
    if (!children.wait_all()) return 1;
    std::cout << "Both processes finished successfully — no deadlock.\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
