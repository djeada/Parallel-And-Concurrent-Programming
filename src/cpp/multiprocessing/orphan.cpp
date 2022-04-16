#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

int main() {
  auto pid = fork();

  if (pid < 0) {
    std::cout << "Error in fork()" << std::endl;
    return 1;
  }

  if (pid == 0) {
    std::cout << "Child process" << std::endl;
    std::cout << "My PID is " << getpid() << std::endl;
    std::cout << "My parent PID is " << getppid() << std::endl;
    std::cout << "Child process is going to sleep for 5 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Child process is awake now" << std::endl;
    std::cout << "My parent PID is " << getppid() << std::endl;
    std::cout << "Child process is exiting" << std::endl;
    exit(0);
  } else {
    std::cout << "Parent process" << std::endl;
    std::cout << "My PID is " << getpid() << std::endl;
    std::cout << "Parent process is exiting" << std::endl;
  }

  std::cout << "End of main()" << std::endl;

  return 0;
}
