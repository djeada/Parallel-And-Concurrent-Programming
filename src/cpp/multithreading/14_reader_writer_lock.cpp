#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class SharedData {
public:
  void read(int thread_id) {
    {
      std::unique_lock<std::mutex> lock(readers_mutex);
      ++readers;
      if (readers == 1) {
        data_mutex.lock();
      }
    }

    std::cout << "Reader " << thread_id << " reads data: " << data << std::endl;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(random_duration(500, 1000)));

    {
      std::unique_lock<std::mutex> lock(readers_mutex);
      --readers;
      if (readers == 0) {
        data_mutex.unlock();
      }
    }
  }

  void write(int thread_id) {
    std::unique_lock<std::mutex> lock(data_mutex);
    std::cout << "Writer " << thread_id << " is writing..." << std::endl;
    ++data;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(random_duration(500, 1000)));
    std::cout << "Writer " << thread_id
              << " finished writing. New data: " << data << std::endl;
  }

  int random_duration(int min, int max) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
  }

private:
  int data = 0;
  std::mutex data_mutex;
  int readers = 0;
  std::mutex readers_mutex;
};

void reader(SharedData &shared_data, int thread_id) {
  while (true) {
    shared_data.read(thread_id);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(shared_data.random_duration(1000, 2000)));
  }
}

void writer(SharedData &shared_data, int thread_id) {
  while (true) {
    shared_data.write(thread_id);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(shared_data.random_duration(1000, 2000)));
  }
}

int main() {
  SharedData shared_data;

  const int num_readers = 3;
  const int num_writers = 2;

  std::vector<std::thread> threads;

  for (int i = 0; i < num_readers; ++i) {
    threads.emplace_back(reader, std::ref(shared_data), i);
  }

  for (int i = 0; i < num_writers; ++i) {
    threads.emplace_back(writer, std::ref(shared_data), i);
  }

  for (auto &thread : threads) {
    thread.join();
  }

  return 0;
}
