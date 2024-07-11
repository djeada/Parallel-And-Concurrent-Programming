#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <condition_variable>
#include <atomic>

// Task processor function
void task_processor(int task_id) {
    std::cout << "Thread " << std::this_thread::get_id() << " processed task " << task_id << ".\n";
}

// Thread pool class
class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    void enqueue_task(std::function<void()> task);
    void join();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void worker_thread();
};

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::scoped_lock lock(queue_mutex);
        stop.store(true);
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue_task(std::function<void()> task) {
    {
        std::scoped_lock lock(queue_mutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::join() {
    {
        std::scoped_lock lock(queue_mutex);
        stop.store(true);
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this] { return stop.load() || !tasks.empty(); });
            if (stop.load() && tasks.empty()) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

int main() {
    ThreadPool pool(4); // 4 threads
    for (int i = 0; i < 100; i++) {
        pool.enqueue_task([i](){ task_processor(i); });
    }
    pool.join();
    return 0;
}
