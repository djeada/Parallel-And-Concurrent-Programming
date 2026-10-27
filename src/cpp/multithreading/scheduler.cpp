/**
 * Modern C++20: Cancellable Periodic Work
 *
 * std::jthread owns the worker; condition_variable_any provides a timed wait
 * that wakes on a stop request. Shutdown need not wait for the next interval.
 * The task itself must return before shutdown can complete.
 *
 * Compile: g++ -std=c++20 -pthread scheduler.cpp
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>

class Scheduler {
public:
    Scheduler(int interval_seconds, std::function<void()> task)
        : interval_(interval_seconds), task_(std::move(task)),
          worker_([this](std::stop_token stop) {
              while (!stop.stop_requested()) {
                  task_();
                  std::unique_lock lock(mutex_);
                  wake_.wait_for(lock, stop, interval_, [] { return false; });
              }
          }) {}

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    // Called by the owning thread, outside the scheduled task.
    void stop() {
        worker_.request_stop();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

private:
    std::chrono::seconds interval_;
    std::function<void()> task_;
    std::mutex mutex_;
    std::condition_variable_any wake_;
    // Members are destroyed in reverse order: join before destroying state.
    std::jthread worker_;
};

void print_heartbeat() {
    std::cout << "Heartbeat: still alive!\n";
}

int main() {
    std::cout << "Starting scheduler (runs every 1 second)...\n";
    Scheduler scheduler(1, print_heartbeat);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Stopping scheduler...\n";
    scheduler.stop();
    std::cout << "Done.\n";
    return 0;
}
