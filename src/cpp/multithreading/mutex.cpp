#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mutex;

void foo(int id) {
  mutex.lock(); // comment out this line of code to test what happens without
                // the mutex
  for (int i = 0; i < 100; i++) {
    std::cout << "Thread id: " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  mutex.unlock(); // comment out this line of code to test what happens without
                  // the mutex
}

auto main() -> int {
  std::cout << "MAIN FUNCTION STARTS" << std::endl;

  std::vector<std::unique_ptr<std::thread>> threads;

  threads.reserve(4);
  for (int i = 0; i < 4; i++) {
    threads.emplace_back(std::make_unique<std::thread>(foo, i));
  }

  for (auto &thread : threads) {
    thread->join();
  }

  std::cout << "THE END" << std::endl;

  return 0;
}
