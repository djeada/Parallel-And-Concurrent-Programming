/*
This program demonstrates how to share an array between multiple processes using
shared memory in C++. We use the mmap system call to create a shared memory
object and allow the processes to access it directly.

The program creates two child processes that operate on the shared array. One
child process increments the elements of the array, and the other decrements
them. The parent process creates the shared array and starts the child
processes. This example demonstrates the concepts of inter-process communication
and sharing data between processes in C++ using shared memory.
*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr size_t ARRAY_SIZE = 3;

void increment_array(int *shared_array) {
  while (true) {
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
      shared_array[i] += 1;
    }
    std::cout << "Array incremented: ";
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
      std::cout << shared_array[i] << ' ';
    }
    std::cout << std::endl;
    sleep(1);
  }
}

void decrement_array(int *shared_array) {
  while (true) {
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
      shared_array[i] -= 1;
    }
    std::cout << "Array decremented: ";
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
      std::cout << shared_array[i] << ' ';
    }
    std::cout << std::endl;
    sleep(1);
  }
}

int main() {
  int *shared_array = static_cast<int *>(
      mmap(NULL, sizeof(int) * ARRAY_SIZE, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, -1, 0));

  for (size_t i = 0; i < ARRAY_SIZE; ++i) {
    shared_array[i] = 0;
  }

  pid_t pid1 = fork();
  if (pid1 == 0) {
    increment_array(shared_array);
    exit(0);
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    decrement_array(shared_array);
    exit(0);
  }

  int status;
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);

  munmap(shared_array, sizeof(int) * ARRAY_SIZE);

  return 0;
}
