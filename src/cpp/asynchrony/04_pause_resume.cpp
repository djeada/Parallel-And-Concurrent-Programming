/*
This script demonstrates how to pause and resume function execution using
std::thread, std::mutex, and std::condition_variable in C++. It consists of two main parts:
1. A long-running function that periodically prints its progress.
2. A button handler that listens for user input to toggle pause and resume.

The script shows how to use C++ threading and synchronization primitives to
effectively pause and resume the execution of the long-running function.
*/

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool paused = false;

void long_function() {
    int i = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !paused; });

        std::cout << "Executing step " << i << std::endl;
        i++;

        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void button_handler() {
    while (true) {
        std::cin.get();

        std::unique_lock<std::mutex> lock(mtx);
        paused = !paused;
        cv.notify_all();

        std::string state = paused ? "paused" : "resumed";
        std::cout << "Function " << state << std::endl;
    }
}

int main() {
    std::thread long_function_thread(long_function);
    std::thread button_handler_thread(button_handler);

    long_function_thread.join();
    button_handler_thread.join();

    return 0;
}
