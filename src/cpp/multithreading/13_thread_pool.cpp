#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
      threads.emplace_back([this] {
        while (true) {
          std::unique_lock<std::mutex> lock(queue_mutex);
          condition.wait(lock, [this] { return !tasks.empty() || stop; });

          if (stop && tasks.empty()) {
            break;
          }

          auto task = std::move(tasks.front());
          tasks.pop();
          lock.unlock();
          task();
        }
      });
    }
  }

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args)
      -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> result = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      if (stop) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
      }
      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread &t : threads) {
      t.join();
    }
  }

private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mutex;
  std::condition_variable condition;
  std::atomic<bool> stop{false};
};

void my_function(int i) {
  std::cout << "Task " << i << " started." << std::endl;
  std::this_thread::sleep_for(
      std::chrono::milliseconds(rand() % 1000 + 500)); // Simulate some work
  std::cout << "Task " << i << " finished." << std::endl;
}

int main() {
  ThreadPool thread_pool(4);

  std::vector<std::future<void>> futures;
  for (int i = 0; i < 10; ++i) {
    futures.emplace_back(thread_pool.enqueue(my_function, i));
  }

  for (auto &future : futures) {
    future.wait();
  }

  std::cout << "Main thread finished." << std::endl;
  return 0;
}
