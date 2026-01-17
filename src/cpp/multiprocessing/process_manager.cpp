/**
 * Process Manager
 *
 * This example demonstrates a simple process manager that coordinates
 * multiple worker processes writing to a shared resource (file).
 *
 * Key concepts:
 * - Manager pattern: central coordinator spawns and manages workers
 * - Each worker process runs independently with its own memory
 * - File I/O can be used as a simple form of IPC
 * - Wait for all children before reading results
 *
 * Note: File access without locking may cause interleaved writes.
 * For production code, use proper file locking or other IPC mechanisms.
 */

#include <fstream>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class ProcessManager {
public:
    explicit ProcessManager(int num_workers, const std::string& output_file)
        : num_workers_(num_workers), output_file_(output_file) {}

    void run() {
        // Clear output file
        std::ofstream(output_file_).close();

        // Spawn workers
        std::vector<pid_t> pids;
        for (int i = 0; i < num_workers_; ++i) {
            pid_t pid = fork();
            if (pid == 0) {
                worker(i);
                _exit(0);
            }
            pids.push_back(pid);
        }

        // Wait for all workers
        for (pid_t pid : pids) {
            waitpid(pid, nullptr, 0);
        }
    }

    void print_results() const {
        std::cout << "Results from workers:\n";
        std::ifstream in(output_file_);
        std::string line;
        while (std::getline(in, line)) {
            std::cout << "  " << line << "\n";
        }
    }

private:
    void worker(int id) {
        std::ofstream out(output_file_, std::ios::app);
        out << "Worker " << id << " (PID: " << getpid() << ") completed\n";
    }

    int num_workers_;
    std::string output_file_;
};

int main() {
    ProcessManager manager(5, "/tmp/process_manager_output.txt");
    manager.run();
    manager.print_results();
    return 0;
}
