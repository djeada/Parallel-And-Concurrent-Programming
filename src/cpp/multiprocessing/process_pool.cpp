/**
 * C++20 + POSIX: persistent workers and typed pipe results.
 * Each worker reads multiple tasks from its pipe. Results travel through IPC,
 * not eight-bit exit codes. Bound in-flight tasks to apply backpressure, close
 * task writers to signal EOF, then collect results and reap owned children.
 * Compile: g++ -std=c++20 -pthread process_pool.cpp
 */
#include "../common/posix.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <span>
#include <thread>
#include <vector>

class ProcessPool {
public:
    explicit ProcessPool(std::size_t workers)
        : tasks_(workers), children_(workers) {
        if (workers == 0 || workers > 64) {
            throw std::invalid_argument("Worker count must be between 1 and 64");
        }
        for (std::size_t id = 0; id < workers; ++id) {
            children_.spawn([&, id] {
                results_.reader.reset();
                for (std::size_t index = 0; index < tasks_.size(); ++index) {
                    tasks_[index].writer.reset();
                    if (index != id) tasks_[index].reader.reset();
                }
                int task = 0;
                while (posix_example::read_exact(tasks_[id].reader.get(),
                       std::as_writable_bytes(std::span(&task, 1)))) {
                    std::cout << "Task " << task << " starting (PID: " << getpid() << ")\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    const std::array<std::int64_t, 2> result{task, std::int64_t{task} * 2};
                    // One small record fits PIPE_BUF: writes from workers do
                    // not interleave. Native int layout is local-machine IPC.
                    posix_example::write_all(results_.writer.get(), std::as_bytes(std::span(result)));
                    std::cout << "Task " << task << " finished\n";
                }
            });
        }
        results_.writer.reset();
        for (auto& task : tasks_) task.reader.reset();
    }

    ~ProcessPool() {
        for (auto& task : tasks_) task.writer.reset();
        if (std::uncaught_exceptions() == 0) {
            try {
                while (pending_ != 0) collect_one();
            } catch (...) {
                children_.terminate();
            }
        }
        // children_ is destroyed first and reaps workers while pipe state lives.
    }
    ProcessPool(const ProcessPool&) = delete;
    ProcessPool& operator=(const ProcessPool&) = delete;

    void submit(int task) {
        if (closed_) throw std::logic_error("Submitting to a closed process pool");
        if (pending_ >= tasks_.size() * 2) collect_one();
        auto& destination = tasks_[next_worker_++ % tasks_.size()];
        posix_example::write_all(destination.writer.get(), std::as_bytes(std::span(&task, 1)));
        ++pending_;
    }

    void wait_all() {
        for (auto& task : tasks_) task.writer.reset();
        closed_ = true;
        while (pending_ != 0) collect_one();
        if (!children_.wait_all()) throw std::runtime_error("A process worker failed");
    }

private:
    void collect_one() {
        std::array<std::int64_t, 2> result{};
        if (!posix_example::read_exact(results_.reader.get(), std::as_writable_bytes(std::span(result)))) {
            throw std::runtime_error("Workers stopped before returning all results");
        }
        --pending_;
        std::cout << "Task result: " << result[1] << '\n';
    }

    std::vector<posix_example::Pipe> tasks_;
    posix_example::Pipe results_;
    std::size_t next_worker_ = 0;
    std::size_t pending_ = 0;
    bool closed_ = false;
    posix_example::ChildGroup children_;
};

int main() try {
    ProcessPool pool(3);
    for (int task = 0; task < 10; ++task) pool.submit(task);
    pool.wait_all();
    std::cout << "All tasks completed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
