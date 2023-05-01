#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class MyThread {
public:
  MyThread(std::function<void()> function)
      : function_(function), thread_(&MyThread::run, this) {}

  ~MyThread() { thread_.join(); }

  void run() {
    std::cout << "Function started" << std::endl;
    function_();
    std::cout << "Function finished" << std::endl;
  }

private:
  std::function<void()> function_;
  std::thread thread_;
};

void my_function() {
  std::cout << "my_function goes to sleep" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "my_function wakes up" << std::endl;
}

int main() {
  // Start threads with different functions
  MyThread thread_a(my_function);
  MyThread thread_b(
      []() { std::cout << "it's me, the lambda function" << std::endl; });

  // Threads are joined automatically in MyThread's destructor

  std::cout << "Main thread finished" << std::endl;

  return 0;
}
