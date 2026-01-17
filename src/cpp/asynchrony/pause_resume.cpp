/**
 * Pausable Asynchronous Tasks
 *
 * This example demonstrates how to pause and resume async tasks
 * using condition variables and atomic flags.
 *
 * Key concepts:
 * - std::atomic<bool> for thread-safe flag communication
 * - std::condition_variable to block until a condition is met
 * - std::promise/std::future for signaling task completion
 * - Combining these primitives enables cooperative task control
 *
 * This pattern is useful for tasks that should be interruptible
 * or controllable by external events.
 */

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> paused{false};
std::atomic<bool> running{true};

void pausable_task(std::promise<void>&& done_signal) {
    int step = 0;
    while (running) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !paused || !running; });

        if (!running) break;

        std::cout << "Executing step " << step++ << "\n";
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    done_signal.set_value();
}

int main() {
    std::promise<void> done_promise;
    std::future<void> done_future = done_promise.get_future();

    auto task = std::async(std::launch::async, pausable_task,
                           std::move(done_promise));

    std::cout << "Task running. Commands: 'p' = pause/resume, 'q' = quit\n";

    char cmd;
    while (std::cin >> cmd) {
        if (cmd == 'p') {
            paused = !paused;
            cv.notify_all();
            std::cout << (paused ? "Paused" : "Resumed") << "\n";
        } else if (cmd == 'q') {
            running = false;
            paused = false;
            cv.notify_all();
            break;
        }
    }

    done_future.wait();
    std::cout << "Task completed\n";

    return 0;
}
