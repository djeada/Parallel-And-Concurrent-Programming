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
