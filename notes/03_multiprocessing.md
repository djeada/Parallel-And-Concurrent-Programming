## Multiprocessing

Multiprocessing is a method of achieving concurrency by creating several processes within a single program. It is important to note that parallelism is not guaranteed by multiprocessing. Processes, unlike threads, do not share any resources by default, hence it makes the most sense to use that approach when you wish to simultaneously complete unrelated tasks.

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

To create new processes in C++, use operating system-specific APIs:
* In Windows: `CreateProcess()`
* In Linux: `fork()`

UNIX-based operating systems use a single process that spawns all other processes. When you call `fork()`, you create a clone of the current process. Everything is copied, except the process ID (PID).

To make the parent process wait for the child process to complete, use the `waitpid` function.

To determine if a program is running in the child or parent process, check the return value of `fork()`. The child receives zero, while the parent receives the child's PID.

To replace the child process with a different program, use the `exec*()` family of functions. The `exec()` function replaces the current process with the specified program.

To compile the examples, use the GCC compiler with the following flags:

```
g++ file_name.cpp -std=c++17 -pthread -o executable_name
```
| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [01_basic_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/01_basic_process.cpp) | Create and start a basic process                      |
| 2   | [02_multiple_processes.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/02_multiple_processes.cpp) | Integrate multiple processes for a complex task       |
| 3   | [03_deadlock.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/03_deadlock.cpp) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [04_process_pool.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/04_process_pool.cpp) | Use a process pool to manage concurrent tasks         |
| 5   | [05_queue_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/05_queue_communication.cpp) | Communicate between processes using a Queue           |
| 6   | [06_pipe_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/06_pipe_communication.cpp) | Communicate between processes using a Pipe            |
| 7   | [07_shared_value.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/07_shared_value.cpp) | Use a shared value to store data between processes    |
| 8   | [08_shared_array.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/08_shared_array.cpp) | Use a shared array to store data between processes    |
| 9   | [09_manager.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/09_manager.cpp) | Use a manager to share complex data structures        |
| 10  | [10_process_lock.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/10_process_lock.cpp) | Use a lock to synchronize access to shared resources  |
| 11  | [11_process_semaphore.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/11_process_semaphore.cpp) | Use a semaphore to control access to shared resources |
| 12  | [12_process_barrier.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/12_process_barrier.cpp) | Use a barrier to synchronize multiple processes       |
| 13  | [13_producer_consumer_multiprocess.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/13_producer_consumer_multiprocess.cpp) | Implement a producer-consumer pattern with processes  |
| 14  | [14_orphan.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/14_orphan.cpp) | Demonstrate an orphan process scenario                |
| 15  | [15_zombie.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/15_zombie.cpp) | Demonstrate a zombie process scenario                 |

#### Examples in Python

In Python, you can use the `os`, `subprocess`, and `multiprocessing` modules to create and manage processes.

The `os` module provides the `fork()` function, while the `subprocess` module is used for creating independent processes. The `multiprocessing` module offers a higher level of abstraction and is used for running subtasks of your program and allowing them to communicate easily.

To start a new process in the `multiprocessing` module, use the `Process` class with the callback function as the first argument. Use the `start()` method to begin process execution, and the `join()` method to wait for process completion.

The `Pool` class in the `multiprocessing` module allows running a function in parallel with different sets of inputs.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [01_basic_process.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/01_basic_process.py) | Create and start a basic process                      |
| 2   | [02_multiple_processes.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/02_multiple_processes.py) | Integrate multiple processes for a complex task       |
| 3   | [03_deadlock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/03_deadlock.py) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [04_process_pool.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/04_process_pool.py) | Use a process pool to manage concurrent tasks         |
| 5   | [05_queue_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/05_queue_communication.py) | Communicate between processes using a Queue           |
| 6   | [06_pipe_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/06_pipe_communication.py) | Communicate between processes using a Pipe            |
| 7   | [07_shared_value.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/07_shared_value.py) | Use a shared value to store data between processes    |
| 8   | [08_shared_array.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/08_shared_array.py) | Use a shared array to store data between processes    |
| 9   | [09_manager.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/09_manager.py) | Use a manager to share complex data structures        |
| 10  | [10_process_lock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/10_process_lock.py) | Use a lock to synchronize access to shared resources  |
| 11  | [11_process_semaphore.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/11_process_semaphore.py) | Use a semaphore to control access to shared resources |
| 12  | [12_process_barrier.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/12_process_barrier.py) | Use a barrier to synchronize multiple processes       |
| 13  | [13_producer_consumer_multiprocess.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/13_producer_consumer_multiprocess.py) | Implement a producer-consumer pattern with processes  |
| 14  | [14_orphan.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/14_orphan.py) | Demonstrate an orphan process scenario                |
| 15  | [15_zombie.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/15_zombie.py) | Demonstrate a zombie process scenario                 |

#### Examples in JavaScript

In Node.js, you can create child processes using the following functions: `spawn()`, `fork()`, `exec()`, and `execFile()`.

The `spawn()` function returns a `childObject` that can be used to listen for events. The available events include 'error', 'exit', 'close', 'disconnect', and 'message'.

To use multiprocessing in Node.js, you can leverage the `cluster` module. This module allows you to harness the power of multicore systems by using processes.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [01_basic_process.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/01_basic_process.js) | Create and start a basic process                      |
| 2   | [02_multiple_processes.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/02_multiple_processes.js) | Integrate multiple processes for a complex task       |
| 3   | [03_deadlock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/03_deadlock.js) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [04_process_pool.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/04_process_pool.js) | Use a process pool to manage concurrent tasks         |
| 5   | [05_queue_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/05_queue_communication.js) | Communicate between processes using a Queue           |
| 6   | [06_pipe_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/06_pipe_communication.js) | Communicate between processes using a Pipe            |
| 7   | [07_shared_value.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/07_shared_value.js) | Use a shared value to store data between processes    |
| 8   | [08_shared_array.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/08_shared_array.js) | Use a shared array to store data between processes    |
| 9   | [09_manager.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/09_manager.js) | Use a manager to share complex data structures        |
| 10  | [10_process_lock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/10_process_lock.js) | Use a lock to synchronize access to shared resources  |
| 11  | [11_process_semaphore.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/11_process_semaphore.js) | Use a semaphore to control access to shared resources |
| 12  | [12_process_barrier.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/12_process_barrier.js) | Use a barrier to synchronize multiple processes       |
| 13  | [13_producer_consumer_multiprocess.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/13_producer_consumer_multiprocess.js) | Implement a producer-consumer pattern with processes  |
| 14  | [14_orphan.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/14_orphan.js) | Demonstrate an orphan process scenario                |
| 15  | [15_zombie.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/15_zombie.js) | Demonstrate a zombie process scenario                 |
