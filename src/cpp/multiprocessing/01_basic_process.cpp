#include <iostream>
#include <thread>
#include <mutex>
#include <sys/types.h>
#include <unistd.h>

void foo() {
    std::cout << "Worker function" << std::endl;
    std::cout << "Worker function process id: " << getpid() << std::endl;
    std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
    std::cout << "Worker function finished" << std::endl;
}

int main() {
    std::cout << "Main function" << std::endl;
    std::cout << "Main function process id: " << getpid() << std::endl;
    std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        foo();
    } else if (pid > 0) {
        // Parent process
        waitpid(pid, nullptr, 0);
        std::cout << "Main function finished" << std::endl;
    } else {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }

    return 0;
}
