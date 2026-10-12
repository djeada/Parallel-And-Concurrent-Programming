/**
 * Pausable Asynchronous Tasks
 *
 * This example demonstrates how to pause and resume async tasks
 * using a condition variable and mutex-protected flags.
 *
 * Key concepts:
 * - Mutex-protected flags for task control
 * - std::condition_variable to block until a condition is met
 * - std::promise/std::future for signaling task completion
 * - Combining these primitives enables cooperative task control
 *
 * This pattern is useful for tasks that should be interruptible
 * or controllable by external events.
 */

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool paused = false;  // Protected by mtx
bool running = true;  // Protected by mtx

void pausable_task(std::promise<void>&& done_signal) {
    int step = 0;
    std::unique_lock<std::mutex> lock(mtx);
    while (running) {
        cv.wait(lock, [] { return !paused || !running; });

        if (!running) break;

        std::cout << "Executing step " << step++ << "\n";
        cv.wait_for(lock, std::chrono::seconds(1), [] {
            return paused || !running;
        });
    }
    lock.unlock();
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
            bool is_paused;
            {
                std::lock_guard<std::mutex> lock(mtx);
                paused = !paused;
                is_paused = paused;
            }
            cv.notify_all();
            std::cout << (is_paused ? "Paused" : "Resumed") << "\n";
        } else if (cmd == 'q') {
            break;
        }
    }

    // EOF and 'q' both stop the worker, including while it is paused.
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
    }
    cv.notify_all();

    done_future.get();
    task.get();
    std::cout << "Task completed\n";

    return 0;
}
