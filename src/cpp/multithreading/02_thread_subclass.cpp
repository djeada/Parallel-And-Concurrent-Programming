#include <chrono>
#include <iostream>
#include <thread>

class MyThread : public std::thread {
public:
  template <typename Function>
  MyThread(Function &&function)
      : std::thread(
            [function](Function func) {
              std::cout << func.__name__() << " started" << std::endl;
              func();
              std::cout << func.__name__() << " finished" << std::endl;
            },
            std::forward<Function>(function)) {}
};

void my_function() {
  std::cout << "my_function goes to sleep" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "my_function wakes up" << std::endl;
}

const char *lambda_name() { return "it's me, the lambda function"; }

int main() {
  MyThread thread_a(my_function);
  MyThread thread_b([]() { std::cout << lambda_name() << std::endl; });

  thread_a.join();
  thread_b.join();

  std::cout << "Main thread finished" << std::endl;

  return 0;
}
