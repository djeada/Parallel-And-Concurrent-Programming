/**
 * POSIX Message Queues
 *
 * This example demonstrates inter-process communication using
 * POSIX message queues.
 *
 * Key concepts:
 * - Message queues provide asynchronous IPC with persistence
 * - mq_open() creates/opens a named queue visible to all processes
 * - mq_send() adds messages; mq_receive() retrieves them
 * - Queue persists until explicitly unlinked with mq_unlink()
 * - Supports priority ordering of messages
 *
 * Message queues are ideal for:
 * - Producer-consumer patterns across processes
 * - Decoupling sender and receiver timing
 * - Buffering data when processes work at different speeds
 *
 * Build: g++ -std=c++17 -o message_queue message_queue.cpp -lrt
 */

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr char QUEUE_NAME[] = "/example_queue";

void producer() {
    mq_attr attr{};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);

    mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, 0666, &attr);
    if (mq == -1) {
        perror("mq_open");
        _exit(1);
    }

    for (int i = 1; i <= 5; ++i) {
        std::cout << "Producer: sending " << i << "\n";
        mq_send(mq, reinterpret_cast<char*>(&i), sizeof(i), 0);
        usleep(200'000);  // 200ms
    }

    mq_close(mq);
}

void consumer() {
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == -1) {
        perror("mq_open");
        _exit(1);
    }

    mq_attr attr;
    mq_getattr(mq, &attr);

    int item;
    for (int i = 0; i < 5; ++i) {
        ssize_t bytes = mq_receive(mq, reinterpret_cast<char*>(&item),
                                   attr.mq_msgsize, nullptr);
        if (bytes > 0) {
            std::cout << "Consumer: received " << item << "\n";
        }
    }

    mq_close(mq);
}

int main() {
    // Clean up any leftover queue
    mq_unlink(QUEUE_NAME);

    pid_t pid = fork();

    if (pid == 0) {
        // Give producer time to create queue
        usleep(100'000);
        consumer();
        _exit(0);
    } else {
        producer();
        waitpid(pid, nullptr, 0);
    }

    mq_unlink(QUEUE_NAME);
    std::cout << "Done\n";

    return 0;
}
