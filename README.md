# Parallel and Concurrent Programming
Unveiling the mysteries of parallel and concurrent programming.

## Table of Contents
<!--ts-->

  - [Basic-terminology](#Basic-terminology)
    - [Sequential-vs-Nonsequential](#Sequential-vs-Nonsequential)
    - [Concurency-vs-Parallelism](#Concurency-vs-Parallelism)
    - [Synchronous-vs-Asynchronous](#Synchronous-vs-Asynchronous)
    - [Process](#Process)
      - [Role-of-the-OS](#Role-of-the-OS)
      - [States-of-the-process](#States-of-the-process)
    - [Thread](#Thread)
      - [The-role-of-the-OS](#The-role-of-the-OS)
    - [Process-vs-Thread](#Process-vs-Thread)
    - [CPU-Bound-vs-I/O-Bound](#CPU-Bound-vs-I/O-Bound)
      - [CPU-Bound](#CPU-Bound)
      - [I/O-Bound](#I/O-Bound)
  - [Multithreading](#Multithreading)
    - [Thread-pool-vs-On-demand-thread](#Thread-pool-vs-On-demand-thread)
    - [Worker-threads](#Worker-threads)
    - [Advantages-of-threads-over-processes](#Advantages-of-threads-over-processes)
    - [Challenges-with-multithreading](#Challenges-with-multithreading)
      - [Data-race](#Data-race)
      - [Mutex](#Mutex)
      - [Semaphore](#Semaphore)
      - [Common-miconceptions](#Common-miconceptions)
    - [Examples](#Examples)
      - [Examples-in-C++](#Examples-in-C++)
      - [Examples-in-Python](#Examples-in-Python)
      - [Examples-in-JavaScript](#Examples-in-JavaScript)
  - [Multiprocessing](#Multiprocessing)
    - [Child-processes](#Child-processes)
      - [Zombie-Process](#Zombie-Process)
      - [Orphan-Process](#Orphan-Process)
    - [Communication-between-processes](#Communication-between-processes)
      - [Message-Passing](#Message-Passing)
      - [Shared-Memory](#Shared-Memory)
    - [Challenges-with-multiprocessing](#Challenges-with-multiprocessing)
      - [Debugging](#Debugging)
      - [Deadlocks](#Deadlocks)
    - [Containers](#Containers)
    - [Examples](#Examples)
      - [Examples-in-C++](#Examples-in-C++)
      - [Examples-in-Python](#Examples-in-Python)
      - [Examples-in-JavaScript](#Examples-in-JavaScript)
  - [Asynchrony](#Asynchrony)
    - [Building-blocks-of-asynchronous-programming](#Building-blocks-of-asynchronous-programming)
      - [Event-loop](#Event-loop)
      - [Corutines](#Corutines)
      - [Futures](#Futures)
    - [Asynchrony-vs-multithreading](#Asynchrony-vs-multithreading)
    - [Challenges](#Challenges)
    - [Examples](#Examples)
      - [Examples-in-C++](#Examples-in-C++)
      - [Examples-in-Python](#Examples-in-Python)
      - [Examples-in-JavaScript](#Examples-in-JavaScript)
  - [MPI](#MPI)
  - [Rules-of-thumb](#Rules-of-thumb)
  - [Hardware](#Hardware)
    - [A-single-core-CPU](#A-single-core-CPU)
    - [A-multi-core-CPU](#A-multi-core-CPU)
    - [Graphics-Processing-Unit-(GPU)](#Graphics-Processing-Unit-(GPU))
    - [Shared-memory-architectures](#Shared-memory-architectures)
  - [Bibliography](#Bibliography)

<!--te-->

## Basic terminology

Let's start by defining some key terms and emphasizing the distinctions between related concepts. In general those concepts are universal and may be applied to any programming language. The differences between the languages will be discussed in greater detail later, when we attempt to explain the specific approach in implementing concurrency.

### Sequential vs Nonsequential

Sequential:

* Tasks are carried out in the order they occur in the code.
* Depend on being executed step-by-step to produce correct results.

Non-sequential:

* Tasks do not have to be executed in the order in which they appear in the code. 
* Step-by-step execution is not necessary for correct results.

### Concurency vs Parallelism

The execution of two or more instructions at the same time is referred to as parallel execution.
Concurrency does not indicate non-parallel or parallel; it simply implies the ability to divide a program into several pieces and re-order them arbitrarily so that each can be processed by a distinct thread, but it says nothing about running in parallel or not.

Concurency:

* Refers to the program structure. 
* Means DEALING with multiple things at the same time.
* Requires scheduling and synchronization.

Parallelism:

* Stands for simultaneous execution.
* Means PERFORMING numerous tasks at the same time.
* Increases throughput.
* Requires parallel hardware and concurrency support.

Note: Throughput is doing more of something at the same time. 

### Synchronous vs Asynchronous

Synchronous:

* When you start one task, you must wait until it is completed before starting the next. 
* Blocking.

Asynchronous:

* Other tasks can be started before the present one is completed. 
* Non-blocking.

### Process

A process is an instance of program in execution. When you use your web browser, for example, there is a process linked with it; the same is true for every other application, such as a text editor or video player. Furthermore, a single application can be connected with several processes. So there is always at least one process, but there might be more. Processes are also used by your operating system to do tasks in the background while you are conducting your work. Of course, modern operating systems can manage several processes at the same time. It does not imply that the CPU is executing instructions from several processes at the same time, but rather that the CPU is constantly switching between processes. However, if the hardware supports it, it is possible that two processes will run in parallel at some point. 

#### Role of the OS

The operating system is in charge of overseeing all processes.

Each process is given:

- time to use the processor,
- share of computer memory,
- access to the hard drive.

The operating system maintains a table known as the process table to keep track of the status of all processes.
Every process is documented in this table, along with the resources it employs and the current status of the process.

#### States of the process

1. Running - The running state indicates that the process has all of the resources it requires for execution and that the operating system has granted it permission to use the processor. At any given moment, only one process can be in that state.

2. Ready - It is awaiting authorization to use the CPU.

3. Waiting - The process of waiting for an external event to occur, such as user input or disk access. 

### Thread

Within a process, a thread is an ordered stream of instructions. A process is composed of one or more threads. 

A thread is provided with its own program counter (PC), register set, and stack space. Threads, unlike processes, are not independent of one another; as a result, threads share their code section, data section, and OS resources such as open files and signals with other threads. Threads are frequently referred to as lightweight processes since they have some of the qualities of processes. That is one of the reasons why dividing a program into multiple processes is a popular method of achieving concurrency. For example a text editor can employ numerous threads, one to format the text, another to receive inputs, and so on. 

#### The role of the OS

The scheduler controls CPU access and switches between threads. Each gets a single timeslice for its work, and if it isn't finished, it must wait for its next turn. Threads have various priorities as well (numbers). The higher the priority, the more often the thread will be run. 

### Process vs Thread

A program's memory allocation is often divided into: 

- Code
- Data
- Stack
- Heap

Cloned processes only share the code portion; everything else is separate. Threads operating within the same process, on the other hand, share all of those segments except the stack. 

Important: Each thread has its own call stack, but the memory on other thread stacks is still accessible, and you could theoretically keep a reference to memory in another thread's local stack frame! 

Process:

* Independent instance;
* Includes code and data;
* Carries considerable state (e.g., ready, running, waiting, or stopped) information;
* Separate adress space;
* No process can directly access the memory of another process;
* Processes communicate each other through inter-process communication.

Thread:

* Independent path of execution;
* Subset of a process;
* Operating system schedules threads for execution;
* All threads within a single process share the same address space and other resources.

### CPU-Bound vs I/O-Bound

To use concruency to speed up a software, we must first identify the bottleneck in our application. Depending on whether the slowdown is caused by I/O activities or an underused CPU, different solutions will be used. 

#### CPU-Bound

CPU-bound refers to a situation in which the time required to accomplish a task is mostly dictated by the speed of the central processor.

* Software solution: Parallelism is required for the improvement of software speed.
* Hardware solution: The higher the clock rate of our CPU, the better the performance of our software. 
     
          I/O waiting
          CPU Processing  ----Task 1---->----Task 2---->
     

#### I/O-Bound

The phrase "I/O bound" refers to a situation in which the time it takes to finish a task is mostly dictated by the time spent waiting for input/output operations to complete.

* Software solution: Concurrency without parallelism may be sufficient to cause improvement.
* Hardware solution: Faster I/O, such as faster memory I/O, hard disk I/O, network I/O, and so on.

          I/O waiting         -----request----->     ------request------>     ------request------>
          CPU Processing  --->                  ---->                    ---->


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

Why does it happen? By defualt threads switch preemptively (you don't control when the threads switch, the os can make the switch at any time). This is useful since you don't have to manually code where the task should switch. The tradeoff of this convenience is that you must presume that a switch will occur at an inconvenient moment.

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

Be aware that NodeJS already handles I/O operations internally through the usage of a thread pool. As a result, spawning additional threads makes only sense for synchronous JS code. 

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multithreading/single_worker_thread/main.js">single worker thread</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multithreading/multiple_worker_threads/main.js">multiple worker threads</a>

## Multiprocessing

Multiprocessing is a method of achieving concurrency by creating several processes within a single program. It is important to note that parallelism is not guaranteed by multiprocessing. Processes, unlike threads, do not share any resources by default, hence it makes the most sense to use that approach when you wish to simulatenously complete unrelated tasks.

### Child processes 

The process that spawns new processes is referred to as the parent process. The processes that have been spawned are known as child processes.

The parent processe may:

- wait till it's child is finished; 
- check on the child's condition (it may be running, sleeping, stopped, or zombie); 
- it may want to communicate with the child and request some data back from it; 
- if it's taking too long, it may may just need to kill the child.

#### Zombie Process

A zombie process is one that has finished running but still has an entry in the process table to report to its parent process. A child process always becomes a zombie before being removed from the process table. The parent process reads the exit status of the child process and removes the child process entry from the process table. The zombie isn't taking up much memory or resources; it's just an exit status waiting to be delivered. However, too many zombies might significantly slow down the system.

#### Orphan Process

An orphan process is one whose parent process has finished execution or been terminated without waiting for its child process to finish. An orphan is alive and running, just like any other process; it just has a peculiar name. 

### Communication between processes

To fully take advantage of multiprocessing we need to have a way to communicate between the processes. 

#### Message Passing

Message passing exchanges data via techniques known as inter-process communication (IPC). On the plus side, it is applicable to both local and remote communication (multiprocessing programs can be distributed between remote machines). One disadvantage is a possible "loop-back" configuration, this may result in excessive overhead for big messages. 
   
#### Shared Memory

Shared memory enables programs to access and share data as though they were local to each process's address space. On the plus side, in a loop-back design, it may be more efficient for massive data. One disadvantage is that it does not generalize well to remote systems and may be more error-prone. 

### Challenges with multiprocessing

The independence of processes from one another is their strength. It's also its weakest point, because processes don't communicate easily with one another.

#### Debugging

Even for single-process applications, debugging may be a time-consuming operation. We often have to run the code line by line to figure out where the bug is hidden. The more processes there are, the more complex the whole operation becomes. Because the debugger can only track one process at a time, you must specify which one it should be. You may not be aware of which one is creating the issues. In that case you may need to repeat the operation  for each process.

#### Deadlocks

A deadlock occurs when two or more processes wait for each other to complete and none of them ever do. Consider the following scenario: during the coronavirus pandemic, many places, including mask retailers, forced you to wear a mask. But what could you do if you didn't have a mask?To enter the shop, you must wear a mask, which you had to buy from the shop in the first place. 

What causes a deadlock to occur?

* Mutual exclusion occurs when a resource is not available for sharing.
* A circular wait is a collection of processes that wait for each other in a circular pattern.
* No preemption is the situation when the operating system may not recover a resource from a process until the process returns it.
* Hold and wait refers to when a process holds at least one resource while waiting for another waiting process to hold another resource. 

### Containers

An alternative to a single multiprocess program is a microservice architecture using containers, each of which executes a single process program. There is a plethora of technologies available that make container orchestration as easy as ABC.

### Examples

#### Examples in C++

How to create new processes? 

The C++ standard library only defines concurrent execution synchronization for threads. We must use OS-specific API: 

* In Windows: <code>CreateProcess()</code>
* In Linux: <code>fork()</code>

In every UNIX-based operating system, all processes are created from a single process. The first process is initiated when your system boots up. 

All of the processes that are currently running on your machine are clones of that original process.
When you call <code>fork()</code>', you are just requesting that a clone of your process be created. Everything is copied from the parent process, except for the PID. 

The function <code>waitpid</code> is commonly called in the parent process and causes a process to wait for the execution of a specified process to complete before continuing. 

How do we check if the program in use is the child or the parent?
You may examine the <code>fork()</code>'s return value. It returns different results to the parent and the child. The child receives zero, whereas the parent receives the child's PID. 

When creating a new process, you usually don't want it to do the same thing as the parent.
You spawn the child process to perform a specific task while the parent is busy doing something else.

How do you make a child process run a different program? 

* <code>exec*()</code> family of functions is used to replace an existing process with a new program. 

<code>exec()</code> does not create a new process and run the program in it; instead, it runs the program you provide by replacing it with the existing process. The current process has been terminated. The new one has the same PID as the previous one, but everything else is changed. 

<code>exec()</code> will only return a value if the new program could not be launched for whatever reason. Otherwise, the caller program will simply stop existing, and the new program will run in its place. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/spawn_single_process.cpp">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/spawn_multiple_processes.cpp">spawn multiple processes</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/single_pipe.cpp">single pipe</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/deadlock.cpp">deadlock</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/orphan.cpp">orphan</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/zombie.cpp">zombie</a>

#### Examples in Python

The <code>os</code> module supports the usage of pure <code>fork()</code>, which works in the same way that we discussed while discussing C++ examples. However, there are superior higher level abstraction alternatives. <code>subprocess</code> and <code>multiprocessing</code> are two modules that may be used to create new processes from a Python script. 

Subprocess creates new processes, but you have no way of communicating with them except from stdin/stdout and any additional APIs the other program may use. Its primary use is to start processes that are fully independent of your own program.

Multiprocessing creates new processes as well, but these processes are meant to run some subtask of your program and easily communicate with one another. The multiprocessing module includes primitives for sharing values between processes.

Furthermore, multiprocessing has an API similar to that of the threading module. Use the <code>Process</code> class to start a new process. The first argument should be the callback function. Then, to begin the process execution, use the <code>start()</code> method. Similarly to threads, if you want the rest of your program to wait for the process to complete, use <code>join()</code>.

Pool is another useful multiprocessing construction. It allows users to run one function in parallel with different sets of inputs.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/spawn_single_process.py">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/process_pool.py">process pool</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/data_sharing_queue.py">data sharing queue</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/single_pipe.py">single pipe</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/deadlock.py">deadlock</a>

#### Examples in JavaScript

In NodeJs, there are four functions for creating a child process: <code>spawn()</code>, <code>fork()</code>, <code>exec()</code>, and <code>execFile()</code>.

Spawn returns a <code>childObject</code>, which may later be used to listen for events. The following events are available: 

     Class: ChildProcess
        Event: 'error'
        Event: 'exit'
        Event: 'close'
        Event: 'disconnect'
        Event: 'message'

There are also a number of potentially useful objects from childObject, which are as follows: 

    Class: ChildProcess
        child.stdin
        child.stdout
        child.stderr
        child.stdio
        child.pid
        child.connected
        child.kill([signal])
        child.send(message[, sendHandle][, callback])
        child.disconnect()

Although multithreading is not supported there is a way to harness the power of a multicore system by using processes. Node.js has a module called cluster designed to support a multiprocessing alternative.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/tree/master/src/nodejs/multiprocessing/spawn_single_process">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multiprocessing/spawn_multiples_processes/main.js">spawn multiple processes</a>

## Asynchrony

Asynchrony is unrelated to the previous two concepts (multithreading and multiprocessing). When one task is completed in an asynchronous approach, you may go on to the next without waiting for the preceding task to finish. This is known as nonblocking behavior. When you execute anything synchronously, you wait for it to finish before proceeding to the next task. Concurrency is accomplished with the help of asynchronous programming. Parallelism may be achieved by asynchronous programming in a multi-threaded environment. 

"Synchronously" means "using the same clock," therefore synchronous instructions share the same clock and must execute one after the other. The term "asynchronous" translates to "not utilizing the same clock," which indicates that the instructions are not concerned with being in synchronization with one another. The phrase does not refer to the connection of the instructions to one another. It is referring to the connection of each instruction to the clock. 

For example, retrieving data from a database may take some time, but we don't want our UI to freeze while we wait. As soon as the asynchronous function is invoked, the asynchronous call accepts a call-back reference and returns execution to the client function. While the asynchronous function performs any necessary processing, your UI may continue to respond to the user; after the asynchronous function has completed its task, the client function may use the resultant data (e.g. to display it in the ui). 

### Building blocks of asynchronous programming

#### Event loop

Although it is not always necessary, an event loop is frequently used in the implementation of asynchrony in programming languages. An event loop is a loop that may be used to register tasks to be executed, execute them, delay or cancel them, and handle other events linked to these tasks. The loop executes one function, then stops it and executes another while that function waits for IO. 

#### Corutines

Coroutines are functions that may be stopped and restarted while they are running. They are commonly defined with the <code>async</code> keyword. 

#### Futures

A Future is an item that is designed to hold the outcome of a corutine with which it is related. Future contains state information, which can typically be one of the following:

1. pending - there is no outcome or exception specified for the future.
2. canceled - the future has been canceled.
3. completed - the future was finished, either by a result set or by an exception. 

### Asynchrony vs multithreading

Asynchrony and multithreading often go hand in hand, but the concepts itself are orthogonal to each other and do not have to be implemented together.

1. Synchronous execution with single thread:

          single thread: AAAABBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCC

2. Synchronous execution with many threads:

         thread 1: AAAAAAAAAAAA---------------------------------

         thread 2: ------------BBBBBB--------------------------

         thread 3: ------------------CCCCCCCCCCCCCCCCCCCCCCCCCCC

3. Asynchronous execution with single thread:
   
          single thread: AAAABBBBAAAACCCAAABBBCCCCCBBBBBBBBBCCCCC

4. Asynchronous execution with many threads:

          thread 1: AAAAAAAAA-----
          thread 2: ---BBBBBBBBBBB
          thread 3: ----CCCC------

### Challenges

Asynchronous functions, unlike multithreading, switch cooperatively, hence the programmer is obliged to induce a task switch whenever it is thought suitable. Locks and other forms of synchronization are no longer required. Furthermore, the cost of task shifts is quite minimal. In most cases, calling a pure function incurs more cost than restarting a generator or awaitable. However, asynchronous functions (known as coruitnes) annotated with the async keyword are required. The challenge is, how will you deal with the vast world of functions written in a blocking manner? 

### Examples

#### Examples in C++
The <code>std::async()</code> function template is the simplest approach to make a function call asynchronous. This function receives the name of the function to be called as the first parameter, followed by a variadic list of callback parameters. For example, if we wish to call a function <code>int foo(int x, int y)</code> asynchronously, we may do it as follows: 

    auto future = std::async(foo, 3, 5);  
    auto result = future.get(); 

In this approach we have however very little control over the details. We don't even know if the callback was executed concurrently. There is a way to ensure that the function be executed in a separate thread. The template std::packaged_task wraps a function and provides a future for the functions return value, but the object itself is callable so we can delegate it to a separate thread.

    std::packaged_task<int(double, char, bool)> task(foo);
    auto future = task.get_future();
    std::thread thread(std::move(task), 3, 5);
    auto result = future.get();  
    
The thread starts running after the object gets created. We can either join or detach it.

Another option is to overwrite the default value of the first parameter that specifies how the task should be completed. We have two choices:

1. <code>std::launch::async</code> specifies that the function call must be executed on its own thread.
2. <code>std::launch::deferred</code> implies that we do not want to start a new thread and merely want to guarantee that the function is called before its return value is required. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name
     
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_read_result.cpp">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_status.cpp">future status</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/async_reuses_threads.cpp">async reuses threads</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/merge_sort.cpp">merge sort</a>

#### Examples in Python

When dealing with I/O operations, a multithreaded Python program that employs threading will be idle between the time the request is received and the time the answer is provided. If a thread knows when an I/O request has been delivered, it may switch to another task without becoming idle, and one thread should be sufficient to handle all of these jobs. Threading, obviously, would not be able to achieve this, but we have <code>asyncio</code>.

The main module we'll be using is named <code>asyncio</code>. It is distinguished for its outstanding speed and simplicity of usage. It makes it easy to write single-threaded concurrent programs using corutines (stripped down version of threads). It conceals the complexities of concurrent programming from us by providing a plethora of different functions that implement numerous synchronization strategies for us.

A coroutine can pause function execution by using the keywords <code>yield</code>, <code>yield from</code>, or <code>await</code> in an expression. The function is stopped until the yield statement returns a value.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_until_complete.py">loop run until complete</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_forever.py">loop run forever</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_read_result.py">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_create_task.py">future create task</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/data_sharing_queue.py">data sharing queue</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/mutex.py">mutex</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/semaphore.py">semaphore</a>

#### Examples in JavaScript

* <a href=""></a>
* <a href=""></a>
* <a href=""></a>
* <a href=""></a>

## MPI

MPI is a distributed memory device programming model. Parallelism is achieved by running various processes on different machines. 

### Examples
#### Examples in C++
#### Examples in Python

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/mpi/hello_world_mpi.py">hello world mpi</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/mpi/point_to_point_communication.py">point to point communication</a>

## Rules of thumb

In the preceding sections, I attempted to state the facts as objectively as possible. I'd like to offer some of my personal perspectives on the issue here.

1. Avoid concurrency as much as possible. I think that this is an intriguing issue, however it is difficult to design well and far more difficult to debug. Make sure your programs are both synchronous and sequential.

2. If performance becomes an issue, take the following steps: 
     - If your system's performance is dominated by I/O waits, you should use asynchronous programming.
     - If your system's performance is predicated on CPU-intensive calculation, you should think about employing processes.
     Processes are more isolated than threads and eliminate various kinds of mistakes that might occur in multi-threaded systems.
     - Threads are the ideal solution for obtaining the absolute most CPU speed out of a multi-core computer since they share memory, but this is still dependent on the language implementation, so double-check. 

## Hardware

Different ways of achieving parallelism:

* Within the processor (instruction-level parallelism, multicore). 
* Using several processors in the same machine (multiprocessing). 
* Using various machines (distributed computing, multicomputer).
 
### A single-core CPU

* Every step of the program is converted to a binary instruction(s).

          print(3) ---> 0110 1010 (Hex: 6A)
     
* The codes are tailored to the individual CPU architecture.
* All codes combine to produce an instruction set.
* The program is first loaded into system memory (RAM) from the hard drive.
* The instructions are then sent from system memory to the CPU via a bus.
* Once a program is loaded into the CPU, it moves down the queue (pipeline) of instructions, which are executed one at a time.
* To execute instructions, the CPU employs many components (ALU, for example).
* The clock determines the speed of the CPU. 

### A multi-core CPU

* Intended to handle multithreading easily.
* Hyper-threading was one of the early concepts for improving multithreading efficiency.
* The pipelines for the threads were being duplicated in hyper-threading (it was effectiveliy quicker but not everything could be executed in parallel).
* The term "multi-core" refers to the fact that everything is replicated (pipeline and execution engine). 

### Graphics Processing Unit (GPU)

* Intended to do linear algebra computations efficiently.
* Potentially contain hundreds of cores.

### Shared memory architectures

* All processors act independently, access the same global address space
* Changes in one memory location are visible for all others
* Uniform memory access (UMA)
     - Equal load and store access for all processors to all memory
     - Default approach for majority of SMP systems in the past
* Non-uniform memory access (NUMA) system
     - Delay on memory access according to the accessed region
     - Typically realized by processor interconnection network and local memories
     - Cache-coherent NUMA (CC-NUMA), completely implemented in hardware
     - Became standard approach with recent X86 chips

|                         | Data Parallel / SIMD                             | Task Parallel / MIMD         |
| ------------------------| ------------------------------------------------ | ---------------------------- |
| **Shared Memory (SM)**      | GPU, Cell, SSE, AltiVec, Vector processor        | ManyCore/SMP system          |
| **Distributed Memory (DM)** | processor-array systems, systolic arrays, Hadoop | cluster systems, MPP systems |

## Bibliography

* https://www.personal.kent.edu/~rmuhamma/OpSystems/os.html
* https://www.oreilly.com/library/view/the-art-of/9780596802424/
* https://pybay.com/site_media/slides/raymond2017-keynote/index.html
* https://courses.cs.cornell.edu/cs100/1999su/lectures/lecture17/sld003.htm
* https://inst.eecs.berkeley.edu/~cs152/fa16/lectures/
* https://princetonuniversity.github.io/PUbootcamp/sessions/parallel-programming/
* https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/
* https://lucumr.pocoo.org/2016/10/30/i-dont-understand-asyncio/
* https://www.cs.unb.ca/~bremner/teaching/cs2613/books/nodejs-api/
