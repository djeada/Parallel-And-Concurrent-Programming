/**
 * C++20 + POSIX: scoped permits across processes.
 * POSIX semaphore handles can be inherited by forked children; std::semaphore
 * does not promise process-shared behavior. A lock_guard releases each permit.
 * This bounds concurrent access, not requests per second.
 * Compile: g++ -std=c++20 -pthread process_semaphore.cpp
 */
#include "../common/posix.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

int main() try {
    constexpr int num_processes = 10;
    posix_example::Semaphore permits(3, "resource_limit");
    posix_example::ChildGroup children(num_processes);
    for (int id = 0; id < num_processes; ++id) {
        children.spawn([&, id] {
            std::lock_guard permit(permits);
            std::cout << "Process " << id << " acquired resource\n" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Process " << id << " releasing resource\n" << std::flush;
        });
    }
    if (!children.wait_all()) return 1;
    std::cout << "All processes completed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
