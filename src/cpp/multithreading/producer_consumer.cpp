#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstdio>

class TaskQueue {
public:
    void add_task(int task) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        task_queue.push(task);
        lock.unlock();
        task_added.notify_one();
    }

    int take_task() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        while (task_queue.empty()) {
            task_added.wait(lock);
        }
        int task = task_queue.front();
        task_queue.pop();
        return task;
    }

private:
    std::queue<int> task_queue;
    std::mutex queue_mutex;
    std::condition_variable task_added;
};

TaskQueue task_queue;

void task_producer() {
    for (int i = 0; i < 10000; i++) { // add 10,000 tasks
        task_queue.add_task(1);
    }
    task_queue.add_task(-1); // indicate no more tasks
    printf("Producer is done adding tasks!\n");
}

void task_consumer() {
    int tasks_completed = 0;
    while (true) {
        int task = task_queue.take_task();
        if (task == -1) { // check for last task
            printf("Consumer completed %d tasks.\n", tasks_completed);
            task_queue.add_task(-1); // put back last task for other consumers to take
            return;
        } else {
            tasks_completed += task; // complete the task
        }
    }
}

int main() {
    std::thread producer(task_producer);
    std::thread consumer1(task_consumer);
    std::thread consumer2(task_consumer);
    producer.join();
    consumer1.join();
    consumer2.join();
}
