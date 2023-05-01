
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;

void foo(int id) {
  mtx.lock(); // comment out this line of code to test what happens without the
              // mutex
  for (int i = 0; i < 100; ++i) {
    std::cout << "Thread id: " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  mtx.unlock(); // comment out this line of code to test what happens without
                // the mutex
}

int main() {
  std::cout << "MAIN FUNCTION STARTS" << std::endl;
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.emplace_back(foo, i);
  }
  for (auto &thread : threads) {
    thread.join();
  }
  std::cout << "THE END" << std::endl;
  return 0;
}
