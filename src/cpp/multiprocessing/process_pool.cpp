/**
 * Process Pool
 *
 * This example demonstrates a simple process pool - a fixed number
 * of worker processes that execute tasks from a queue.
 *
 * Key concepts:
 * - Limits the number of concurrent processes to avoid resource exhaustion
 * - Reuses process slots as tasks complete
 * - Parent process manages the pool and collects results via exit codes
 * - Child exit status is retrieved via WEXITSTATUS macro
 *
 * Process pools are useful when:
 * - Tasks are CPU-bound and benefit from parallel execution
 * - You want to limit system resource usage
 * - Each task needs memory isolation
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

class ProcessPool {
public:
    explicit ProcessPool(size_t max_workers)
        : max_workers_(max_workers) {}

    void submit(int task_id) {
        // If pool is full, wait for a worker to finish
        if (workers_.size() >= max_workers_) {
            wait_for_one();
        }

        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "fork() failed\n";
            return;
        }
        if (pid == 0) {
            // Child: execute task and exit with result
            execute_task(task_id);
            _exit(task_id * 2);  // Return result via exit code
        } else {
            workers_.push_back(pid);
        }
    }

    void wait_all() {
        while (!workers_.empty()) {
            wait_for_one();
        }
    }

private:
    void wait_for_one() {
        int status;
        pid_t finished = wait(&status);

        if (WIFEXITED(status)) {
            std::cout << "Task result: " << WEXITSTATUS(status) << "\n";
        }

        auto it = std::find(workers_.begin(), workers_.end(), finished);
        if (it != workers_.end()) {
            workers_.erase(it);
        }
    }

    void execute_task(int task_id) {
        std::cout << "Task " << task_id << " starting (PID: " 
                  << getpid() << ")\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Task " << task_id << " finished\n";
    }

    size_t max_workers_;
    std::vector<pid_t> workers_;
};

int main() {
    constexpr int num_tasks = 10;
    constexpr int pool_size = 3;

    ProcessPool pool(pool_size);

    for (int i = 0; i < num_tasks; ++i) {
        pool.submit(i);
    }

    pool.wait_all();
    std::cout << "All tasks completed\n";

    return 0;
}
