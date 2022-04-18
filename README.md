# Parallel and Concurrent Programming
Unveiling the mysteries of parallel and concurrent programming.

## Basic terminology

Let's start by defining some key terms and emphasizing the distinctions between related concepts. Those concepts are universal and may be applied to any programming language. The differences between the languages will be discussed in greater detail later, when we attempt to explain the specific approach in implementing concurrency.

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

- time to use the processor
- computer memory
- disks

The operating system maintains a table known as the process table to keep track of the status of all processes.
Every process is documented in this table, along with the resources it employs and the current status of the process.

#### States of the process

1. Running - The running state indicates that the process has all of the resources it requires for execution and that the operating system has granted it permission to use the processor. At any given moment, only one process can be in that state.

2. Ready - awaiting authorization to utilize the processor

3. Waiting - the process of waiting for an external event to occur, such as user input or disk access. 

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

### CPU-Bound VS I/O-Bound

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
* Hardware solution: Faster I/O, such as faster memory I/O, hard disk I/O, network I/O, and so on, our program's performance will improve.

          I/O waiting         -----request----->     ------request------>     ------request------>
          CPU Processing  --->                  ---->                    ---->


## Multithreading

Multithreading is a method of achieving concurrency by creating several threads within a single process. It is important to note that parallelism is not guaranteed by multithreading.

### Thread pool vs. On-demand thread

To reduce creation costs, a thread pool pre-spawns threads. Threads are created once rather than multiple times.

In contrast, on-demand thread spawning reduces the waste of resources spent by idle prespawned threads. On the other side, it might cause the program to slow down when the threads are needed. 

### Worker threads

In multithreading, we typically have one main thread that initiates all other threads. These new threads are referred to as "worker threads." They are named thus because they are waiting for a job and only do the task when it is allocated to them by someone else. To regulate and limit the number of worker threads, you may wish to employ a thread pool.

A web server process, for example, receives a request and assigns it to a thread from its pool for processing. That thread obeys the main thread, completes the task, and returns to the pool. Until then, the main thread is free to do anything it wants. 

### Advantages of threads over processes

1. Responsiveness: If the process is separated into threads, when one thread completes its execution, its output may be returned instantly.

2. Faster context transition: The time it takes to switch between threads is shorter than the time it takes to move between processes.
Process context shift necessitates greater CPU cost.

3. Resource sharing: Code, data, and files can be shared across all threads in a process.
Each thread, on the other hand, has its own stack and registers. 

4. Effective multiprocessor system utilization: If we have numerous threads in a single process, we can schedule multiple threads on various processors.
This will speed up the process execution.

5. Communication: Because the threads share a common address space, communication between them is simplified.
While in process, we must use a special communication approach to communicate between two processes. 

### Challanges with multithreading

The irony is that multithreading challenges are coming from its biggest advantage. Threads are favored over processes because they share state with one another, making communication simple. This fact is also the reason for most common challanges when dealing with multithreading. 

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

Semaphore are used for synchronization (i.e "Hello there, function foo()! Did you know that something occurred just now?"). A semaphore is an integer variable that can only be accessed via two atomic operations: wait and signal. Changes to the sempahore value in the wait and signal actions must be carried out independently. That is, when one process changes a semaphore value, no other process can change the same semaphore value at that time.

#### Common miconceptions

Because a binary semaphore can only accept one of two values, it serves the same function as a mutex. The prevalent misunderstanding is that they are the same thing.

Mutex is used to gain exclusive access to a resource. A binary semaphore should be used as a signaling technique. The binary "producer" merely informs all of the "consumers" that what they were expecting has occurred. 

### Examples

#### Examples in C++

When we use <code>join()</code>:

* The program will wait for the thread to complete before continuing its execution. 
* Any resources belonging to the thread will be cleaned up.

When we use <code>detach()</code>:

* The program does not wait for the threads to complete their execution before proceeding to the next instruction.
* When the program terminates, all remaining detached threads are suspended and their objects are destroyed. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name

#### Examples in Python

A global interpreter lock (GIL) is a technique used in programming language interpreters to synchronize thread execution such that only one native thread may run at any given moment. Even when running on a multi-core CPU, GIL permits only one native thread to execute at a time.

