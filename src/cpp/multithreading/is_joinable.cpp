/**
 * Thread Joinability
 *
 * This example demonstrates the joinable() method and thread lifecycle.
 *
 * Key concepts:
 * - A thread is "joinable" if it represents an active thread of execution
 * - joinable() returns true from construction until join() or detach() is called
 * - You MUST either join() or detach() a thread before its destructor runs
 * - Calling join() on a non-joinable thread throws std::system_error
 *
 * Thread lifecycle:
 * 1. Construction: thread becomes joinable
 * 2. Running: thread executes its function
 * 3. Completion: thread finishes but is still joinable
 * 4. join()/detach(): thread becomes non-joinable
 */

#include <chrono>
#include <iostream>
#include <thread>

void background_work() {
    std::cout << "  Worker: started processing...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "  Worker: finished processing.\n";
}

int main() {
    std::cout << "Creating worker thread...\n";
    std::thread worker(background_work);

    // Thread is joinable immediately after construction
    std::cout << "  Is joinable? " << std::boolalpha << worker.joinable() << "\n";

    std::cout << "Main: doing other work...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Still joinable while running
    std::cout << "  Is joinable? " << worker.joinable() << "\n";

    std::cout << "Main: waiting for worker to finish...\n";
    worker.join();

    // No longer joinable after join()
    std::cout << "  Is joinable? " << worker.joinable() << "\n";

    std::cout << "Both threads complete.\n";
    return 0;
}
