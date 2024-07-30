## Multiprocessing

**Multiprocessing** involves running multiple processes simultaneously. Each process has its own memory space, making them more isolated from each other compared to threads, which share the same memory. This isolation means that multiprocessing can be more robust and less prone to errors from shared state, as each process runs independently. Multiprocessing is often used to leverage multiple CPU cores, allowing a program to perform computationally intensive tasks in parallel, thus improving performance. Communication between processes is typically achieved through inter-process communication (IPC) mechanisms, such as pipes, sockets, or shared memory. While more resource-intensive than multithreading, due to the need for separate memory spaces, multiprocessing can achieve better performance for CPU-bound tasks and provides better fault isolation.

### Introduction to Processes

In computing, a process is an instance of a program in execution. It includes the program code, current activity, and the state of the program's resources. Processes are crucial for multitasking environments, as they allow multiple programs to run concurrently on a single computer system. A process can create other processes during its execution, which are termed as child processes. These child processes are managed by the parent process, which can control and monitor their execution status, handle their termination, and communicate with them.

#### Child Processes

A child process is created by a parent process using system calls like `fork` or `spawn`. The child process inherits a subset of the parent's resources and environment. The parent process has several responsibilities and capabilities concerning its child processes:

The parent can **wait** for a child process to finish execution using system calls like `wait` or `waitpid`. This allows the parent to collect the exit status and determine if the child terminated successfully or encountered an error.
- The parent can check the **state** of the child process, determining whether it is currently running, sleeping (waiting for a resource), stopped (suspended), or a zombie (completed but not yet reaped).
- Parent and child processes can communicate through various Inter-Process Communication (IPC) mechanisms, such as pipes, sockets, or shared memory, to exchange data or signals.
- The parent can terminate the child process if necessary, such as when the child is misbehaving or taking too long to complete, using system calls like `kill`.

#### Zombie Process

A zombie process is a process that has completed its execution but still has an entry in the process table. This situation occurs because the parent process has not yet read the exit status of the child process. Although zombies do not consume significant system resources, they occupy a slot in the process table. If a parent process does not properly clean up after its children, numerous zombie processes can accumulate, potentially exhausting the system's available process slots and slowing down system performance.

#### Orphan Process

An orphan process is a process whose parent process has terminated before the child process. When a parent process terminates, its child processes are typically adopted by the system's init process (PID 1), which becomes their new parent. The init process periodically reaps orphaned processes, ensuring that they do not become zombies. Orphan processes continue running and are managed like any other process by the system.

### Communication Between Processes

Effective multiprocessing often requires processes to communicate with each other to share data, synchronize actions, or coordinate tasks. There are several methods to facilitate this communication, each with its own advantages and limitations. Choosing the appropriate method depends on factors like the amount of data being transferred, the need for synchronization, and whether the processes are running on the same or different machines.

#### Message Passing

Message passing is a communication method where processes exchange data through messages. This method can be implemented using various Inter-Process Communication (IPC) mechanisms, such as:

- **Message Queues** provide a queueing system where messages are stored until the receiving process retrieves them. They are useful for asynchronous communication, allowing processes to send and receive messages independently of each other's execution state.
- **Sockets** enable communication between processes over a network, making them suitable for both local and remote communication. They can be used for stream-based (TCP) or datagram-based (UDP) communication, depending on the needs for reliability and speed.
- **Signals** are simple notifications sent to a process to indicate an event has occurred. They are often used for simple synchronization or to trigger an action but carry limited information compared to other methods.

Message passing is beneficial because it naturally supports the isolation of processes, reducing the risk of interference and increasing system robustness. However, it can introduce overhead, particularly when large messages are involved, due to the need to copy data between processes. Additionally, ensuring the order and delivery of messages can be complex, especially in distributed systems.

#### Shared Memory

Shared memory allows multiple processes to access a common memory area, enabling them to read and write data quickly. This method is efficient for large data exchanges because it avoids the overhead associated with copying data between processes. Shared memory is particularly useful in scenarios where low-latency data transfer is critical, such as in real-time applications or high-performance computing.

However, shared memory requires careful management to prevent data corruption and ensure consistency. Key challenges include:

