#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

thread_local int thread_local_data = 0;

std::mutex print_mutex;

void worker(int thread_id) {
  thread_local_data = thread_id;

  // Simulate some work
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::unique_lock<std::mutex> lock(print_mutex);
  std::cout << "Thread " << thread_id
            << " has thread-local data: " << thread_local_data << std::endl;
}

int main() {
  const int num_threads = 5;
  std::vector<std::thread> threads;

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker, i);
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}
