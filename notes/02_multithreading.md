## Multithreading

* Multithreading is a way of achieving concurrency in software development.
* It involves creating multiple threads within a single process to execute tasks concurrently.
* Multithreading does not guarantee parallelism.

### Thread pool vs On-demand thread

* Two ways to create threads in multithreading are: using a thread pool or on-demand thread spawning.
* Thread pool pre-spawns threads to reduce creation costs, while on-demand thread spawning reduces resource wastage.
* On-demand thread spawning may slow down the program when threads are needed.

### Worker threads

* In multithreading, one thread initiates all other threads, known as worker threads.
* Worker threads only perform tasks when allocated to them by another thread.
* To regulate and limit the number of worker threads, a thread pool can be employed.

A web server process, for example, receives a request and assigns it to a thread from its pool for processing. That thread obeys the main thread, completes the task, and returns to the pool. Until then, the main thread is free to do anything it wants. 

### Advantages of threads over processes

1. Multithreading has several advantages over using multiple processes, such as:
    - Better responsiveness.
    - Faster context transitions.
    - Improved resource sharing.
2. Code, data, and files can be shared across all threads within a process.

### Challenges with multithreading

* Multithreading challenges arise from the fact that threads share state with one another, making communication simple.
* The most common challenge when dealing with multithreading is data race.
* Data race happens when the result of a multithreaded program depends on the order in which threads are executed.
* This is because threads switch preemptively, meaning that you don't control when they switch, and a switch may occur at an inconvenient moment.
* When multiple threads use the same resources, data race becomes increasingly dangerous.
* Mutex and semaphore are mechanisms developed to prevent data race.

#### Data race

When in multithreaded program, the result is depndend on the order in which the threads are executed, then we refer to that stuation as 'race condition'. 

