#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int slow_square_sync(int x) {
  std::cout << "Starting slow square computation for " << x << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  int result = x * x;
  std::cout << "Finished slow square computation for " << x << std::endl;
  return result;
}

int slow_square_async(int x) {
  std::cout << "Starting slow square computation for " << x << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  int result = x * x;
  std::cout << "Finished slow square computation for " << x << std::endl;
  return result;
}

void synchronous_execution() {
  auto start_time = std::chrono::steady_clock::now();

  int result1 = slow_square_sync(3);
  int result2 = slow_square_sync(4);

  auto elapsed_time = std::chrono::steady_clock::now() - start_time;
  auto elapsed_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count();

  std::cout << "\nSynchronous execution took " << elapsed_seconds << " seconds."
            << std::endl;
  std::cout << "Results: " << result1 << ", " << result2 << std::endl;
}

void asynchronous_execution() {
  auto start_time = std::chrono::steady_clock::now();

  auto future1 = std::async(std::launch::async, slow_square_async, 3);
  auto future2 = std::async(std::launch::async, slow_square_async, 4);

  int result1 = future1.get();
  int result2 = future2.get();

  auto elapsed_time = std::chrono::steady_clock::now() - start_time;
  auto elapsed_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count();

  std::cout << "\nAsynchronous execution took " << elapsed_seconds
            << " seconds." << std::endl;
  std::cout << "Results: " << result1 << ", " << result2 << std::endl;
}

int main() {
  synchronous_execution();
  asynchronous_execution();

  return 0;
}
