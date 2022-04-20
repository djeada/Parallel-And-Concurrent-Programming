#include <iostream>
#include <thread>
#include <unistd.h> // getpid() definition

void foo() {
  std::cout << "Worker function." << std::endl;
  std::cout << "Worker function id: " << getpid() << std::endl;
  std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
}

int main() {
  std::cout << "Main function id: " << getpid() << std::endl;
  std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;

  std::thread thread(foo);

  // if we use join main thread will wait for that thread to finish
  thread.join();

  // other option is to detach the thread and then it will proceede freely on
  // it's own --daemon process thread.detach();

  // you can join and detach only once

  std::cout << std::boolalpha << thread.joinable() << std::endl;

  return 0;
}
