/*
This program demonstrates the use of a barrier with multiprocessing in C++.
A barrier is a synchronization primitive that allows multiple processes to
wait for each other at a certain point in their execution. When all processes
reach the barrier, they are allowed to continue executing.

In this example, we create a number of worker processes that perform some work,
and then wait at a barrier. Once all worker processes have reached the barrier,
they are allowed to continue and finish their execution. We use POSIX semaphores
to implement the barrier.

Note that barriers can be useful in cases where the processes need to work in
lockstep or to ensure that certain resources are not accessed before all
processes are ready.
*/

#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>

void worker(sem_t *barrier, int worker_id) {
  // Simulate some work
  int sleep_time = rand() % 3 + 1;
  sleep(sleep_time);
  std::cout << "Worker " << worker_id
            << " finished initial work and is waiting at the barrier...\n";

  // Wait at the barrier
  sem_wait(barrier);
  sem_post(barrier);

  std::cout << "Worker " << worker_id
            << " passed the barrier and is continuing execution...\n";
}

int main() {
  srand(time(0));
  const int num_workers = 5;

  // Create a barrier with num_workers as the number of participants
  sem_t *barrier = sem_open("/barrier", O_CREAT | O_EXCL, 0644, num_workers);
  if (barrier == SEM_FAILED) {
    perror("sem_open");
    return 1;
  }

  // Start worker processes
  for (int i = 0; i < num_workers; i++) {
    pid_t pid = fork();
    if (pid == 0) { // Child process
      worker(barrier, i);
      return 0;
    } else if (pid < 0) { // Fork failed
      perror("fork");
      return 1;
    }
  }

  // Wait for worker processes to finish
  for (int i = 0; i < num_workers; i++) {
    wait(NULL);
  }

  // Cleanup
  sem_close(barrier);
  sem_unlink("/barrier");

  return 0;
}
