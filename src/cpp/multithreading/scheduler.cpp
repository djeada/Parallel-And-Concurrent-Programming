/**
 * Task Scheduler
 *
 * This example demonstrates a simple task scheduler that executes a
 * function at regular intervals using a background thread.
 *
 * Key concepts:
 * - Background thread runs the scheduled task
 * - std::atomic<bool> for thread-safe stop signaling
 * - RAII pattern: destructor stops the scheduler automatically
 * - std::function allows any callable as the task
 *
 * This pattern is useful for periodic tasks like:
 * - Health checks
 * - Log flushing
 * - Heartbeat signals
 */

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class Scheduler {
public:
    Scheduler(int interval_seconds, std::function<void()> task)
        : running_(true),
          interval_(interval_seconds),
          task_(std::move(task)) {
        worker_ = std::thread([this] {
            while (running_) {
                task_();
                std::this_thread::sleep_for(std::chrono::seconds(interval_));
            }
        });
    }

    ~Scheduler() {
        stop();
    }

    // Prevent copying
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    void stop() {
        running_ = false;
        if (worker_.joinable()) {
            worker_.join();
        }
    }

private:
    std::atomic<bool> running_;
    int interval_;
    std::function<void()> task_;
    std::thread worker_;
};

void print_heartbeat() {
    std::cout << "Heartbeat: still alive!\n";
}

int main() {
    std::cout << "Starting scheduler (runs every 1 second)...\n";

    Scheduler scheduler(1, print_heartbeat);

    // Let it run for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Stopping scheduler...\n";
    scheduler.stop();

    std::cout << "Done.\n";
    return 0;
}