The natural question is if it even makes sense to use threads in Python? The answer is, it depends. We can still make greater use of the CPU that is idle while waiting for I/O by using multihreading. We can increase performance by overlapping the waiting time for requests. We should however look at multiprocessing if we want to split CPU-intensive activities over multiple CPU cores. 

The main module that we are going to use is called <code>threading</code>.

#### Examples in JavaScript

## Multiprocessing

Multiprocessing is a method of achieving concurrency by creating several processes within a single program. It is important to note that parallelism is not guaranteed by multiprocessing. Processes, unlike threads, do not share any resources by default, hence it makes the most sense to use that approach when you wish to simulatenously complete unrelated tasks.

### Child processes 

The parent processe may:

- wait for it 
- it may need to check on its status and see if it's still running
- it may want to communicate with it and get some data back from it
- if it's just taking too long I might just need to kill it

#### Zombie Process

A zombie process is one that has completed its execution but still has an entry in the process table to report to its parent process. Before being deleted from the process table, a child process always becomes a zombie. The parent process reads the child process's exit status and removes the child process entry from the process table. 

The zombie isn't occupying any significant memory or resources, it's (effectively) only an exit status waiting to be delivered. 

#### Orphan Process

An orphan process is one that no longer exists because its parent process has ended or been terminated without waiting for its child process to finish.

An orphan is a live, running process just like any other -- it just has a peculiar name.

### Communication between processes

To fully take advantage of multiprocessing we need to have a way to communicate between the processes. 

#### Message Passing

Message passing exchanges data via techniques known as inter-process communication (IPC). On the plus side, it is applicable to both local and remote communication (multiprocessing programs can be distributed between remote machines). One disadvantage is a possible "loop-back" configuration, this may result in excessive overhead for big messages. 
   
#### Shared Memory

Shared memory enables programs to access and share data as though they were local to each process's address space. On the plus side, in a loop-back design, it may be more efficient for massive data. One disadvantage is that it does not generalize well to remote systems and may be more error-prone. 

### Challanges with multiprocessing

The independence of processes from one another is their strength. It's also its weakest point, because processes don't communicate easily with one another.

#### Debugging
If one of the processes in hanging how do you know which one? How can you debug it? When you start debbuger you have to tell it which process it should follow. More processes the harder and more time consuming the whole process is.

#### Deadlocks

A deadlock occurs when two or more processes wait for each other to complete and none of them ever do. Consider the following scenario: Mask in the shop.

What causes a deadlock to occur?

* Mutual Exclusion: A resource is not available for sharing.
* Hold and Wait: A process holds at least one resource while waiting for another resource that is held by another waiting process.
* No Preemption: The operating system is not permitted to reclaim a resource from a process until the process returns it.
* Circular Wait: A group of processes that are waiting for each other in a circular fashion.

### Containers

You can have microservice architecture.
Each app is one process. But many apps are spread across the containers.

### Examples

#### Examples in C++

How to create new processes?

* In Windows: <code>CreateProcess()</code>
* In Linux: <code>fork()</code>

In every UNIX-based operating system, all processes are created from a single process. The first process is initiated when your system boots up. 

All of the processes that are currently running on your machine are clones of that original process.
When you call <code>fork()</code>', you are just requesting that a clone of your process be created. Everything is copied from the parent process, except for the PID. 

How do we check if the program in use is the child or the parent?
You may examine the <code>fork()</code>'s return value. It returns different results to the parent and the child. The child receives zero, whereas the parent receives the child's PID. 

When creating a new process, you usually don't want it to do the same thing as the parent.
You spawn the child process to perform a specific task while the parent is busy doing something else.

How do you make a child process run a different program? 

* <code>exec*()</code> family of functions is used to replace an existing process with a new program. 

<code>exec()</code> does not create a new process and run the program in it; instead, it runs the program you provide by replacing it with the existing process. The current process has been terminated. The new one has the same PID as the previous one, but everything else is changed. 

<code>exec()</code> will only return a value if the new program could not be launched for whatever reason. Otherwise, the caller program will simply stop existing, and the new program will run in its place. 

#### Examples in Python

