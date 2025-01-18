/*
This script demonstrates how to pause and resume function execution using
std::thread, std::mutex, and std::condition_variable in C++. It consists of two
main parts:
1. A long-running function that periodically prints its progress.
2. A button handler that listens for user input to toggle pause and resume.

The script shows how to use C++ threading and synchronization primitives to
effectively pause and resume the execution of the long-running function.
*/
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> paused(false);
std::atomic<bool> running(true);

void long_function(std::promise<void>&& exit_signal) {
  int i = 0;
  while (running) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return !paused; });

    std::cout << "Executing step " << i << std::endl;
    i++;

    lock.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit_signal.set_value();
}

void button_handler() {
  while (running) {
    std::cin.get();

    std::unique_lock<std::mutex> lock(mtx);
    paused = !paused;
    cv.notify_all();

    std::string state = paused ? "paused" : "resumed";
    std::cout << "Function " << state << std::endl;
  }
}

int main() {
  std::promise<void> exit_signal;
  std::future<void> future_obj = exit_signal.get_future();
  auto long_function_thread = std::async(std::launch::async, long_function, std::move(exit_signal));
  std::thread button_handler_thread(button_handler);

  std::cout << "Press Enter to pause/resume, or type 'exit' to quit." << std::endl;
  
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    if (input == "exit") {
      running = false;
      paused = false;
      cv.notify_all();
      break;
    }
  }

  future_obj.wait();
  button_handler_thread.join();

  return 0;
}