- Without proper synchronization mechanisms like locks, semaphores, or condition variables, concurrent access by multiple processes can lead to race conditions, where the outcome depends on the non-deterministic order of operations.
- Shared memory regions must be carefully managed to ensure that only authorized processes can access sensitive data, as they bypass the usual protection mechanisms provided by process isolation.
- Shared memory is generally **limited** to processes running on the same machine, as it involves direct access to memory. While there are techniques to extend shared memory across networked systems, such as distributed shared memory, they introduce additional complexity and overhead.

#### Pipes

Pipes are a simple and efficient form of inter-process communication that allow one-way data flow between processes. There are two main types of pipes:

- **Anonymous Pipes** are used for communication between processes that have a common ancestor, typically a parent-child relationship. They are created using system calls and provide a unidirectional channel for data flow. Anonymous pipes are often used for simple data transfer where one process writes data to the pipe, and the other reads it. However, they do not support complex communication patterns, and their lifespan is tied to the processes that use them.
- **Named Pipes** also known as FIFOs (First In, First Out), can be used for communication between unrelated processes. Unlike anonymous pipes, named pipes have a presence in the file system, allowing processes to access them by name. They support both local and networked inter-process communication and can be bidirectional. Named pipes provide a flexible mechanism for data transfer and synchronization, but managing access permissions and ensuring proper closing of the pipes can add complexity.

Pipes are advantageous because they are lightweight and provide a straightforward mechanism for data streaming. However, they have limitations in terms of buffering capacity and are primarily suited for unidirectional or limited bidirectional communication. Additionally, pipes do not provide built-in mechanisms for complex synchronization, so additional coordination may be necessary for more sophisticated communication patterns.

### Challenges with Multiprocessing

Multiprocessing introduces several challenges, particularly in managing and coordinating independent processes. These challenges include debugging, resource contention, ensuring process synchronization, and managing the overhead associated with inter-process communication.

#### Debugging

Debugging multiprocessing applications is inherently more complex than debugging single-process applications. Each process may have its own set of bugs and issues, and the interaction between processes can introduce additional challenges. Standard debugging tools often focus on a single process, making it necessary to debug each process individually and then understand how they interact. Specific challenges include:

- Occur when the outcome depends on the timing or sequence of events across multiple processes. These are notoriously difficult to reproduce and fix, as they may not consistently manifest.
- The parallel nature of multiprocessing can lead to nondeterministic behavior, where the same inputs do not always produce the same outputs due to the variability in process scheduling and execution order.
- Effective logging and monitoring are essential but challenging in a multiprocessing environment, as logs from different processes need to be correlated accurately.

#### Deadlocks

Deadlocks occur when a set of processes are unable to proceed because each process is waiting for resources held by others, creating a cycle of dependencies that prevents any process from continuing. Deadlocks are characterized by four conditions:

- Resources are allocated to only one process at a time.
- A set of processes are waiting on each other in a circular chain, with each process holding a resource the next process needs.
- Resources cannot be forcibly taken from a process; they must be released voluntarily.
- Processes hold onto resources they already have while waiting for additional resources.

Preventing or mitigating deadlocks requires careful design, such as implementing resource allocation strategies, imposing ordering on resource acquisition, or employing deadlock detection and recovery mechanisms.

#### Data Races

Data races occur when two or more processes or threads access shared data simultaneously, and at least one of the accesses is a write operation. This can lead to inconsistent or incorrect data being read or written, as the processes may overwrite each other's changes unpredictably. Challenges include:

- Identifying data races is challenging because they depend on the specific timing of process execution.
- To prevent data races, synchronization mechanisms like locks, semaphores, or message passing are used, which can introduce significant overhead and complexity, potentially reducing performance.

#### Resource Contention

Resource contention arises when multiple processes compete for the same limited resources, such as CPU time, memory, disk I/O, or network bandwidth. This competition can lead to:

- Processes may experience slowdowns due to waiting for resources to become available.
- Some processes may be perpetually delayed or blocked from accessing required resources, leading to inefficiencies.
- Efficiently scheduling processes to optimize resource use while minimizing contention is a complex problem, often requiring dynamic adjustment based on current system load.

#### Process Synchronization

Ensuring proper synchronization between processes is crucial to avoid inconsistencies and ensure correct execution. Challenges in synchronization include:

- Synchronization mechanisms can introduce significant overhead, especially in high-contention scenarios, potentially negating the benefits of parallelism.
- Designing correct and efficient synchronization mechanisms is complex and prone to errors, such as introducing deadlocks or livelocks.
- Choosing the right level of granularity for synchronization is critical; too coarse can reduce parallelism, while too fine can lead to excessive overhead.