IT IS EASY TO USE INTERPROCESS COMUNICATION WITH MULTIPROCESSING

Subprocess spawns new processes, but aside from stdin/stdout and whatever other APIs the other program may implement you have no means to communicate with them. Its main purpose is to launch processes that are completely separate from your own program.

Multiprocessing also spawns new processes, but they run your code, and are designed to communicate with each other. You use it to divide tasks within your own program across multiple CPU cores.


Using Python multiprocessing, we are able to run a Python using multiple processes. In principle, a multi-process Python program could fully utilize all the CPU cores and native threads available, by creating multiple Python interpreters on many native threads. Because all the processes are independent to each other, and they don’t share memory. To do collaborative tasks in Python using multiprocessing, it requires to use the API provided the operating system. Therefore, there will be slightly large overhead.

multiprocessing is a package that supports spawning processes using an API similar to the threading module. The multiprocessing package offers both local and remote concurrency, effectively side-stepping the Global Interpreter Lock by using subprocesses instead of threads. Due to this, the multiprocessing module allows the programmer to fully leverage multiple processors on a given machine. It runs on both Unix and Windows.

The multiprocessing module can use multiple processes, but still has to work with the Python Global Interpreter Lock, wich means you can't share memory between your processes. So when you try to launch a Pool, you need to copy useful variables, process your calculation, and retrieve the result. This cost you a little time for every process, and makes you less effective.

* https://opensourceoptions.com/blog/asynchronous-parallel-programming-in-python-with-multiprocessing/

The multiprocessing module contains primitives to help share values across multiple processes.

#### Examples in JavaScript

## Asynchrony

 FUTURE = PROMISE

Asynchrony is unrelated to the previous two concepts (multithreading and multiprocessing). When one task is completed in an asynchronous approach, you may go on to the next without waiting for the preceding task to finish. This is known as nonblocking behavior. When you execute anything synchronously, you wait for it to finish before proceeding to the next task. Concurrency is accomplished with the help of asynchronous programming. Parallelism may be achieved by asynchronous programming in a multi-threaded environment. 

"Synchronously" means "using the same clock," therefore synchronous instructions share the same clock and must execute one after the other. The term "asynchronous" translates to "not utilizing the same clock," which indicates that the instructions are not concerned with being in synchronization with one another. The phrase does not refer to the connection of the instructions to one another. It is referring to the connection of each instruction to the clock. 

For example, retrieving data from a database may take some time, but we don't want our UI to freeze while we wait. As soon as the asynchronous function is invoked, the asynchronous call accepts a call-back reference and returns execution to the client function. While the asynchronous function performs any necessary processing, your UI may continue to respond to the user; after the asynchronous function has completed its task, the client function may use the resultant data (e.g. to display it in the ui). 

As opposed to..
Threading typically works with existing code and tools as long as locks are added around critical sections.
For complex systems, async is much easier to get right than threads with locks.
Threads require very little tooling (locks and queues).
Async needs a great deal of tooling (futures, event loops, and non-blocking versions of just about everything).

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

### Why would you want to use asynchrony over multithreading?

### Challanges

Async switches cooperatively, so you do need to add explicit code “yield” or “await” to cause a task switch.

Now you control when task switches occur, so locks and other synchronization are no longer needed.

Also, the cost task switches is very low. Calling a pure Python function has more overhead than restarting a generator or awaitable.

This means that async is very cheap.

In return, you’ll need a non-blocking version of just about everything you do. Accordingly, the async world has a huge ecosystem of support tools. This increases the learning curve.


FOR ASYnchrony you need asynchronous functions (called coruitnes) marked with async keyword. How are you going to interact with the massive world of functions written in a blocking way?

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
    
The thread starts running immediately. We can either detach it, or have join it at the end of the scope, or whenever. The details of using std::thread don't concern us here, though; just be sure to join or detach thr eventually.

#### Examples in Python

Given threading is using multi-thread to maximize the performance of a I/O-bound task in Python, we wonder if using multi-thread is necessary. The answer is no, if you know when to switch the tasks. For example, for each thread in a Python program using threading, it will really stay idle between the request is sent and the result is returned. If somehow a thread could know the time I/O request has been sent, it could switch to do another task, without staying idle, and one thread should be sufficient to manage all these tasks. Without the thread management overhead, the execution should be faster for a I/O-bound task. Obviously, threading could not do it, but we have asyncio.

