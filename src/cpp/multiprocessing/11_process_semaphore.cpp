#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <random>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

// The shared resource is printing to the console
void shared_resource(int process_id) {
  std::cout << "Process " << process_id << " is using the shared resource"
            << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 500));
  std::cout << "Process " << process_id << " is done using the shared resource"
            << std::endl;
}

void worker(sem_t *semaphore, int process_id) {
  std::cout << "Process " << process_id << " is waiting for the semaphore"
            << std::endl;
  sem_wait(semaphore);
  std::cout << "Process " << process_id << " acquired the semaphore"
            << std::endl;
  shared_resource(process_id);
  std::cout << "Process " << process_id << " released the semaphore"
            << std::endl;
  sem_post(semaphore);
}

int main() {
  const int num_processes = 10;
  const int max_concurrent = 3;

  sem_t *semaphore = sem_open("/semaphore_example", O_CREAT, S_IRUSR | S_IWUSR,
                              max_concurrent);

  std::vector<pid_t> children;

  for (int i = 0; i < num_processes; ++i) {
    pid_t pid = fork();

    if (pid == 0) { // child process
      worker(semaphore, i);
      exit(0);
    } else if (pid > 0) { // parent process
      children.push_back(pid);
    } else {
      std::cerr << "Fork failed" << std::endl;
      return 1;
    }
  }

  // Wait for all child processes to finish
  for (const auto &child_pid : children) {
    waitpid(child_pid, nullptr, 0);
  }

  sem_close(semaphore);
  sem_unlink("/semaphore_example");

  return 0;
}