Why does it happen? By defualt threads switch preemptively (you don't control when the threads switch, the os can make the switch at any time). This is useful since you don't have to manually code where the task should switch. The tradeoff of this convenience is that you must presume that a switch may occur at an inconvenient moment.

Consider the following simple example: we have two functions, *funA()* and *funB()*, and *funB()* is dependent on the results of *funA()*.
If we write a single threaded program, all we have to do is call the functions in the correct order:

```bash
funA()
funB()
```
If we delegate both functions to separate threads, this solution will fail.
We don't know which function will be chosen to execute first, or whether they will run in parallel.
In either case, our software will not function properly.

When many threads use the same resources, the scenario becomes increasingly dangerous.

* Data race occurs when at least two concurrent threads access the same memory address that's being modified by at least one thread.
* That section of memory may get corrupted, and crucial areas must be protected with locks.

Analogy: Imagine a crowded restaurant kitchen where multiple chefs are working on the same dish at the same time, using the same ingredients and tools. If they are not coordinated properly, they might end up bumping into each other or accidentally using the same tool or ingredient at the same time, causing confusion and potentially ruining the dish. This is similar to what happens in a data race, where multiple threads are accessing the same memory location at the same time without proper synchronization, leading to unpredictable and potentially incorrect results.

#### Mutex

* A mutex is a mechanism that prevents data race.
* Multiple threads or processes can take turns sharing the same resource without conflict.
* While one thread is allowed to use the resources, other requesting threads are put to sleep until the thread exits the portion of code guarded by the mutex.

Analogy: Think of a public restroom with only one stall. If multiple people try to use the stall at the same time, chaos will ensue, with people pushing and shoving, and no one getting to use the restroom properly. To prevent this, a lock is installed on the door, which can only be opened by one person at a time. This ensures that only one person can use the stall at any given time, and others have to wait their turn. In the same way, a mutex is a lock that threads can use to access a shared resource in a mutually exclusive way.

#### Semaphore

* A semaphore is an integer variable that can only be accessed via two atomic operations: wait and signal.
* Semaphores are used for synchronization.
* Changes to the semaphore value in the wait and signal actions must be carried out independently.
* A binary semaphore should be used as a signaling technique, where the binary "producer" informs all of the "consumers" that what they were expecting has occurred.
    
Analogy: Imagine a busy street intersection with a traffic light. The traffic light controls the flow of traffic by changing colors at regular intervals, and different lanes of traffic have to take turns moving through the intersection. A semaphore works in a similar way, controlling access to a shared resource by allowing a certain number of threads to access it at a time, and blocking others until there is available capacity. The semaphore acts as a signal to the threads, letting them know when it is safe to access the shared resource.
    
#### Common miconceptions

* Binary semaphore and mutex are the same thing: A binary semaphore can only accept one of two values, and it serves the same function as a mutex. However, the prevalent misunderstanding is that they are the same thing. A mutex is used to gain exclusive access to a resource, while a binary semaphore should be used as a signaling technique. The binary "producer" merely informs all of the "consumers" that what they were expecting has occurred.
* Multithreading automatically improves performance: While multithreading can improve performance in certain scenarios, it does not always lead to faster execution times. In fact, if not implemented correctly, multithreading can even decrease performance due to increased overhead and synchronization costs.
* More threads always means better performance: This is not necessarily true. Creating too many threads can actually lead to decreased performance due to increased context switching overhead and resource contention.
* Multithreaded code is always more difficult to write and maintain: While writing correct and efficient multithreaded code can be challenging, it is not always more difficult than writing single-threaded code. Additionally, many modern programming languages and frameworks provide abstractions and tools to simplify multithreaded programming.
    
### Examples

#### Examples in C++

Every C++ application has one default main thread, which is represented by the `main()` function. Since C++11, we may spawn more threads by creating instances of the `std::thread` type. The constructor of that class receives a callback to the function to be called from the new thread as well as a list of arguments for that function. When an object is created, the thread begins executing.

There are two alternatives for handling the created threads: joining or detaching them.

When we use `join()`:
* The program will wait for the thread to complete before continuing its execution.
* Any resources belonging to the thread will be cleaned up.

When we use `detach()`:
* The program does not wait for the threads to complete their execution before proceeding to the next instruction.
* When the program terminates, all remaining detached threads are suspended and their objects are destroyed.

To compile the C++ examples, use the `GCC` compiler with the following flags:

```
g++ file_name.cpp -std=c++17 -pthread -o executable_name
```

Here are some example code snippets demonstrating various aspects of multithreading in C++:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/01_single_worker_thread.cpp">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread using `std::thread`.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/02_thread_subclass.cpp">thread_subclass</a>        | Demonstrate how to create a custom thread class by inheriting `std::thread`.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/03_multiple_worker_threads.cpp">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads using `std::thread`.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/04_race_condition.cpp">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications using C++ examples.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/05_mutex.cpp">mutex</a>    | Illustrate the use of `std::mutex` to protect shared resources and avoid race conditions in C++ applications. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/06_semaphore.cpp">semaphore</a>           | Demonstrate the use of `std::counting_semaphore` to limit the number of concurrent threads accessing a shared resource in C++ applications. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/07_producer_consumer.cpp">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using C++ synchronization mechanisms like `std::mutex` and `std::condition_variable`. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/08_fetch_parallel.cpp">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources using C++ threads. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/09_merge_sort.cpp">merge_sort</a>           | Use multi-threading in C++ to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/10_schedule_every_n_sec.cpp">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using C++ threads. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/11_barrier.cpp">barrier</a>          | Demonstrate the use of `std::barrier` to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/12_thread_local_storage.cpp">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/13_thread_pool.cpp">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/14_reader_writer_lock.cpp">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

#### Examples in Python

A global interpreter lock (GIL) is a technique used in programming language interpreters to synchronize thread execution such that only one native thread may run at any given moment. Even when running on a multi-core CPU, GIL permits only one native thread to execute at a time.

The natural question is if it even makes sense to use threads in Python? The answer is, it depends. We can still make greater use of the CPU that is idle while waiting for I/O by using multithreading. We can increase performance by overlapping the waiting time for requests. We should however look at multiprocessing if we want to split CPU-intensive activities over multiple CPU cores.

The main module we'll be using is named `threading`. To spawn a new thread, just create an object of `Thread` class and specify the function that you want to run in the new thread as the first parameter. The thread will not begin executing until you invoke the `start()` method. Call the `join()` method to suspend further program execution until the thread has completed its task. There are several other functions available that make it simple to work with threads. For example, to see which thread is currently executing, we can call `current_thread()`. We use `main_thread()` to obtain the main thread instance.

Here are some example code snippets demonstrating various aspects of multithreading in Python:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/01_single_worker_thread.py">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/02_thread_subclass.py">thread_subclass</a>        | Demonstrate how to create a custom thread class by subclassing `Thread`.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/03_multiple_worker_threads.py">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/04_race_condition.py">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/05_mutex.py">mutex</a>    | Illustrate the use of mutexes to protect shared resources and avoid race conditions. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/06_semaphore.py">semaphore</a>           | Demonstrate the use of semaphores to limit the number of concurrent threads accessing a shared resource. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/07_producer_consumer.py">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using synchronization mechanisms like mutexes and condition variables. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/08_fetch_parallel.py">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/09_merge_sort.py">merge_sort</a>           | Use multi-threading to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/10_schedule_every_n_sec.py">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using threads. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/11_barrier.py">barrier          | Demonstrate the use of barriers to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/12_thread_local_storage.py">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/13_thread_pool.py">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/14_reader_writer_lock.py">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

#### Examples in JavaScript (Node.js)

Keep in mind that JavaScript is designed to be single-threaded, and while there are ways to spawn separate threads using Web Workers or child processes, the primary way to handle concurrency in JavaScript is through asynchronous programming using callbacks, Promises, and async/await.

By default, a Node.js application operates on a single thread. An event loop in this thread listens for events and then calls the event's related callback function when one is detected. Since V10.5, the `worker_threads` module can be used to spawn extra threads. This module enables the use of threads that execute JavaScript in parallel.

Be aware that Node.js already internally handles I/O operations through the usage of a thread pool. Spawning a thread makes sense only for CPU-intensive work.

To use the `worker_threads` module, you need to create a separate JavaScript file containing the code to be executed in the worker thread. To create a worker thread, use the `Worker` constructor and pass the file path as its first parameter.

```javascript
const { Worker } = require('worker_threads');

const worker = new Worker('./path/to/worker.js');
```

Inside the worker file, you can use the `parentPort` object to communicate with the main thread. For example, you can use the `postMessage()` method to send messages from the worker to the main thread.

```javascript
// worker.js
const { parentPort } = require('worker_threads');

parentPort.postMessage('Hello from the worker thread!');
```

In the main thread, you can listen for messages from the worker by attaching a listener to the message event.

```javascript
worker.on('message', (message) => {
  console.log(`Received message from worker: ${message}`);
});
```

Here are some example code snippets demonstrating various aspects of multithreading in JavaScript (Node.js):


| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/01_single_worker_thread.js">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread using Web Workers.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/02_thread_subclass.js">thread_subclass</a>        | Demonstrate how to create a custom thread class by extending the `Worker` class.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/03_multiple_worker_threads.js">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads using Web Workers.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/04_race_condition.js">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications using JavaScript examples.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/05_mutex.js">mutex</a>    | Illustrate the use of `Atomics` and `SharedArrayBuffer` to protect shared resources and avoid race conditions in JavaScript applications. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/06_semaphore.js">semaphore</a>           | Demonstrate the use of semaphores to limit the number of concurrent threads accessing a shared resource in JavaScript applications using `Atomics` and `SharedArrayBuffer`. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/07_producer_consumer.js">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using JavaScript synchronization mechanisms like `Atomics` and `SharedArrayBuffer`. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/08_fetch_parallel.js">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources using Web Workers. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/09_merge_sort.js">merge_sort</a>           | Use multi-threading in JavaScript to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/10_schedule_every_n_sec.js">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using JavaScript and Web Workers. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/11_barrier.js">barrier</a>          | Demonstrate the use of barriers to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/12_thread_local_storage.js">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/13_thread_pool.js">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/14_reader_writer_lock.js">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

