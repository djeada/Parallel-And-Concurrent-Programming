/**
 * Detached Threads
 *
 * This example demonstrates detached threads - threads that run independently
 * of the thread that created them. When a thread is detached, it continues
 * running even after the main thread ends.
 *
 * Key concepts:
 * - thread.detach() separates the thread from the std::thread object
 * - Detached threads run in the background (daemon-like behavior)
 * - Cannot join a detached thread
 * - Program may exit before detached thread completes
 * - Use for fire-and-forget background tasks
 *
 * Warning: Ensure detached threads don't access resources that may be
 * destroyed when main() ends!
 */

#include <chrono>
#include <iostream>
#include <thread>

void background_monitor() {
    while (true) {
        std::cout << "Background: monitoring logs...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::thread monitor(background_monitor);

    // Detach the thread - it will run independently
    monitor.detach();

    // Main thread continues its own work
    for (int i = 0; i < 3; ++i) {
        std::cout << "Main: processing data...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }

    std::cout << "Main: done! (background thread may still be running)\n";

    // Note: When main() returns, the program exits and the detached
    // thread is terminated. This is why we only see partial output.
    return 0;
}
