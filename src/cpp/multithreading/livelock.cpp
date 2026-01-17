/**
 * Livelock
 *
 * This example demonstrates livelock - a situation where threads are
 * actively running but making no progress. Unlike deadlock where threads
 * are blocked, in livelock threads keep responding to each other.
 *
 * Key concepts:
 * - Livelock occurs when threads repeatedly change state in response to
 *   each other, preventing progress
 * - try_lock() with backoff can help, but may cause livelock if not
 *   implemented carefully
 * - Adding randomized delays or using lock ordering helps break livelock
 *
 * In this example, workers use try_lock() and yield() to avoid deadlock,
 * but this can lead to temporary livelock conditions.
 */

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

int task_count = 5000;

void worker(std::mutex& first_tool, std::mutex& second_tool,
            const std::string& name) {
    int tasks_done = 0;

    while (task_count > 0) {
        first_tool.lock();

        // Try to acquire second lock without blocking
        if (!second_tool.try_lock()) {
            // Release first lock to avoid deadlock
            first_tool.unlock();

            // This yield can cause livelock if all threads do it together
            std::this_thread::yield();
            continue;
        }

        // Got both locks - do work
        if (task_count > 0) {
            --task_count;
            ++tasks_done;
        }

        second_tool.unlock();
        first_tool.unlock();
    }

    std::cout << name << " completed " << tasks_done << " tasks.\n";
}

int main() {
    std::mutex tool_a, tool_b;

    std::vector<std::thread> workers;

    // Workers acquire tools in different orders - potential for livelock
    workers.emplace_back(worker, std::ref(tool_a), std::ref(tool_b), "Alice");
    workers.emplace_back(worker, std::ref(tool_b), std::ref(tool_a), "Bob");
    workers.emplace_back(worker, std::ref(tool_a), std::ref(tool_b), "Charlie");
    workers.emplace_back(worker, std::ref(tool_b), std::ref(tool_a), "Diana");

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "All tasks completed.\n";
    return 0;
}