### Process Management Techniques

Efficiently managing processes involves various techniques to ensure smooth operation, resource optimization, and system stability.

#### Process Synchronization

Process synchronization is crucial to prevent data corruption and ensure consistency when multiple processes access shared resources. Techniques like locks, semaphores, and monitors are used to coordinate access. For example, a lock can prevent multiple processes from modifying a shared resource simultaneously, while a semaphore can manage a limited number of concurrent accesses. Monitors provide a high-level abstraction for managing mutual exclusion and condition synchronization.

#### Load Balancing

Load balancing distributes workloads across multiple processes or processors to ensure optimal use of resources and reduce wait times. Effective load balancing considers the current load on each process, the available system resources, and the nature of the tasks. Techniques for load balancing can be static (pre-determined distribution) or dynamic (adjusting based on real-time system state).

#### Scalability

Scalability refers to the ability of a system to handle increasing workloads by adding more resources, such as additional processes or hardware. Scalability is critical for performance improvement and involves designing systems that can efficiently utilize additional resources without significant overhead. This includes choosing appropriate algorithms, data structures, and communication methods that can scale effectively as the system grows.

### Alternatives to Multiprocessing

While traditional multiprocessing is a widely used approach for parallel execution and resource management, several alternative methods achieve concurrency, isolation, and efficient utilization of system resources. These alternatives offer various advantages, including improved scalability, easier deployment, and better resource isolation. Here are some notable alternatives:

#### Containers

Containers provide a lightweight alternative to traditional multiprocessing by encapsulating applications in isolated environments. This encapsulation includes the application's code, libraries, dependencies, and configuration files. Containers are often used in a microservice architecture, where each service runs in its own container, simplifying deployment and management. They offer advantages such as:

- Containers ensure that applications run in isolated environments, preventing conflicts between dependencies and reducing the risk of security vulnerabilities.
- Containers can be easily scaled up or down to meet demand, enabling efficient use of resources.
- By packaging all necessary components together, containers ensure that applications run consistently across different environments, from development to production.
- Containers can be quickly started or stopped, making them ideal for environments where quick scaling and deployment are necessary.

Containers may introduce overhead for short-lived processes due to the need to set up and tear down the container environment.

#### Event-Driven Architectures

Event-driven architectures revolve around the concept of events, which are messages or signals that indicate a change in state or the occurrence of an action. This architecture is often used in systems where activities are triggered by external inputs, such as user actions or sensor readings. Key benefits include:

- Event-driven systems can respond quickly to changes and inputs, providing a high level of interactivity.
- Components in an event-driven system are loosely coupled, making the system more modular and easier to maintain.
- These systems can handle high volumes of events by distributing the load across multiple event handlers.

Event-driven architectures are particularly well-suited for applications like real-time data processing, user interfaces, and IoT (Internet of Things) systems.

#### Microservices

Microservices architecture decomposes applications into smaller, independently deployable services that communicate through APIs. Each microservice handles a specific business functionality and can be developed, deployed, and scaled independently. Advantages include:

- Microservices can be updated and deployed without affecting the entire application, reducing downtime and deployment risks.
- Individual services can be scaled independently based on demand, optimizing resource usage.
- Different microservices can use different technologies and programming languages, allowing teams to choose the best tool for each job.
- Failures in one microservice do not necessarily affect others, improving the overall resilience of the system.

Managing a microservices architecture can be complex due to the need for effective communication and coordination between services.

#### Serverless Computing

Serverless computing, also known as Function-as-a-Service (FaaS), abstracts server management by allowing developers to run code in response to events without managing the underlying infrastructure. Code is executed in stateless functions, triggered by specific events such as HTTP requests or database changes. Key benefits include:

- Developers focus on writing code without worrying about server provisioning, maintenance, or scaling.
- Users pay only for the compute time consumed by their functions, potentially reducing costs for infrequently used applications.
- Serverless platforms automatically scale functions based on demand, ensuring efficient resource usage.
- Functions can be deployed rapidly, enabling faster development cycles.

Serverless computing is ideal for applications with variable workloads, such as data processing pipelines, real-time analytics, and microservices.

#### Virtual Machines (VMs)

Virtual machines provide a more traditional approach to achieving process isolation by running a complete operating system environment within a host system. Each VM has its own virtualized hardware, operating system, and applications. Benefits include:

