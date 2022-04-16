/*
Check for zombie processes with the following command:
ps axo stat,ppid,pid,comm | grep -w defunct
*/
#include <iostream>
#include <thread>
#include <unistd.h>

int main() {
  auto pid = fork();
  if (pid == 0) {
    std::cout << "The child process is terminated instantly. " << std::endl;
    exit(0);
  } else {

    int sleepTime = 45;
    std::cout << "The parent is going to sleep for " << sleepTime << " seconds"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime * 1000));
    std::cout << "The parent woke up" << std::endl;
  }
  return 0;
}
