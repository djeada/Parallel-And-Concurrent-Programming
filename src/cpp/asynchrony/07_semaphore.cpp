#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

class Semaphore {
public:
  explicit Semaphore(int count) : count(count) {}

  void acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]() { return count > 0; });
    --count;
  }

  void release() {
    std::unique_lock<std::mutex> lock(mtx);
    ++count;
    cv.notify_one();
  }

private:
  std::mutex mtx;
  std::condition_variable cv;
  int count;
};

Semaphore resource_semaphore(3);

void limited_resource(int task_id) {
  resource_semaphore.acquire();
  try {
    std::cout << "Task " << task_id << " is using the limited resource.\n";

    // Improved random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(500, 2500);

    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));

    std::cout << "Task " << task_id << " is done using the limited resource.\n";
  } catch (const std::exception &e) {
    std::cerr << "Exception in task " << task_id << ": " << e.what() << '\n';
  }
  resource_semaphore.release();
}

int main() {
  std::vector<std::future<void>> tasks;

  for (int i = 0; i < 10; ++i) {
    tasks.push_back(std::async(std::launch::async, limited_resource, i));
  }

  for (auto &task : tasks) {
    try {
      task.get(); // Ensure exceptions are propagated
    } catch (const std::exception &e) {
      std::cerr << "Exception in main: " << e.what() << '\n';
    }
  }

  return 0;
}