- VMs provide strong isolation between different environments, making them suitable for running untrusted applications or multiple users on the same hardware.
- VMs can run different operating systems and configurations, offering flexibility for various applications and use cases.
- VMs allow precise allocation of resources (CPU, memory, storage) to different environments, ensuring optimal use of hardware.
- VMs can be used to run legacy applications that require specific older operating systems or configurations.

VMs tend to have higher overhead compared to containers, as they require running a full operating system instance for each VM.

### Examples

#### Examples in C++

In C++, processes can be created and managed using various system APIs and libraries. A process is an instance of a running program that has its own memory space and resources. Unlike threads, processes do not share memory, which provides better isolation but also requires more overhead for inter-process communication (IPC).

##### Creating Processes

To create a new process, the operating system provides specific APIs. On POSIX-compliant systems like Linux, the `fork()` system call is commonly used. The `fork()` call creates a new process by duplicating the calling process. The new process, called the child process, runs concurrently with the parent process.

```cpp
#include <iostream>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        std::cout << "Hello from the child process!" << std::endl;
    } else if (pid > 0) {
        // Parent process
        std::cout << "Hello from the parent process!" << std::endl;
    } else {
        // Fork failed
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
```

In this example, `fork()` creates a new process. The return value in the child process is `0`, while in the parent process, it is the PID of the child.

##### Process Termination

Processes can be terminated using the `exit()` function, which ends the process and returns a status code to the operating system. The parent process can wait for the termination of the child process using the `wait()` or `waitpid()` functions.

```cpp
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        std::cout << "Child process terminating." << std::endl;
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Child process finished with status " << status << std::endl;
    }
    
    return 0;
}
```

Here, the parent process waits for the child to terminate and retrieves its exit status.

##### Inter-Process Communication (IPC)

Processes can communicate with each other using IPC mechanisms, such as pipes, message queues, shared memory, and sockets. Pipes are a simple and commonly used IPC method for one-way communication between processes.

```cpp
#include <iostream>
#include <unistd.h>

int main() {
    int pipefd[2];
    pipe(pipefd);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        close(pipefd[1]); // Close write end
        char buffer[128];
        read(pipefd[0], buffer, sizeof(buffer));
        std::cout << "Child received: " << buffer << std::endl;
        close(pipefd[0]);
    } else {
        // Parent process
        close(pipefd[0]); // Close read end
        const char* message = "Hello from parent";
        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]);
    }
    
    return 0;
}
```

In this example, a pipe is used for communication between the parent and child processes. The parent writes a message to the pipe, and the child reads it.

##### Shared Memory

Shared memory allows multiple processes to access the same memory space, providing a fast way to share data. This requires careful synchronization to prevent race conditions.

```cpp
#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

int main() {
    const int SIZE = 4096;
    void* shared_memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        std::strcpy(static_cast<char*>(shared_memory), "Shared memory message");
        munmap(shared_memory, SIZE);
    } else {
        // Parent process
        wait(nullptr);
        std::cout << "Parent received: " << static_cast<char*>(shared_memory) << std::endl;
        munmap(shared_memory, SIZE);
    }
    
    return 0;
}
```

Here, `mmap` is used to create a shared memory region accessible by both the parent and child processes.

##### Process Synchronization

Processes can be synchronized using various techniques like semaphores or mutexes to control access to shared resources. For example, POSIX semaphores can be used to coordinate access to shared memory.

```cpp
#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>

int main() {
    const int SIZE = 4096;
    void* shared_memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t* sem = static_cast<sem_t*>(mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    sem_init(sem, 1, 1); // Shared between processes, initial value 1

    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        sem_wait(sem);
        std::strcpy(static_cast<char*>(shared_memory), "Child writes to shared memory");
        sem_post(sem);
        munmap(shared_memory, SIZE);
    } else {
        // Parent process
        wait(nullptr);
        sem_wait(sem);
        std::cout << "Parent reads: " << static_cast<char*>(shared_memory) << std::endl;
        sem_post(sem);
        munmap(shared_memory, SIZE);
    }
    
    sem_destroy(sem);
    return 0;
}
```

In this example, a semaphore is used to synchronize access to the shared memory between the parent and child processes.

##### Performance Considerations and Best Practices

