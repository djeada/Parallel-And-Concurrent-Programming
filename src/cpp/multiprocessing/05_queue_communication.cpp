/*
This program demonstrates the use of POSIX message queues to establish
communication between two processes in C++. A message queue allows for safe
inter-process communication by providing a simple way to pass data between
processes. In this example, we create a producer process that generates data and
writes it into the message queue, and a consumer process that reads the data
from the message queue.

When working with multiple processes, it is important to ensure that data is
safely shared between them. Message queues provide a way to achieve this. They
can be used to send data between a producer and a consumer process. The producer
generates data and writes it into the message queue, while the consumer reads
the data from the message queue and processes it. This pattern is useful when
the producer and consumer processes work at different rates or when you want to
separate data generation from data processing.
*/

#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

const char *MQ_NAME = "/my_message_queue";

void producer() {
  srand(time(NULL));

  mqd_t mq = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 0666, nullptr);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 5; ++i) {
    int item = rand() % 10 + 1;
    std::cout << "Producer: Adding " << item << " to the message queue"
              << std::endl;
    mq_send(mq, reinterpret_cast<char *>(&item), sizeof(item), 0);
    usleep((rand() % 500000) + 500000);
  }

  mq_close(mq);
}

void consumer() {
  mqd_t mq = mq_open(MQ_NAME, O_RDONLY);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  int item;
  ssize_t bytesRead;
  while ((bytesRead = mq_receive(mq, reinterpret_cast<char *>(&item),
                                 sizeof(item), nullptr)) > 0) {
    std::cout << "Consumer: Processing " << item << std::endl;
    usleep((rand() % 500000) + 500000);
  }

  mq_close(mq);
}

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // Child process (Consumer)
    consumer();
    exit(EXIT_SUCCESS);
  } else {
    // Parent process (Producer)
    producer();

    // Wait for the child process to finish
    wait(NULL);
  }

  // Cleanup
  mq_unlink(MQ_NAME);

  return 0;
}

// g++ -o main main.cpp -lrt
