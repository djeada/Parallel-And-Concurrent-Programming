/**
 * Modern C++20: Pause, Resume, and Cancel an Owned Worker
 *
 * A mutex protects the pause predicate. std::jthread supplies the stop_token,
 * and condition_variable_any supports interruptible waits, including when
 * the task is paused. The thread's scope supplies the completion boundary.
 *
 * Compile: g++ -std=c++20 -pthread pause_resume.cpp
 */

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <syncstream>
#include <thread>

int main() {
    std::mutex mutex;
    std::condition_variable_any changed;
    bool paused = false;  // Protected by mutex; outlives the worker.

    {
        std::jthread task([&](std::stop_token stop) {
            int step = 0;
            std::unique_lock lock(mutex);
            while (changed.wait(lock, stop, [&] { return !paused; })) {
                if (stop.stop_requested()) break;
                std::osyncstream(std::cout) << "Executing step " << step++ << '\n';
                changed.wait_for(lock, stop, std::chrono::seconds(1), [&] {
                    return paused;
                });
            }
        });

        std::osyncstream(std::cout)
            << "Task running. Commands: 'p' = pause/resume, 'q' = quit\n";
        char command;
        while (std::cin >> command) {
            if (command == 'q') break;
            if (command == 'p') {
                bool is_paused;
                {
                    std::scoped_lock lock(mutex);
                    paused = !paused;
                    is_paused = paused;
                }
                changed.notify_all();
                std::osyncstream(std::cout) << (is_paused ? "Paused\n" : "Resumed\n");
            }
        }
        // EOF and 'q' both leave this scope. The jthread destructor requests
        // stop, wakes either interruptible wait, and joins the worker.
    }

    std::cout << "Task completed\n";
}