- Creating processes is generally more expensive than creating threads due to the need to allocate separate memory and resources. Minimize process creation by reusing processes when possible.
- IPC mechanisms like pipes and shared memory involve overhead. Use the most efficient IPC method for the task at hand and minimize the amount of data transferred.
- Ensure that IPC and synchronization mechanisms are used correctly to avoid deadlocks, where two or more processes are waiting indefinitely for each other.
- Properly manage resources such as file descriptors and shared memory to avoid resource leaks and ensure that resources are cleaned up when no longer needed.


| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/01_basic_process.cpp) | Create and start a basic process                      |
| 2   | [multiple_processes.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/02_multiple_processes.cpp) | Integrate multiple processes for a complex task       |
| 3   | [deadlock.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/03_deadlock.cpp) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [process_pool.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/04_process_pool.cpp) | Use a process pool to manage concurrent tasks         |
| 5   | [queue_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/05_queue_communication.cpp) | Communicate between processes using a Queue           |
| 6   | [pipe_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/06_pipe_communication.cpp) | Communicate between processes using a Pipe            |
| 7   | [shared_value.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/07_shared_value.cpp) | Use a shared value to store data between processes    |
| 8   | [shared_array.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/08_shared_array.cpp) | Use a shared array to store data between processes    |
| 9   | [manager.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/09_manager.cpp) | Use a manager to share complex data structures        |
| 10  | [process_lock.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/10_process_lock.cpp) | Use a lock to synchronize access to shared resources  |
| 11  | [process_semaphore.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/11_process_semaphore.cpp) | Use a semaphore to control access to shared resources |
| 12  | [process_barrier.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/12_process_barrier.cpp) | Use a barrier to synchronize multiple processes       |
| 13  | [orphan.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/13_orphan.cpp) | Demonstrate an orphan process scenario                |
| 14  | [zombie.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/14_zombie.cpp) | Demonstrate a zombie process scenario                 |

#### Examples in Python

In Python, processes can be created and managed using the `multiprocessing` module, which provides a way to create separate processes that run concurrently. Each process has its own memory space, making it a safer option for parallel execution, especially when working with CPU-bound tasks.

##### Creating Processes

To create a new process, the `multiprocessing` module provides the `Process` class. You can define a target function and pass it to a `Process` object, along with any arguments required by the function.

```python
from multiprocessing import Process
import os

def print_message(message):
    print(f"Process ID: {os.getpid()} - {message}")

if __name__ == '__main__':
    p = Process(target=print_message, args=("Hello from the child process!",))
    p.start()  # Start the process
    p.join()   # Wait for the process to finish
    print("Main process finished.")
```

In this example, a new process is created to run the `print_message` function. The `start()` method initiates the process, and `join()` waits for it to complete.

##### Process Termination

A process can be terminated using the `terminate()` method, which stops the process abruptly. The `exitcode` attribute of the `Process` object can be checked to see how the process exited.

```python
from multiprocessing import Process
import time

def long_task():
    print("Starting long task...")
    time.sleep(5)
    print("Task completed.")

if __name__ == '__main__':
    p = Process(target=long_task)
    p.start()
    time.sleep(2)
    p.terminate()  # Terminate the process
    p.join()
    print(f"Process terminated with exit code {p.exitcode}.")
```

Here, the process is terminated after 2 seconds, regardless of whether it has completed its task.

##### Inter-Process Communication (IPC)

Python provides several ways for processes to communicate, such as pipes, queues, and shared memory. Queues are particularly easy to use and allow safe data sharing between processes.

```python
from multiprocessing import Process, Queue

def producer(queue):
    queue.put("Hello from producer")

def consumer(queue):
    message = queue.get()
    print(f"Consumer received: {message}")

if __name__ == '__main__':
    queue = Queue()
    p1 = Process(target=producer, args=(queue,))
    p2 = Process(target=consumer, args=(queue,))
    p1.start()
    p2.start()
    p1.join()
    p2.join()
```

In this example, the producer process puts a message into the queue, which the consumer process retrieves.

##### Shared Memory

Shared memory allows multiple processes to access the same data. Python's `multiprocessing` module provides `Value` and `Array` for simple data types.

```python
from multiprocessing import Process, Value

def increment(shared_value):
    with shared_value.get_lock():  # Ensure mutual exclusion
        shared_value.value += 1

if __name__ == '__main__':
    shared_value = Value('i', 0)  # 'i' stands for integer
    processes = [Process(target=increment, args=(shared_value,)) for _ in range(5)]

    for p in processes:
        p.start()

    for p in processes:
        p.join()

    print(f"Shared value: {shared_value.value}")
```

