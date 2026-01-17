/**
 * Pipe Communication Between Processes
 *
 * This example demonstrates inter-process communication using
 * anonymous pipes created with pipe().
 *
 * Key concepts:
 * - pipe() creates a unidirectional data channel
 * - pipe_fd[0] is the read end, pipe_fd[1] is the write end
 * - Fork inherits the pipe file descriptors
 * - Close unused ends to avoid resource leaks and hangs
 * - read() blocks until data is available or write end is closed
 *
 * Pipes are simpler than message queues but:
 * - Only work between related processes (parent/child)
 * - Are unidirectional (need two pipes for bidirectional)
 * - Don't have message boundaries (stream of bytes)
 */

#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void producer(int write_fd) {
    for (int i = 1; i <= 5; ++i) {
        std::cout << "Producer: sending " << i << "\n";
        write(write_fd, &i, sizeof(i));
        usleep(200'000);  // 200ms
    }
    close(write_fd);
}

void consumer(int read_fd) {
    int item;
    while (read(read_fd, &item, sizeof(item)) > 0) {
        std::cout << "Consumer: received " << item << "\n";
        usleep(100'000);
    }
    close(read_fd);
}

int main() {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child: consumer
        close(pipe_fd[1]);  // Close unused write end
        consumer(pipe_fd[0]);
        _exit(0);
    } else {
        // Parent: producer
        close(pipe_fd[0]);  // Close unused read end
        producer(pipe_fd[1]);
        wait(nullptr);
    }

    std::cout << "Done\n";
    return 0;
}
