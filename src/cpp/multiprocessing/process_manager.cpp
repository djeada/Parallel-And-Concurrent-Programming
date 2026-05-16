/**
 * Process Manager — Coordinator/Worker Pattern with File Locking
 *
 * Demonstrates a central manager that spawns multiple worker processes,
 * each writing results to a shared file.  File access is protected by
 * fcntl advisory locking to prevent interleaved or lost writes.
 *
 * Key concepts:
 *   - Manager pattern: coordinator spawns workers, waits for all
 *   - Workers run independently with isolated memory (separate processes)
 *   - fcntl F_SETLKW: blocking write lock on the whole file
 *   - O_APPEND + single write() per record: atomic append on Linux
 *   - File locking with fcntl is advisory — all processes must
 *     cooperate and acquire the lock before writing
 *
 * Why not std::ofstream without locking?
 *   std::ofstream << may issue multiple write() calls for one <<
 *   expression, and two processes can interleave them, producing
 *   corrupted records.  See shared_array.cpp for an unsynced example.
 *
 * Compile:
 *   g++ -std=c++17 -o process_manager process_manager.cpp
 */

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

static bool lock_file(int fd) {
    struct flock fl{};
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;  // 0 = entire file
    return fcntl(fd, F_SETLKW, &fl) == 0;
}

static bool unlock_file(int fd) {
    struct flock fl{};
    fl.l_type   = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    return fcntl(fd, F_SETLK, &fl) == 0;
}

class ProcessManager {
public:
    explicit ProcessManager(int num_workers, const char* output_file)
        : num_workers_(num_workers), output_file_(output_file) {}

    void run() {
        // Truncate/create output file before spawning workers
        int fd = open(output_file_, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); return; }
        close(fd);

        std::vector<pid_t> pids;
        for (int i = 0; i < num_workers_; ++i) {
            pid_t pid = fork();
            if (pid < 0) { perror("fork"); continue; }
            if (pid == 0) {
                worker(i);
                _exit(0);
            }
            pids.push_back(pid);
        }

        for (pid_t pid : pids) {
            waitpid(pid, nullptr, 0);
        }
    }

    void print_results() const {
        std::cout << "Results from workers:\n";
        int fd = open(output_file_, O_RDONLY);
        if (fd < 0) { perror("open"); return; }
        char buf[256];
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            std::cout << buf;
        }
        close(fd);
    }

private:
    void worker(int id) {
        // Open with O_APPEND so every write() goes to the end
        int fd = open(output_file_, O_WRONLY | O_APPEND);
        if (fd < 0) { perror("open"); return; }

        // Acquire write lock before writing to prevent interleaving
        if (!lock_file(fd)) { perror("lock"); close(fd); return; }

        std::string line = "Worker " + std::to_string(id)
                         + " (PID: " + std::to_string(getpid())
                         + ") completed\n";
        write(fd, line.c_str(), line.size());

        unlock_file(fd);
        close(fd);
    }

    int         num_workers_;
    const char* output_file_;
};

int main() {
    ProcessManager manager(5, "/tmp/process_manager_output.txt");
    manager.run();
    manager.print_results();
    return 0;
}

