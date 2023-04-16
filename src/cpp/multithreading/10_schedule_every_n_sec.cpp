#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

class Scheduler {
public:
    Scheduler(int n, std::function<void()> function)
        : flag(true), interval(n), task(std::move(function)) {
        scheduler_thread = std::thread([this] {
            while (flag) {
                task();
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }
        });
    }

    ~Scheduler() {
        stop();
    }

    void stop() {
        flag = false;
        if (scheduler_thread.joinable()) {
            scheduler_thread.join();
        }
    }

private:
    std::atomic<bool> flag;
    int interval;
    std::function<void()> task;
    std::thread scheduler_thread;
};

void print_message() {
    std::cout << "It works" << std::endl;
}

int main() {
    Scheduler scheduler(1, print_message);

    // wait 10 seconds and stop the scheduler
    std::this_thread::sleep_for(std::chrono::seconds(10));
    scheduler.stop();

    return 0;
}
