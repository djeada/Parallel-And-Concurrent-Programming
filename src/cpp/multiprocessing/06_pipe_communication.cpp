/*
This program demonstrates the use of fork() to create multiple processes and
pipes to establish communication between them in C++. A pipe allows for safe
inter-process communication by providing a simple way to pass data between
processes. In this example, we create a producer process that generates data and
writes it into the pipe, and a consumer process that reads the data from the
pipe.

When working with multiple processes, it is important to ensure that data is
safely shared between them. Pipes provide a way to achieve this. They can be
used to send data between a producer and a consumer process. The producer
generates data and writes it into the pipe, while the consumer reads the data
from the pipe and processes it. This pattern is useful when the producer and
consumer processes work at different rates or when you want to separate data
generation from data processing.
*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void producer(int pipe_fd) {
  srand(time(NULL));
  for (int i = 0; i < 5; ++i) {
    int item = rand() % 10 + 1;
    std::cout << "Producer: Adding " << item << " to the pipe" << std::endl;
    write(pipe_fd, &item, sizeof(item));
    usleep((rand() % 500000) + 500000);
  }
  close(pipe_fd);
}

void consumer(int pipe_fd) {
  int item;
  ssize_t bytesRead;
  while ((bytesRead = read(pipe_fd, &item, sizeof(item))) > 0) {
    std::cout << "Consumer: Processing " << item << std::endl;
    usleep((rand() % 500000) + 500000);
  }
  close(pipe_fd);
}

int main() {
  int pipe_fd[2];

  if (pipe(pipe_fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // Child process (Consumer)
    close(pipe_fd[1]);
    consumer(pipe_fd[0]);
    exit(EXIT_SUCCESS);
  } else {
    // Parent process (Producer)
    close(pipe_fd[0]);
    producer(pipe_fd[1]);

    // Wait for the child process to finish
    wait(NULL);
  }

  return 0;
}
