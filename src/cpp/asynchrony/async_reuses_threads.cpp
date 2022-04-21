#include <future>
#include <iostream>
#include <thread>

void foo() {
  std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
}

auto main() -> int {
  std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;

  std::future<void> future1 = async(std::launch::async, foo);
  future1.get();

  std::future<void> future2 = async(std::launch::async, foo);
  future2.get();

  std::future<void> future3 = async(std::launch::async, foo);
  future3.get();

  return 0;
}