In this example, multiple processes safely increment a shared integer using a `Value` object.

##### Process Synchronization

Synchronization between processes can be achieved using locks, events, conditions, and semaphores. These synchronization primitives ensure that only one process can access a critical section at a time.

```python
from multiprocessing import Process, Lock

def print_with_lock(lock, message):
    with lock:
        print(message)

if __name__ == '__main__':
    lock = Lock()
    messages = ["Message 1", "Message 2", "Message 3"]
    processes = [Process(target=print_with_lock, args=(lock, msg)) for msg in messages]

    for p in processes:
        p.start()

    for p in processes:
        p.join()
```

Here, a lock is used to synchronize access to the print statement, ensuring that messages are printed one at a time.

##### Process Pools

For managing a large number of processes, the `multiprocessing.Pool` class provides a convenient way to parallelize the execution of a function across multiple input values.

```python
from multiprocessing import Pool

def square(x):
    return x * x

if __name__ == '__main__':
    with Pool(4) as p:
        results = p.map(square, [1, 2, 3, 4, 5])
    print(results)
```

In this example, a pool of four processes is created to compute the square of numbers concurrently.

##### Performance Considerations and Best Practices

- Creating and destroying processes can be costly in terms of time and resources. Use process pools or reuse processes to mitigate this overhead.
- Use efficient IPC methods appropriate for the data size and structure. For small amounts of data, queues and pipes are often sufficient, but for larger shared data, shared memory can be more efficient.
- Ensure that synchronization primitives are used correctly to avoid deadlocks, where processes are stuck waiting for each other.
- Properly manage resources such as file descriptors and shared memory. Clean up resources when they are no longer needed to avoid leaks and ensure system stability.

Python's `multiprocessing` module makes it easy to create and manage processes, providing a higher level of parallelism and isolation compared to threading. This is particularly useful for CPU-bound tasks and scenarios where memory safety is a concern.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/01_basic_process.py) | Create and start a basic process                      |
| 2   | [multiple_processes.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/02_multiple_processes.py) | Integrate multiple processes for a complex task       |
| 3   | [deadlock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/03_deadlock.py) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [process_pool.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/04_process_pool.py) | Use a process pool to manage concurrent tasks         |
| 5   | [queue_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/05_queue_communication.py) | Communicate between processes using a Queue           |
| 6   | [pipe_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/06_pipe_communication.py) | Communicate between processes using a Pipe            |
| 7   | [shared_value.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/07_shared_value.py) | Use a shared value to store data between processes    |
| 8   | [shared_array.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/08_shared_array.py) | Use a shared array to store data between processes    |
| 9   | [manager.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/09_manager.py) | Use a manager to share complex data structures        |
| 10  | [process_lock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/10_process_lock.py) | Use a lock to synchronize access to shared resources  |
| 11  | [process_semaphore.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/11_process_semaphore.py) | Use a semaphore to control access to shared resources |
| 12  | [process_barrier.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/12_process_barrier.py) | Use a barrier to synchronize multiple processes       |
| 13  | [orphan.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/13_orphan.py) | Demonstrate an orphan process scenario                |
| 14  | [zombie.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/14_zombie.py) | Demonstrate a zombie process scenario                 |

#### Examples in JavaScript

In Node.js, processes can be created and managed using the `child_process` module. This module allows you to spawn new processes, execute commands, and communicate with child processes. Node.js is single-threaded by default, but the `child_process` module provides the ability to utilize multiple processes for parallel execution.

##### Creating Processes

Node.js provides several methods to create child processes, including `spawn`, `exec`, `execFile`, and `fork`. The `spawn` method is used to launch a new process with a specified command.

```javascript
const { spawn } = require('child_process');

const ls = spawn('ls', ['-lh', '/usr']);

ls.stdout.on('data', (data) => {
  console.log(`Output: ${data}`);
});

ls.stderr.on('data', (data) => {
  console.error(`Error: ${data}`);
});

ls.on('close', (code) => {
  console.log(`Child process exited with code ${code}`);
});
```

In this example, the `spawn` method is used to execute the `ls` command. The output and errors from the command are captured via the `stdout` and `stderr` streams.

##### Process Termination

A process can be terminated using the `kill` method, which sends a signal to the process. The `SIGTERM` signal is commonly used to request a graceful shutdown.

