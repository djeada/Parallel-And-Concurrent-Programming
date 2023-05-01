#include <chrono>
#include <coroutine>
#include <iostream>
#include <random>
#include <thread>

template <typename T> class AsyncGenerator {
public:
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  AsyncGenerator(handle_type h) : handle_(h) {}

  ~AsyncGenerator() {
    if (handle_) {
      handle_.destroy();
    }
  }

  AsyncGenerator(const AsyncGenerator &) = delete;
  AsyncGenerator &operator=(const AsyncGenerator &) = delete;

  AsyncGenerator(AsyncGenerator &&other) : handle_(other.handle_) {
    other.handle_ = nullptr;
  }

  AsyncGenerator &operator=(AsyncGenerator &&other) {
    if (this != &other) {
      if (handle_) {
        handle_.destroy();
      }
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

  bool next() {
    handle_.resume();
    return !handle_.done();
  }

  T value() { return handle_.promise().value_; }

  struct promise_type {
    T value_;

    std::suspend_always initial_suspend() { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    auto get_return_object() {
      return AsyncGenerator{handle_type::from_promise(*this)};
    }

    void return_void() {}

    std::suspend_always yield_value(T value) {
      value_ = value;
      return {};
    }

    void unhandled_exception() { std::exit(1); }
  };

private:
  handle_type handle_;
};

AsyncGenerator<int> async_generator() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.5, 1.5);

  for (int i = 0; i < 5; ++i) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(dis(gen) * 1000)));
    co_yield i;
  }
}

int main() {
  auto gen = async_generator();
  while (gen.next()) {
    std::cout << "Received value: " << gen.value() << std::endl;
  }

  return 0;
}
