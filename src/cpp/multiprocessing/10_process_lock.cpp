/*
This program demonstrates the use of a mutex (Lock) with process creation using fork() in C++.
A mutex (short for "mutual exclusion") is a synchronization primitive that is used to prevent
multiple processes from simultaneously accessing a shared resource, such as a file or
a variable in memory. In this example, we use a POSIX mutex to protect access
to a shared counter variable, ensuring that only one process can increment the counter at a time.
This prevents race conditions and ensures that the final value of the counter is correct.
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/mman.h>

struct SharedData {
    pthread_mutex_t mutex;
    int counter;
};

void worker(SharedData *data) {
    pthread_mutex_lock(&data->mutex);
    std::cout << "Process " << getpid() << " is starting..." << std::endl;
    sleep(1); // Simulate some work
    data->counter += 1;
    std::cout << "Process " << getpid() << " is finished. Counter: " << data->counter << std::endl;
    pthread_mutex_unlock(&data->mutex);
}

int main() {
    const int num_processes = 4;

    // Create shared memory
    SharedData *data = static_cast<SharedData *>(mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    // Initialize the mutex and the counter
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &mutex_attr);
    data->counter = 0;

    for (int i = 0; i < num_processes; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            worker(data);
            exit(0);
        } else if (pid > 0) {
            // Parent process
        } else {
            std::cerr << "Fork failed." << std::endl;
            exit(1);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_processes; ++i) {
        wait(NULL);
    }

    std::cout << "Final counter value: " << data->counter << std::endl;

    // Cleanup
    pthread_mutex_destroy(&data->mutex);
    pthread_mutexattr_destroy(&mutex_attr);
    munmap(data, sizeof(SharedData));

    return 0;
}