Using Python asyncio, we are also able to make better use of the CPU sitting idle when waiting for the I/O. What’s different to threading is that, asyncio is single-process and single-thread. There is an event loop in asyncio which routinely measure the progress of the tasks. If the event loop has measured any progress, it would schedule another task for execution, therefore, minimizing the time spent on waiting I/O. This is also called cooperative multitasking. The tasks must cooperate by announcing when they are ready to be switched out. 

* https://masnun.com/2015/11/20/python-asyncio-future-task-and-the-event-loop.html

The main module we'll be using is named <code>asyncio</code>. It is distinguished for its outstanding speed and simplicity of usage. It makes it easy to write single-threaded concurrent programs using corutines (stripped down version of threads). It conceals the complexities of concurrent programming from us by providing a plethora of different functions that implement numerous synchronization strategies for us. 

Corutines

Coroutines are functions that can be stopped and resumed while being run. In Python, they are defined using the async def keyword. Much like generators, they too use their own form of yield from which is await. 

CAN BE STOPPED AND RESUMED

Futures

Futures are objects that have the __await__() method implemented, and their job is to hold a certain state and result. The state can be one of the following:

    PENDING - future does not have any result or exception set.
    CANCELLED - future was cancelled using fut.cancel()
    FINISHED - future was finished, either by a result set using fut.set_result() or by an exception set using fut.set_exception()

The result, just like you have guessed, can either be a Python object, that will be returned, or an exception which may be raised.


#### Examples in JavaScript

## Rules of thumb

Previously i tried to be objective. Here I want to share some of my subjective takes on the subject

Avoid Threads, Async and Promises
Threads and Async are fascinating. BUT: It's hard to debug. You will need much longer than you initially estimated. Avoid it, if you want to get things done. It's different in your spare time: Do what you want and what is fascinating for you. There is one tool and one concept that is rock solid, well known, easy
to debug, and available everywhere and it is great for parallel execution. The tool is called "operating system" and the concept is
called "process". Why re-invent it? Do you think starting a new process is "expensive" ("it is too slow")? Just, do not start a new process for
every small method you want to call in parallel. Use a [Task Queue](https://www.fullstackpython.com/task-queues.html). Let this tool
handle the complicated async stuff and keep your code simple like running in one process with one thread.

## Hardware

 Inside the processor (instruction-level parallelism, multicore)
• Through multiple processors in one machine (multiprocessing)
• Through multiple machines (multicomputer)

### A single-core CPU

* Every step of the program is converted to instruction

          print(3) ---> 0110 1010 (Hex: 6A)
 
 * Codes are specific for the given architecture
 * All codes form an instruction set


          CPU <===> RAM
  
  * From hardrive to system memory (RAM)
  * From system memory over a bus to CPU
  * Once a program is loaded into CPU it's going down the queue (pipeline) of instructions. Executing one at at ime.
  * CPU uses different components (ALU and so on) inside it to execute the instructions
  * At the heart of the CPU there is a clock
  
### Today's CPU

* designed to handle multithreading easily
* hyper threading was one of the early ideas
* hyper threading was duplicating the pipelines for the threads (quicker but not everything in parallel(
* Multi-Core: everything is duplicated (pipeline and execution engine)
* Multi CPU - the best solution

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


### Parallel hardware

* Multi-core processors
* Graphics Processing Unit (GPU)

## Bibliography

* https://www.personal.kent.edu/~rmuhamma/OpSystems/os.html
* https://www.oreilly.com/library/view/the-art-of/9780596802424/
* https://pybay.com/site_media/slides/raymond2017-keynote/index.html
* https://courses.cs.cornell.edu/cs100/1999su/lectures/lecture17/sld003.htm
* https://inst.eecs.berkeley.edu/~cs152/fa16/lectures/
* https://princetonuniversity.github.io/PUbootcamp/sessions/parallel-programming/
* https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/
* https://lucumr.pocoo.org/2016/10/30/i-dont-understand-asyncio/
