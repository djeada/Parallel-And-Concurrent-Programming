/**
 * C++20 + POSIX: several scoped child processes.
 * Each fork is checked, and only successfully created children are waited for.
 * An exception during startup terminates/reaps children already started.
 * Compile: g++ -std=c++20 -pthread multiple_processes.cpp
 */
#include "../common/posix.hpp"

#include <chrono>
#include <iostream>
#include <thread>

int main() try {
    posix_example::ChildGroup children(2);
    children.spawn([] {
        std::cout << "Task A started (PID: " << getpid() << ")\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Task A finished\n";
    });
    children.spawn([] {
        std::cout << "Task B started (PID: " << getpid() << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Task B finished\n";
    });
    if (!children.wait_all()) return 1;
    std::cout << "All children finished\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
