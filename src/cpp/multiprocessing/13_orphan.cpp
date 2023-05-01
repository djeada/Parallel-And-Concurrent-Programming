/*
This program demonstrates the concept of orphan processes in C++. An orphan
process is a child process whose parent process has terminated before the child
process. In most operating systems, including Linux, when the parent process
terminates, its orphaned children are reparented to the init or systemd process
(PID 1).

In this example, we create a child process that outlives its parent process,
becoming an orphan. After the parent process exits, you can check the process
list (using commands like 'ps' or 'top') to see that the child process is still
running and has been reparented to the init or systemd process (PID 1).
*/

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();

  if (pid == 0) {
    // Child process
    std::cout << "Child process (PID: " << getpid() << ") started."
              << std::endl;
    sleep(10);
    std::cout << "Child process (PID: " << getpid() << ") finished."
              << std::endl;
  } else {
    // Parent process
    std::cout << "Parent process (PID: " << getpid() << ") started."
              << std::endl;
    std::cout << "Parent process (PID: " << getpid()
              << ") is exiting, leaving the child process orphaned."
              << std::endl;
  }

  return 0;
}
