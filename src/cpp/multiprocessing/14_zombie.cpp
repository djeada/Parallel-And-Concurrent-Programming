/*
This program demonstrates the concept of zombie processes in C++. A zombie process is
a child process that has terminated but has not been reaped by its parent process. The
operating system keeps an entry in the process table for the terminated child process,
holding information about its termination status until the parent process collects this
information using a system call (usually wait() or waitpid()).

In this example, we create a child process that terminates but isn't reaped by its
parent, becoming a zombie. After the child process exits, you can check the process
list (using commands like 'ps' or 'top') to see that the child process is in a 'zombie'
state, denoted by the status 'Z+'.

WARNING: This example intentionally creates a zombie process. You should manually kill
the parent process after running the program to avoid having a zombie process lingering
on your system.
*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        std::cout << "Child process (PID: " << getpid() << ") started." << std::endl;
        sleep(2);
        std::cout << "Child process (PID: " << getpid() << ") finished." << std::endl;
        exit(0);
    } else {
        // Parent process
        std::cout << "Parent process (PID: " << getpid() << ") started." << std::endl;
        std::cout << "Parent process (PID: " << getpid() << ") is NOT waiting for the child process to terminate." << std::endl;
        std::cout << "Child process (PID: " << pid << ") will become a zombie after termination." << std::endl;
        sleep(10);
        std::cout << "Parent process (PID: " << getpid() << ") finished." << std::endl;
    }

    return 0;
}
