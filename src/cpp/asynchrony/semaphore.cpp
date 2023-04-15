#include <chrono>
#include <coroutine>
#include <iostream>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <thread>
#include <vector>

// A simple task class
class Task {
public:
  std::coroutine_handle<> handle;

  Task(std::coroutine_handle<> handle) : handle(handle) {}
  ~Task() { handle.destroy(); }
  void operator()() { handle(); }
};

// A simple task promise
class TaskPromise {
public:
  std::suspend_never initial_suspend() { return {}; }
  std::suspend_never final_suspend() noexcept { return {}; }
  Task get_return_object() {
    return {std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }
  void unhandled_exception() { std::terminate(); }
  void return_void() {}
};

// A simple task type
using task = Task;

// Coroutine declaration
task resource_limited_operation(int id);
task worker(std::shared_mutex &mutex, int id);
task task_generator();

// Coroutine definitions
task resource_limited_operation(int id) {
  std::cout << "Worker " << id
            << " is entering the resource-limited operation\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1500 + 500));
  std::cout << "Worker " << id
            << " is leaving the resource-limited operation\n";
  co_return;
}

task worker(std::shared_mutex &mutex, int id) {
  for (int i = 0; i < 3; ++i) {
    {
      std::shared_lock lock(mutex, 2); // Limit concurrency to 2
      co_await resource_limited_operation(id);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 500));
  }
  std::cout << "End of worker " << id << " function\n";
  co_return;
}

task task_generator() {
  std::shared_mutex mutex;
  int n_workers = 5;
  std::vector<std::jthread> threads;

  for (int i = 0; i < n_workers; ++i) {
    threads.emplace_back(worker, std::ref(mutex), i);
  }

  for (auto &thread : threads) {
    thread.join();
  }

  co_return;
}

int main() {
  task_generator()();
  std::cout << "The End\n";
  return 0;
}
