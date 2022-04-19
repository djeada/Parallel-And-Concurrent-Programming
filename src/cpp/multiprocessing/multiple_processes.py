#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

const int NUM_PROCESSES = 5;

void printMessage(const int& processNum)
{
    std::cout << "Process #" << processNum << " with PID " << getpid() << " is running." << std::endl;
}

int main()
{

    printMessage(0);

    for (int j = 1; j <= NUM_PROCESSES; j++)
    {
        auto pid = fork();
        if (pid < 0)
        {
            std::cerr << "Fork failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            printMessage(j);
            exit(EXIT_SUCCESS);
        }
    }

    int status;
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        auto pid = wait(&status);
        std::cout << "Process " << pid << " exited with status " << status << std::endl;
    }

    return 0;
}
