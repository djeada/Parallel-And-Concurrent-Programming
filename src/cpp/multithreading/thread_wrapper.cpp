/**
 * Thread Wrapper Class
 *
 * This example demonstrates wrapping std::thread in a custom class to
 * provide RAII-style thread management.
 *
 * Key concepts:
 * - RAII: Resource Acquisition Is Initialization
 * - Thread is joined automatically in destructor
 * - Callable can be any function, lambda, or functor
 * - Custom logic can wrap the actual function execution
 *
 * This pattern is useful for adding logging, timing, or other
 * cross-cutting concerns to thread execution.
 */

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class ManagedThread {
public:
    explicit ManagedThread(std::function<void()> func)
        : func_(std::move(func)),
          thread_(&ManagedThread::run, this) {}

    ~ManagedThread() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    // Prevent copying
    ManagedThread(const ManagedThread&) = delete;
    ManagedThread& operator=(const ManagedThread&) = delete;

private:
    void run() {
        std::cout << "Thread starting...\n";
        func_();
        std::cout << "Thread finished.\n";
    }

    std::function<void()> func_;
    std::thread thread_;
};

void long_running_task() {
    std::cout << "Task: going to sleep...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Task: woke up!\n";
}

int main() {
    std::cout << "Creating managed threads...\n";

    {
        // Threads are automatically joined when they go out of scope
        ManagedThread thread1(long_running_task);

        ManagedThread thread2([] {
            std::cout << "Lambda task running!\n";
        });

        std::cout << "Main thread continues...\n";

        // Threads join automatically when destructors run
    }

    std::cout << "All threads completed.\n";
    return 0;
}
