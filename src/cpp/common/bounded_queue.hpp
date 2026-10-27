#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <stop_token>
#include <utility>

namespace cpp_example {

template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {
        if (capacity == 0) throw std::invalid_argument("Queue capacity must be positive");
    }

    bool add_task(T task, std::stop_token stop = {}) {
        {
            std::unique_lock lock(mutex_);
            if (!changed_.wait(lock, stop, [this] {
                    return closed_ || queue_.size() < capacity_;
                }) || closed_) {
                return false;
            }
            queue_.push(std::move(task));
        }
        changed_.notify_all();
        return true;
    }

    std::optional<T> take_task(std::stop_token stop = {}) {
        std::unique_lock lock(mutex_);
        if (!changed_.wait(lock, stop, [this] { return closed_ || !queue_.empty(); })
            || queue_.empty()) {
            return std::nullopt;
        }
        T task = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        changed_.notify_all(); // Wake a producer waiting for capacity.
        return task;
    }

    void close() {
        {
            std::scoped_lock lock(mutex_);
            closed_ = true;
        }
        changed_.notify_all();
    }

private:
    const std::size_t capacity_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable_any changed_;
    bool closed_ = false;
};

} // namespace cpp_example
