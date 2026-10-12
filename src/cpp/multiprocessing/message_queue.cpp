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

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

bool producer(mqd_t mq) {
    for (int i = 1; i <= 5; ++i) {
        std::cout << "Producer: sending " << i << "\n";
        while (mq_send(mq, reinterpret_cast<const char*>(&i), sizeof(i), 0) < 0) {
            if (errno == EINTR) continue;
            perror("mq_send");
            return false;
        }
        usleep(200'000);
    }
    return true;
}

bool consumer(mqd_t mq) {
    for (int i = 0; i < 5; ++i) {
        int item;
        ssize_t bytes;
        do {
            bytes = mq_receive(mq, reinterpret_cast<char*>(&item), sizeof(item), nullptr);
        } while (bytes < 0 && errno == EINTR);
        if (bytes != sizeof(item)) {
            if (bytes < 0) perror("mq_receive");
            else std::cerr << "Unexpected message size\n";
            return false;
        }
        std::cout << "Consumer: received " << item << "\n";
    }
    return true;
}

int main() {
    // Create before fork; both processes inherit the ready queue descriptor.
    // A per-process name allows concurrent runs without deleting another queue.
    std::string name = "/example_queue_" + std::to_string(getpid());
    mq_attr attr{};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);
    mqd_t mq = mq_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600, &attr);
    if (mq == static_cast<mqd_t>(-1)) { perror("mq_open"); return 1; }
    // Existing descriptors remain valid; the queue is removed at last close.
    if (mq_unlink(name.c_str()) < 0) {
        perror("mq_unlink");
        mq_close(mq);
        return 1;
    }

    std::cout.flush();
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); mq_close(mq); return 1; }
    if (pid == 0) {
        bool ok = consumer(mq);
        mq_close(mq);
        std::cout.flush();
        _exit(ok ? 0 : 1);
    }

    bool ok = producer(mq);
    mq_close(mq);
    if (!ok) kill(pid, SIGTERM);  // Consumer must not wait for missing messages.
    int status;
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR) continue;
        perror("waitpid");
        return 1;
    }
    if (!ok || !WIFEXITED(status) || WEXITSTATUS(status) != 0) return 1;
    std::cout << "Done\n";
    return 0;
}
