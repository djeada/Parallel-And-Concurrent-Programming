## Basic terminology

Let's start by defining some key terms and emphasizing the distinctions between related concepts. In general those concepts are universal and may be applied to any programming language. The differences between the languages will be discussed in greater detail later, when we attempt to explain the specific approach in implementing concurrency.

### Sequential vs Nonsequential

- Sequential:
  - Tasks are carried out in the order they occur in the code.
  - Depend on being executed step-by-step to produce correct results.

- Non-sequential:
  - Tasks do not have to be executed in the order in which they appear in the code. 
  - Step-by-step execution is not necessary for correct results.

Analogies:

* Sequential: Think of a recipe where each step must be completed before moving on to the next step. You cannot mix the ingredients before chopping them, for example. The recipe must be followed sequentially for the desired outcome.
* Non-sequential: Imagine you are packing your suitcase for a trip. You can pack your clothes in any order you want and still get to your destination with everything you need. The order in which you pack your clothes is not important, and you can achieve the desired outcome (having everything you need) even if you don't pack in a specific sequence.

### Concurrency vs Parallelism

- Concurency:
  - Refers to the program structure. 
  - Means DEALING with multiple things at the same time.
  - Requires scheduling and synchronization.

- Parallelism:
  - Stands for simultaneous execution.
  - Means PERFORMING numerous tasks at the same time.
  - Increases throughput.
  - Requires parallel hardware and concurrency support.
  
Analogies:

* Concurrency: Think of a restaurant where multiple chefs are preparing different dishes at the same time. Each chef is responsible for a specific task, and they all work together to ensure that all dishes are ready at the same time. The chefs are working concurrently, but they may not be performing their tasks in parallel.
* Parallelism: Imagine a group of friends cleaning a house together. If everyone works on their own task (cleaning the bathroom, vacuuming the living room, etc.) at the same time, they are working in parallel. This can result in a faster completion time than if they worked on each task one after the other.

### Synchronous vs Asynchronous

- Synchronous:
  - When you start one task, you must wait until it is completed before starting the next. 
  - Blocking.

- Asynchronous:
  - Other tasks can be started before the present one is completed. 
  - Non-blocking.
 
Analogies:

* Synchronous: Imagine you are at a restaurant and you place an order for a burger. The kitchen must finish making your burger before it can start making the next order. This is synchronous, and you must wait for your order to be completed before the next order can be processed.
* Asynchronous: Imagine you are sending a text message to a friend while also listening to music. You can send the text message while the music continues to play in the background. You don't have to wait for the message to be sent before continuing to listen to the music. This is asynchronous, and you can perform multiple tasks at the same time without waiting for each task to be completed before starting the next.

## Process

A process is an instance of a program that is currently executing on a computer. When you use any application on your computer, such as a web browser, text editor, or video player, it runs as a process. In some cases, a single application can have multiple processes running simultaneously. Additionally, the operating system uses processes to perform tasks in the background while you work.

Modern operating systems can manage multiple processes at the same time. Although the CPU cannot execute instructions from multiple processes at the same time, it switches between processes very quickly. If the hardware supports it, two or more processes can run in parallel at some point.

### Role of the OS

The operating system is responsible for managing all processes running on the computer. 

Each process is allocated a portion of computer resources including:

- time to use the processor,
- share of computer memory,
- access to the hard drive.

The operating system maintains a table known as the process table, which keeps track of all processes' status.

### States of the process
There are three states of a process:

1. Running: The process is currently using the CPU, has all the resources it needs, and has been granted permission by the operating system to execute. At any given moment, only one process can be in this state.
2. Ready: The process is waiting for permission from the operating system to use the CPU. Multiple processes can be in the ready state simultaneously.
3. Waiting: The process is waiting for an external event to occur, such as user input or disk access. In this state, the process is not using the CPU, and other processes can be executing in the meantime.

## Thread

In computer science, a thread refers to a sequence of instructions that can be executed concurrently with other threads within a program. A process can be comprised of one or more threads, with each thread being provided with its own program counter (PC), register set, and stack space. Threads share resources such as code section, data section, and OS resources such as open files and signals with other threads within the same process. Threads are sometimes called lightweight processes because they have some similarities to processes.

Threads can be used to improve the performance and concurrency of programs. For example, a text editor can employ several threads, one to format the text, another to receive inputs, and so on. The use of multiple threads within a program can allow multiple operations to be performed simultaneously, which can lead to faster execution times and improved performance.

### Role of the OS

The operating system plays a crucial role in managing threads. The scheduler controls CPU access and switches between threads. Each thread is given a single time slice for its work, and if it isn't finished, it must wait for its next turn. Threads also have various priorities (represented by numbers) assigned to them. The higher the priority, the more often the thread will be run.

### Process vs Thread

A program's memory allocation is often divided into four parts:

* Code
* Data
* Stack
* Heap

When a process is cloned, only the code portion is shared; everything else is separate. On the other hand, threads that operate within the same process share all of these segments except the stack. Each thread has its own call stack, but the memory on other thread stacks is still accessible, and you could theoretically keep a reference to memory in another thread's local stack frame.

Here are some key differences between a process and a thread:

Process:

* Independent instance;
* Includes code and data;
* Carries considerable state (e.g., ready, running, waiting, or stopped) information;
* Separate address space;
* No process can directly access the memory of another process;
* Processes communicate with each other through inter-process communication.

Thread:

* Independent path of execution;
* Subset of a process;
* Operating system schedules threads for execution;
* All threads within a single process share the same address space and other resources.

## Understanding CPU-Bound vs I/O-Bound

To optimize a software program's performance, it's important to identify the primary source of slowdown or the bottleneck. This could be either due to I/O (input/output) activities or an underutilized CPU. Different strategies can be applied based on the cause of the bottleneck.

### CPU-Bound

When a task's completion time mainly depends on the speed of the CPU, it is considered CPU-bound. In simpler terms, the CPU's processing power determines how quickly the task finishes.

- To speed up a CPU-bound software, you can use parallelism, which involves dividing a task into smaller subtasks and processing them simultaneously.
- On the hardware side, having a CPU with a higher clock rate can improve your software's performance.

Here's an example that illustrates a CPU-bound scenario:

```bash
I/O waiting
CPU Processing  ----Task 1---->----Task 2---->
```

### I/O-Bound

A task is I/O-bound when its completion time mostly relies on the time spent waiting for input/output operations to finish. In simpler terms, the efficiency of I/O components determines how fast the task is completed.

- In software, you can improve the performance of I/O-bound tasks by using concurrency without necessarily using parallelism. Concurrency allows multiple tasks to be executed in overlapping time periods.
- For hardware improvements, focus on faster I/O components, such as quicker memory I/O, hard disk I/O, or network I/O.

Here's an example that illustrates an I/O-bound scenario:

```bash
I/O waiting         -----request----->     ------request------>     ------request------>
CPU Processing  --->                  ---->                    ---->
```