```javascript
const { spawn } = require('child_process');

const process = spawn('node', ['-e', 'console.log("Running"); setTimeout(() => {}, 10000)']);

setTimeout(() => {
  process.kill('SIGTERM');
  console.log('Process terminated');
}, 2000);
```

Here, the process is terminated after 2 seconds using `kill`.

##### Inter-Process Communication (IPC)

Node.js supports IPC between parent and child processes through the use of the `fork` method. The `fork` method spawns a new Node.js process and establishes a communication channel between the parent and child processes.

```javascript
// parent.js
const { fork } = require('child_process');
const child = fork('./child.js');

child.on('message', (message) => {
  console.log(`Parent received: ${message}`);
});

child.send('Hello from parent');

// child.js
process.on('message', (message) => {
  console.log(`Child received: ${message}`);
  process.send('Hello from child');
});
```

In this example, the parent process communicates with the child process using the `send` and `message` events.

##### Handling Process Output

Child process methods such as `spawn` and `exec` provide ways to handle output and errors. The `exec` method buffers the entire output and invokes a callback when the process terminates.

```javascript
const { exec } = require('child_process');

exec('node -v', (error, stdout, stderr) => {
  if (error) {
    console.error(`Error: ${error.message}`);
    return;
  }
  if (stderr) {
    console.error(`Stderr: ${stderr}`);
    return;
  }
  console.log(`Stdout: ${stdout}`);
});
```

This example uses `exec` to run a command and handle the output and errors in a callback function.

##### Using Process Pools

Node.js does not have a built-in concept of process pools like some other languages. However, you can manage a pool of processes by manually spawning a set number of processes and reusing them. For more advanced scenarios, libraries like `generic-pool` or `node-pool` can be used to manage resource pools.

```javascript
const { fork } = require('child_process');
const numWorkers = 4;
const workers = [];

for (let i = 0; i < numWorkers; i++) {
  workers.push(fork('./worker.js'));
}

workers.forEach((worker, index) => {
  worker.on('message', (message) => {
    console.log(`Worker ${index} says: ${message}`);
  });

  worker.send(`Hello from main to worker ${index}`);
});
```

In this example, a pool of worker processes is created and managed manually.

##### Performance Considerations and Best Practices

- Node.js is single-threaded, and blocking the event loop can prevent the system from handling other tasks. Offload CPU-intensive tasks to child processes to maintain responsiveness.
- When using IPC, minimize the data transferred between processes to reduce overhead. Use efficient data formats and avoid unnecessary serialization.
- Ensure that processes are terminated gracefully. Handle cleanup tasks like closing database connections and freeing resources before terminating.
- Always handle errors when working with child processes. Use event listeners for `error`, `exit`, and `close` events to handle unexpected situations.
- Be cautious when executing external commands to avoid security vulnerabilities, such as command injection attacks. Sanitize inputs and validate all arguments.

Node.js's `child_process` module offers a powerful way to handle multiple processes, enabling parallel execution and efficient resource management. This is particularly useful for offloading heavy computation tasks and handling large I/O operations.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/01_basic_process.js) | Create and start a basic process                      |
| 2   | [multiple_processes.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/02_multiple_processes.js) | Integrate multiple processes for a complex task       |
| 3   | [deadlock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/03_deadlock.js) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [process_pool.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/04_process_pool.js) | Use a process pool to manage concurrent tasks         |
| 5   | [queue_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/05_queue_communication.js) | Communicate between processes using a Queue           |
| 6   | [pipe_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/06_pipe_communication.js) | Communicate between processes using a Pipe            |
| 7   | [shared_value.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/07_shared_value.js) | Use a shared value to store data between processes    |
| 8   | [shared_array.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/08_shared_array.js) | Use a shared array to store data between processes    |
| 9   | [manager.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/09_manager.js) | Use a manager to share complex data structures        |
| 10  | [process_lock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/10_process_lock.js) | Use a lock to synchronize access to shared resources  |
| 11  | [process_semaphore.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/11_process_semaphore.js) | Use a semaphore to control access to shared resources |
| 12  | [process_barrier.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/12_process_barrier.js) | Use a barrier to synchronize multiple processes       |
| 13  | [orphan.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/13_orphan.js) | Demonstrate an orphan process scenario                |
| 14  | [zombie.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/14_zombie.js) | Demonstrate a zombie process scenario                 |
