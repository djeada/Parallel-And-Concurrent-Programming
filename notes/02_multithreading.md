## Multithreading

Multithreading is a method of achieving concurrency by creating several threads within a single process. It is important to note that parallelism is not guaranteed by multithreading.

### Thread pool vs On-demand thread

To reduce creation costs, a thread pool pre-spawns threads. Threads are created once rather than multiple times.

In contrast, on-demand thread spawning reduces the waste of resources spent by idle prespawned threads. On the other side, it might cause the program to slow down when the threads are needed. 

### Worker threads

In multithreading, we typically have one main thread that initiates all other threads. These new threads are referred to as "worker threads." They are named thus because they are waiting for a job and only do the task when it is allocated to them by someone else. To regulate and limit the number of worker threads, you may wish to employ a thread pool.

A web server process, for example, receives a request and assigns it to a thread from its pool for processing. That thread obeys the main thread, completes the task, and returns to the pool. Until then, the main thread is free to do anything it wants. 

### Advantages of threads over processes

1. Responsiveness: If a process is splited into threads, when one thread completes its execution, its output may be used instantly.

2. Faster context transition: The time it takes to switch between threads is shorter than the time it takes to move between processes.

3. Resource sharing: Code, data, and files can be shared across all threads within a process.

### Challenges with multithreading

The irony is that most multithreading challenges are coming from its biggest advantage. Threads are favored over processes because they share state with one another, making communication simple. This fact is also the reason for most common challenges when dealing with multithreading. 

#### Data race

When in multithreaded program, the result is depndend on the order in which the threads are executed, then we refer to that stuation as 'race condition'. 

Why does it happen? By defualt threads switch preemptively (you don't control when the threads switch, the os can make the switch at any time). This is useful since you don't have to manually code where the task should switch. The tradeoff of this convenience is that you must presume that a switch may occur at an inconvenient moment.

Consider the following simple example: we have two functions, *funA()* and *funB()*, and *funB()* is dependent on the results of *funA()*.
If we write a single threaded program, all we have to do is call the functions in the correct order:

    funA()
    funB()
    
If we delegate both functions to separate threads, this solution will fail.
We don't know which function will be chosen to execute first, or whether they will run in parallel.
In either case, our software will not function properly.

When many threads use the same resources, the scenario becomes increasingly dangerous. Given the following:

* At least two concurrent threads access the same memory address.
* It's being modified by at least one thread.

That section of memory may even get corrupted. As a result, crucial areas must be protected with locks.

#### Mutex

A mutex is a mechanism develop to prevent data races. Multiple threads (or processes) can take turns sharing the same resource without conflict.
While one thread is allowed to use the resources, other requesting threads are put to sleep until thread exits the portion of code guarded by the mutex.

#### Semaphore

Semaphores are used for synchronization (i.e "Hello there, function foo()! Did you know that something occurred just now?"). A semaphore is an integer variable that can only be accessed via two atomic operations: wait and signal. Changes to the sempahore value in the wait and signal actions must be carried out independently. That is, when one process changes a semaphore value, no other process can change the same semaphore value at that time.

#### Common miconceptions

Because a binary semaphore can only accept one of two values, it serves the same function as a mutex. The prevalent misunderstanding is that they are the same thing.

Mutex is used to gain exclusive access to a resource. A binary semaphore should be used as a signaling technique. The binary "producer" merely informs all of the "consumers" that what they were expecting has occurred. 

### Examples

#### Examples in C++

Every C++ application has one default main thread, which is represented by the <code>main()</code> function. Since C++11, we may spawn more threads by creating instances of the <code>std::thread</code> type. The constructor of that class receives a callback to the function to be called from the new thread as well as a list of arguments for that function. When an object is created, the thread begins executing.

Then we have two alternatives. We have the option of joining or detaching the thread. 

When we use <code>join()</code>:

* The program will wait for the thread to complete before continuing its execution. 
* Any resources belonging to the thread will be cleaned up.

When we use <code>detach()</code>:

* The program does not wait for the threads to complete their execution before proceeding to the next instruction.
* When the program terminates, all remaining detached threads are suspended and their objects are destroyed. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/raw_pthread.cpp">raw pthread</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/single_worker_thread.cpp">single worker thread</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/multiple_worker_threads.cpp">multiple worker threads</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/race_condition.cpp">race condition</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/mutex.cpp">mutex</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/semaphore.cpp">semaphore</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multithreading/merge_sort.cpp">merge sort</a>

#### Examples in Python

A global interpreter lock (GIL) is a technique used in programming language interpreters to synchronize thread execution such that only one native thread may run at any given moment. Even when running on a multi-core CPU, GIL permits only one native thread to execute at a time.

The natural question is if it even makes sense to use threads in Python? The answer is, it depends. We can still make greater use of the CPU that is idle while waiting for I/O by using multihreading. We can increase performance by overlapping the waiting time for requests. We should however look at multiprocessing if we want to split CPU-intensive activities over multiple CPU cores.

The main module we'll be using is named <code>threading</code>. To spawn  a new thread, just create an object of <code>Thread</code> class and specify the function that you want to run in the new thread as the first parameter. The thread will not begin executing until you invoke the <code>start()</code> method. Call the <code>join()</code> method to suspend further program execution until the thread has completed its task. There are several other functions available that make it simple to work with threads. For example to see which thread is currently executing we can call <code>current_thread()</code>. We use <code>main_thread()</code> to obtain the main thread instance.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/single_worker_thread.py">single worker thread</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/multiple_worker_threads.py">multiple worker threads</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/thread_subclass.py">thread subclass</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/race_condition.py">race condition</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/mutex.py">mutex</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/semaphore.py">semaphore</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/merge_sort.py">merge sort</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/fetch_parallel.py">fetch parallel</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multithreading/schedule_every_n_sec.py">schedule every n sec</a>

#### Examples in JavaScript

By default, a NodeJs application operates on a single thread. An event loop in this thread listens for events and then calls the event's related callback function when one is detected. Since V10.5, the <code>worker_threads</code> module can be used to spawn extra threads. 

Be aware that NodeJS already internally handles I/O operations through the usage of a thread pool. Spawning a thread makes only sense for a CPU-intensive work.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multithreading/single_worker_thread/main.js">single worker thread</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multithreading/multiple_worker_threads/main.js">multiple worker threads</a>
