#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Semaphore {
  std::mutex mutex_;
  std::condition_variable condition_;
  unsigned long count_ = 0;

public:
  void release() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    ++count_;
    condition_.notify_one();
  }

  void acquire() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    while (!count_)
      condition_.wait(lock);
    --count_;
  }

  bool try_acquire() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (count_) {
      --count_;
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

int main() {

  std::vector<std::thread> threads;

  threads.emplace_back(std::thread(producer));
  threads.emplace_back(std::thread(consumer));

  for (auto &thread : threads)
    thread.join();

  std::cout << "The end of main function" << std::endl;
  return 0;
}
