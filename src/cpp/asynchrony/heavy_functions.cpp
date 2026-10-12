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
 * - Futures propagate task exceptions through get()
 *
 * Requires POSIX poll/read for interruptible console input (Linux/macOS).
 *
 * This pattern is essential for GUI applications or services
 * that must remain responsive during long computations.
 */

#include <cerrno>
#include <chrono>
#include <future>
#include <iostream>
#include <poll.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

int blocking_computation(int id) {
    std::cout << "Task " << id << " starting...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Task " << id << " done\n";
    return id * 10;
}

// Poll stdin on the main thread so unfinished input cannot block shutdown.
// POSIX poll/read are used because std::getline cannot be cancelled.
void handle_user_input(bool& accepting_input, std::string& input) {
    pollfd stdin_poll{STDIN_FILENO, POLLIN, 0};
    int ready = poll(&stdin_poll, 1, 100);
    if (ready < 0) {
        if (errno != EINTR) accepting_input = false;
        return;
    }
    if (ready == 0) return;
    if (stdin_poll.revents & (POLLIN | POLLHUP)) {
        char ch;
        ssize_t bytes = read(STDIN_FILENO, &ch, 1);
        if (bytes == 0 || (bytes < 0 && errno != EINTR)) {
            accepting_input = false;
        } else if (bytes > 0) {
            if (ch == '\n') {
                if (input == "quit") accepting_input = false;
                else std::cout << "You typed: " << input << "\n";
                input.clear();
            } else {
                input += ch;
            }
        }
    } else if (stdin_poll.revents & (POLLERR | POLLNVAL)) {
        accepting_input = false;
    }
}

int main() {
    // Launch background tasks
    std::vector<std::future<int>> tasks;
    for (int i = 0; i < 4; ++i) {
        tasks.push_back(std::async(std::launch::async, blocking_computation, i));
    }

    std::cout << "Type 'quit' to stop input; all tasks will finish before exit\n";
    bool accepting_input = true;
    std::string input;
    while (true) {
        bool all_ready = true;
        for (auto& task : tasks) {
            if (task.wait_for(std::chrono::milliseconds(0)) !=
                std::future_status::ready) all_ready = false;
        }
        if (all_ready) break;
        if (accepting_input) handle_user_input(accepting_input, input);
        else std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

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

    return 0;
}
