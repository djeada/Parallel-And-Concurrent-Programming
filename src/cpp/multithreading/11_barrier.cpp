#include <barrier>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex print_mutex;

void worker(std::barrier<> &sync_point, int thread_id) {
  {
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << "Thread " << thread_id << " is waiting at the barrier."
              << std::endl;
  }

  sync_point.arrive_and_wait();

  {
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << "Thread " << thread_id << " has passed the barrier."
              << std::endl;
  }
}

int main() {
  const int num_threads = 4;
  std::barrier<> sync_point(num_threads);
  std::vector<std::thread> threads;

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker, std::ref(sync_point), i);
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}
