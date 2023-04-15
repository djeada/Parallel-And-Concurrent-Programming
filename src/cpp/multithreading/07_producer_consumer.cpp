#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Semaphore {
  std::mutex mutex;
  std::condition_variable condition_;
  unsigned long count = 0;

public:
  void release() {
    std::lock_guard<decltype(mutex)> lock(mutex);
    ++count;
    condition_.notify_one();
  }

  void acquire() {
    std::unique_lock<decltype(mutex)> lock(mutex);
    while (count == 0u) {
      condition_.wait(lock);
    }
    --count;
  }

  auto try_acquire() -> bool {
    std::lock_guard<decltype(mutex)> lock(mutex);
    if (count != 0u) {
      --count;
      return true;
    }
    return false;
  }
};

Semaphore semaphore;
int item = -1;

void consumer() {
  std::cout << "The begining of consumer function" << std::endl;
  semaphore.acquire();
  std::cout << "consumer got notified: the value is " << item << std::endl;
  std::cout << "The end of consumer function" << std::endl;
}

void producer() {
  std::cout << "The begining of producer function" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  item = 100;

  std::cout << "producer send's the value " << item << std::endl;
  semaphore.release();
  std::cout << "The end of producer function" << std::endl;
}

auto main() -> int {

  std::vector<std::thread> threads;

  threads.emplace_back(std::thread(producer));
  threads.emplace_back(std::thread(consumer));

  for (auto &thread : threads) {
    thread.join();
  }

  std::cout << "The end of main function" << std::endl;
  return 0;
}
