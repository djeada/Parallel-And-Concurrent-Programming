#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <chrono>
#include <thread>

void foo() {
    std::cout << "Function foo starts" << std::endl;
    std::cout << "Worker function process id: " << getpid() << std::endl;
    std::cout << "Worker thread id: " << pthread_self() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Function foo finishes" << std::endl;
}

void bar() {
    std::cout << "Function bar starts" << std::endl;
    std::cout << "Worker function process id: " << getpid() << std::endl;
    std::cout << "Worker thread id: " << pthread_self() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Function bar finishes" << std::endl;
}

int main() {
    std::cout << "Main function" << std::endl;
    std::cout << "Main function process id: " << getpid() << std::endl;
    std::cout << "Main thread id: " << pthread_self() << std::endl;

    pid_t pid = fork();

    if (pid == 0) { // Child process
        foo();
    } else if (pid > 0) { // Parent process
        pid_t pid2 = fork();
        if (pid2 == 0) { // Second child process
            bar();
        } else if (pid2 > 0) { // Parent process
            int status;
            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            std::cout << "Main function finished" << std::endl;
        } else {
            std::cerr << "Error: Failed to create the second child process" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Failed to create the first child process" << std::endl;
        return 1;
    }

    return 0;
}
