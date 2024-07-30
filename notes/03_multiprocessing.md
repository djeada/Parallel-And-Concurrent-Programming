## Multiprocessing

**Multiprocessing** involves running multiple processes simultaneously. Each process has its own memory space, making them more isolated from each other compared to threads, which share the same memory. This isolation means that multiprocessing can be more robust and less prone to errors from shared state, as each process runs independently. Multiprocessing is often used to leverage multiple CPU cores, allowing a program to perform computationally intensive tasks in parallel, thus improving performance. Communication between processes is typically achieved through inter-process communication (IPC) mechanisms, such as pipes, sockets, or shared memory. While more resource-intensive than multithreading, due to the need for separate memory spaces, multiprocessing can achieve better performance for CPU-bound tasks and provides better fault isolation.

### Child Processes

The process that spawns new processes is referred to as the parent process. The processes that have been spawned are known as child processes.

The parent process may:

- Wait for its child to finish;
- Check on the child's condition (it may be running, sleeping, stopped, or zombie);
- Communicate with the child and request some data back from it;
- Terminate the child if it's taking too long.

#### Zombie Process

A zombie process is one that has finished running but still has an entry in the process table to report to its parent process. A child process always becomes a zombie before being removed from the process table. The parent process reads the exit status of the child process and removes the child process entry from the process table. The zombie isn't taking up much memory or resources; it's just an exit status waiting to be delivered. However, too many zombies might significantly slow down the system.

#### Orphan Process

An orphan process is one whose parent process has finished execution or been terminated without waiting for its child process to finish. An orphan is alive and running, just like any other process; it just has a peculiar name.

### Communication between Processes

To fully take advantage of multiprocessing, we need to have a way to communicate between the processes.

#### Message Passing

Message passing exchanges data via techniques known as inter-process communication (IPC). On the plus side, it is applicable to both local and remote communication (multiprocessing programs can be distributed between remote machines). One disadvantage is a possible "loop-back" configuration, which may result in excessive overhead for large messages.

#### Shared Memory

Shared memory enables programs to access and share data as though they were local to each process's address space. This, however, does not happen automatically, and we must explicitly request the OS to allow two processes to share memory. On the plus side, in a loop-back design, it may be more efficient for large data. One disadvantage is that it does not generalize well to remote systems and may be more error-prone.

### Challenges with Multiprocessing

The independence of processes from one another is their strength. It's also its weakest point because processes don't communicate easily with one another.

#### Debugging

Even for single-process applications, debugging may be a time-consuming operation. We often have to run the code line by line to figure out where the bug is hidden. The more processes there are, the more complex the whole operation becomes. Because the debugger can only track one process at a time, you must specify which one it should be. You may not be aware of which one is causing the issues. In that case, you may need to repeat the operation for each process.

#### Deadlocks

A deadlock occurs when two or more processes wait for each other to complete, and none of them ever do. Consider the following scenario: during the coronavirus pandemic, many places, including mask retailers, required you to wear a mask. But what could you do if you didn't have a mask? To enter the shop, you must wear a mask, which you had to buy from the shop in the first place.

What causes a deadlock to occur?

* Mutual exclusion occurs when a resource is not available for sharing.
* A circular wait is a collection of processes that wait for each other in a circular pattern.
* No preemption is the situation when the operating system may not recover a resource from a process until the process returns it.
* Hold and wait refers to when a process holds at least one resource while waiting for another waiting process to hold another resource.

### Containers

An alternative to a single multiprocess program is a microservice architecture using containers, each of which executes a single-process program. There is a plethora of technologies available that make container orchestration as easy as ABC. The obvious question is how long your child processes should live. If they are only supposed to live for a short time, this approach is excessive.

### Process Synchronization

In a multiprocessing environment, multiple processes may need to access shared resources, such as files or memory. To prevent conflicts and ensure that the processes operate correctly, they must be properly synchronized. Synchronization techniques include the use of locks, semaphores, and monitors to control access to shared resources and ensure that only one process can access a resource at a time.

### Load Balancing

In a multiprocessing system, it's essential to distribute tasks evenly among the available processes to maximize resource utilization and minimize waiting time. Load balancing algorithms help to ensure that the workload is distributed evenly across the processes, taking into account factors such as the current load on each process, the priority of tasks, and the availability of resources.

### Scalability

One of the key advantages of multiprocessing is the ability to scale up an application to handle an increased workload. As more processes or computing resources are added, the application should be able to handle more tasks simultaneously, resulting in improved performance and throughput. Designing an application to scale effectively requires careful planning, including the choice of appropriate algorithms, data structures, and communication protocols.

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
