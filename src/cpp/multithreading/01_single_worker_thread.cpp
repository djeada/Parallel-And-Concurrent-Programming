#include <iostream>
#include <string>
#include <sys/syscall.h>
#include <thread>
#include <unistd.h>

void foo() {
  std::cout << "Worker function" << std::endl;
  std::cout << "Worker function process id: " << getpid() << std::endl;
  std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
  std::cout << "Worker thread name: Example_thread" << std::endl;
}

int main() {
  std::cout << "Main function process id: " << getpid() << std::endl;
  std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;
  std::cout << "Main thread name: main" << std::endl;

  std::thread worker_thread(foo);

  // if we use join main thread will wait for that thread to finish
  worker_thread.join();

  // other option is to detach the thread and then it will proceede freely on
  // it's own --daemon process worker_thread.detach();

  // you can join and detach only once

  std::cout << std::boolalpha << worker_thread.joinable() << std::endl;

  return 0;
}
