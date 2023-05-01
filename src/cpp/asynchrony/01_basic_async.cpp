/*
This program demonstrates the difference between synchronous and asynchronous
approaches to handling I/O-bound tasks using C++. The synchronous version
runs the tasks sequentially, while the asynchronous version takes advantage of
std::async to run the tasks concurrently. This allows for better resource
utilization and improved performance when dealing with I/O-bound tasks.

The program contains two functions, sync_main() and async_main(), which
demonstrate the synchronous and asynchronous approaches, respectively. The
execution time for each approach is measured and displayed to show the
performance difference.
*/

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void sync_task(int task_id, int sleep_time) {
  std::cout << "Task " << task_id << " started." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
  std::cout << "Task " << task_id << " finished." << std::endl;
}

void sync_main() {
  auto start_time = std::chrono::steady_clock::now();

  sync_task(1, 2);
  sync_task(2, 1);

  auto elapsed_time = std::chrono::steady_clock::now() - start_time;
  std::cout
      << "Synchronous execution took "
      << std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count()
      << " seconds." << std::endl;
}

void async_task(int task_id, int sleep_time) {
  std::cout << "Task " << task_id << " started." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
  std::cout << "Task " << task_id << " finished." << std::endl;
}

void async_main() {
  auto start_time = std::chrono::steady_clock::now();

  auto future1 = std::async(std::launch::async, async_task, 1, 2);
  auto future2 = std::async(std::launch::async, async_task, 2, 1);

  future1.wait();
  future2.wait();

  auto elapsed_time = std::chrono::steady_clock::now() - start_time;
  std::cout
      << "Asynchronous execution took "
      << std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count()
      << " seconds." << std::endl;
}

int main() {
  sync_main();
  async_main();

  return 0;
}
