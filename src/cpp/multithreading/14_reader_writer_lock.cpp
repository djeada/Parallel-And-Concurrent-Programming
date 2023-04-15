#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedData {
public:
  void read(int thread_id) {
    std::unique_lock<std::mutex> reader_lock(readers_mutex);
    ++readers;
    if (readers == 1) {
      data_mutex.lock();
    }
    reader_lock.unlock();

    std::cout << "Reader " << thread_id << " reads data: " << data << std::endl;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(rand() % 1000 + 500)); // Simulate some work

    reader_lock.lock();
    --readers;
    if (readers == 0) {
      data_mutex.unlock();
    }
  }

  void write(int thread_id) {
    std::unique_lock<std::shared_mutex> lock(data_mutex);
    std::cout << "Writer " << thread_id << " is writing..." << std::endl;
    ++data;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(rand() % 1000 + 500)); // Simulate some work
    std::cout << "Writer " << thread_id
              << " finished writing. New data: " << data << std::endl;
  }

private:
  int data = 0;
  int readers = 0;
  std::mutex readers_mutex;
  std::shared_mutex data_mutex;
};

void reader(SharedData &shared_data, int thread_id) {
  while (true) {
    shared_data.read(thread_id);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(rand() % 1000 + 1000));
  }
}

void writer(SharedData &shared_data, int thread_id) {
  while (true) {
    shared_data.write(thread_id);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(rand() % 1000 + 1000));
  }
}

int main() {
  SharedData shared_data;

  int num_readers = 3;
  int num_writers = 2;

  std::vector<std::thread> threads;

  for (int i = 0; i < num_readers; ++i) {
    threads.emplace_back(reader, std::ref(shared_data), i);
  }

  for (int i = 0; i < num_writers; ++i) {
    threads.emplace_back(writer, std::ref(shared_data), i);
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}
