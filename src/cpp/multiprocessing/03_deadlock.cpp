#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

void process_a(sem_t *lock_a, sem_t *lock_b) {
  sem_wait(lock_a);
  std::cout << "Process A acquired lock_a" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Process A trying to acquire lock_b" << std::endl;
  sem_wait(lock_b);
  std::cout << "Process A acquired lock_b" << std::endl;
  sem_post(lock_b);
  sem_post(lock_a);
}

void process_b(sem_t *lock_a, sem_t *lock_b) {
  sem_wait(lock_b);
  std::cout << "Process B acquired lock_b" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Process B trying to acquire lock_a" << std::endl;
  sem_wait(lock_a);
  std::cout << "Process B acquired lock_a" << std::endl;
  sem_post(lock_a);
  sem_post(lock_b);
}

int main() {
  sem_t *lock_a = sem_open("/lock_a", O_CREAT, 0644, 1);
  sem_t *lock_b = sem_open("/lock_b", O_CREAT, 0644, 1);

  int pid = fork();

  if (pid == 0) { // child process
    process_a(lock_a, lock_b);
  } else { // parent process
    process_b(lock_a, lock_b);
    wait(nullptr);
  }

  sem_close(lock_a);
  sem_close(lock_b);
  sem_unlink("/lock_a");
  sem_unlink("/lock_b");

  std::cout << "Process finished" << std::endl;

  return 0;
}
