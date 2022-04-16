#include <iostream>
#include <sys/wait.h>
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

  // create a new process with fork
  auto pid = fork();
  if (pid == 0) {
    // child process
    foo();
  } else {
    // parent process
    wait(NULL);
  }

  return 0;
}
