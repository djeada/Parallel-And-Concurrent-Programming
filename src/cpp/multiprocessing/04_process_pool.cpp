#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>
class ProcessPool {
public:
  ProcessPool(size_t num_workers) : num_workers(num_workers) {
    worker_pids.reserve(num_workers);
  }

  void add_task(const std::function<void(int)> &task, int task_id) {
    if (worker_pids.size() >= num_workers) {
      // Wait for a worker process to finish
      int status;
      pid_t pid = wait(&status);
      if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        std::cout << "Task result collected: " << exit_status << std::endl;
      }

      // Remove the finished worker process from the list
      auto it = std::find(worker_pids.begin(), worker_pids.end(), pid);
      if (it != worker_pids.end()) {
        worker_pids.erase(it);
      }
    }

    pid_t pid = fork();

    if (pid == 0) {
      // Child process
      task(task_id);
      exit(task_id * 2);
    } else {
      // Parent process
      worker_pids.push_back(pid);
    }
  }

  ~ProcessPool() {
    // Wait for all worker processes to finish
    while (!worker_pids.empty()) {
      int status;
      pid_t pid = wait(&status);
      if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        std::cout << "Task result collected: " << exit_status << std::endl;
      }
      worker_pids.pop_back();
    }
  }

private:
  size_t num_workers;
  std::vector<pid_t> worker_pids;
};

void worker_task(int task_id) {
  std::cout << "Task " << task_id << " is starting..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Task " << task_id << " is finished." << std::endl;
}

int main() {
  const size_t num_tasks = 10;
  const size_t num_workers = 3;

  ProcessPool pool(num_workers);

  for (size_t i = 0; i < num_tasks; ++i) {
    pool.add_task(worker_task, i);
  }

  return 0;
}
