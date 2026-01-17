/**
 * Running Heavy Functions Asynchronously
 *
 * This example demonstrates running computationally intensive
 * (blocking) functions in the background while keeping the
 * main thread responsive.
 *
 * Key concepts:
 * - std::async allows heavy work to run in background threads
 * - Main thread remains responsive for user interaction
 * - Results are collected when all tasks complete
 * - Exception handling in async tasks
 *
 * This pattern is essential for GUI applications or services
 * that must remain responsive during long computations.
 */

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

int blocking_computation(int id) {
    std::cout << "Task " << id << " starting...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Task " << id << " done\n";
    return id * 10;
}

void handle_user_input(std::atomic<bool>& running) {
    std::cout << "Type 'quit' to exit once tasks complete\n";
    std::string input;
    while (running && std::getline(std::cin, input)) {
        if (input == "quit") {
            running = false;
            break;
        }
        std::cout << "You typed: " << input << "\n";
    }
}

int main() {
    // Launch background tasks
    std::vector<std::future<int>> tasks;
    for (int i = 0; i < 4; ++i) {
        tasks.push_back(std::async(std::launch::async, blocking_computation, i));
    }

    // Keep main thread responsive
    std::atomic<bool> running{true};
    std::thread input_thread(handle_user_input, std::ref(running));

    // Collect results
    std::vector<int> results;
    for (auto& task : tasks) {
        results.push_back(task.get());
    }

    std::cout << "Results: ";
    for (int r : results) {
        std::cout << r << " ";
    }
    std::cout << "\n";

    running = false;
    if (input_thread.joinable()) {
        input_thread.join();
    }

    return 0;
}
