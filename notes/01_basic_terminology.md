## Basic terminology

Let's start with the terms used throughout these notes. Some sound similar but describe different things: how tasks are ordered, whether they run at the same time, and what happens while they wait. These ideas apply across programming languages; later notes explain how individual languages implement them.

### Sequential vs Non-sequential Execution

Tasks can run one after another, or their execution can overlap. Which approach is possible depends partly on whether one task needs another task's result.

#### Sequential Execution

In sequential execution, each task finishes before the next begins. For example, a program might read a file, process its contents, and then save the result. Each step has the previous step's output available when it starts.

Characteristics of Sequential Execution:

- Tasks follow a specified sequence, one at a time.
- A step may depend on an earlier result, though independent tasks can also run sequentially.
- The single flow of execution is usually easy to follow and debug.
- Waiting in one task delays all later tasks in that sequence.
- Think of following a recipe step by step: chop the ingredients, mix them, and then cook them.

#### Non-sequential Execution

Non-sequential execution allows some tasks to overlap or run in a different order. Independent tasks offer the most freedom, but any dependencies still have to be respected. A program can process two files independently, for example, but must wait for both results before combining them.

Characteristics of Non-sequential Execution:

- Independent tasks can run in different orders or at the same time.
- Tasks that depend on earlier results still need to wait for them.
- Managing these dependencies makes the program harder to reason about.
- Overlapping work can improve performance, provided the coordination costs are small enough.
- Think of gathering items for a trip: you can collect clothes and toiletries in either order, but both must be ready before you finish packing.

### Concurrency vs Parallelism

Concurrency and parallelism both involve multiple tasks. The distinction is whether those tasks make progress over overlapping periods or actually execute at the same instant.

#### Concurrency
Concurrency means handling multiple tasks whose execution overlaps in time. On a single CPU core, a program can switch between tasks, making progress on each without executing them simultaneously.

Characteristics of Concurrency:

- The program keeps track of several tasks that are in progress.
- Tasks can take turns using the CPU, including while another task waits for input or output.
- Scheduling determines which task runs; synchronization coordinates access to shared data or dependent work.
- Concurrency can improve responsiveness, but does not guarantee faster execution.
- Think of one chef preparing several dishes: while a soup simmers, the chef chops vegetables for a salad. Both dishes are in progress, even though the chef performs one action at a time.

#### Parallelism
Parallelism means executing multiple tasks at the same instant, for example on different CPU cores. A large task can be split into smaller pieces that run in parallel, provided their dependencies allow it.

Characteristics of Parallelism:

- Running work simultaneously can reduce the time needed to finish it.
- It can also increase throughput: the amount of work completed in a given period.
- It requires hardware that can execute work in parallel, such as multiple CPU cores.
- Dividing work and coordinating results adds overhead, so the speedup is not automatic.
- Think of several friends cleaning different rooms at the same time. They finish sooner if there is enough work for everyone and they do not keep getting in each other's way.

### Synchronous vs Asynchronous Execution

Synchronous and asynchronous execution describe how a caller waits for an operation to finish. This is a different question from whether several tasks execute in parallel.

#### Synchronous Execution
With a synchronous call, the caller waits for the operation to finish before continuing. This makes the flow easy to follow, but a slow operation can hold up that caller. Other threads in the program may still run during the wait.

Characteristics of Synchronous Execution:

- The caller continues only after the call completes or reports an error.
- Results are available in the order the caller requests them.
- This direct control flow is usually straightforward to read and debug.
- Think of ordering at a counter and waiting there until your food is ready before doing anything else.

#### Asynchronous Execution
An asynchronous operation lets the caller start work and handle its completion later. While the operation is pending, other work can proceed. This is useful for disk or network operations, where much of the time may be spent waiting. It does not, by itself, mean that code runs on another CPU core.

Characteristics of Asynchronous Execution:

- The caller can receive a handle to a pending result instead of waiting for the result immediately.
- Useful work can overlap with waiting, improving responsiveness or throughput.
- Completion and errors must be handled later, using mechanisms such as callbacks, futures, or async/await.
- Think of ordering food, receiving a collection number, and finding a table while the kitchen prepares your order.

### Comparison of Execution Paradigms

These descriptions can apply together. A program can use synchronous calls in several parallel threads, or handle asynchronous operations concurrently on a single thread.

| Characteristic              | Sequential                                                                                                                                         | Non-Sequential                                                                                                                      | Concurrent                                                                                                                        | Parallel                                                                                                                           | Asynchronous                                                                                                                       | Synchronous                                                                                                                         |
|---------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------|
| Definition                  | Tasks are executed one after another in a specific order.                                                                                               | Tasks may overlap or change order where dependencies allow.                                                                                       | Multiple tasks make progress within overlapping time periods.                                                                         | Multiple tasks are executed simultaneously on multiple processors.                                                                     | Tasks are initiated and proceed independently of other tasks, often with callbacks or promises to handle completion.                    | The caller waits for an operation to complete before continuing.                                           |
| Order of Execution          | Strict and predictable order.                                                                                                                           | Flexible order.                                                                                                                         | Overlapping time periods, but not necessarily simultaneous.                                                                           | Simultaneously on different processors.                                                                                                | Initiated independently, can complete at any time.                                                                                      | Calls complete before the caller continues; other threads may still run.                                                |
| Task Scheduling             | Simple and straightforward.                                                                                                                             | Flexible, can change dynamically.                                                                                                       | Managed by the scheduler to ensure progress of all tasks.                                                                             | Managed by the system, often requires multiple processors.                                                                             | Can be scheduled to run at any time, often managed by event-driven programming models.                                                  | The calling thread waits; other threads may be scheduled.                                                                               |
| Complexity                  | Low complexity, easy to design and debug.                                                                                                               | More complex due to flexibility.                                                                                                        | Moderate complexity, requires careful management of resources.                                                                        | High complexity, requires handling of synchronization and resource sharing.                                                            | Moderate to high complexity, requires handling of callbacks, promises, or other mechanisms to manage independent tasks.                 | Low complexity, but can lead to inefficiencies if tasks have to wait for long periods.                                                   |
| Performance                 | Can be slower due to waiting for each task to complete before starting the next.                                                                         | Can be efficient if tasks are independent and do not need a specific order.                                                            | Can improve performance by overlapping tasks, but may involve context switching overhead.                                              | Can significantly improve performance for CPU-bound tasks by using multiple processors.                                                 | Can improve responsiveness and resource utilization, especially in I/O-bound tasks.                                                     | Can lead to inefficiencies if tasks involve a lot of waiting, as each task has to wait for the previous one to complete.                  |
| Use Cases                   | Simple scripts, batch processing where order is crucial.                                                                                                | Event-driven systems, where tasks can be handled as they come.                                                                         | Multithreading applications, GUI applications where multiple tasks need to be handled concurrently.                                   | High-performance computing, data processing tasks that can be divided into smaller, independent tasks.                                  | Web servers handling multiple requests, applications with I/O operations where waiting for responses would be inefficient.              | Systems where order of operations is crucial and tasks are dependent on the completion of previous tasks, such as transaction systems. |

## Process

A process is a running instance of a program, with its own memory and resources managed by the operating system. A text editor might run in one process, while a browser may use several to handle tabs and background work. The operating system also runs processes for services such as scheduled jobs and network communication.

### Characteristics of a Process

- Each process is assigned a unique process identifier (PID), which helps the OS manage and differentiate between various processes.
- Each thread's program counter records where it is in the instruction sequence.
- Process state represents the current status of the process, such as running, waiting, etc.
- Memory allocation for each process includes several segments: the code segment contains the executable code, the data segment stores global and static variables, the stack segment manages function calls and local variables, and the heap segment is used for dynamic memory allocation during the process runtime.

### Role of the Operating System (OS)

The operating system manages the resources processes need: CPU time, memory, and access to devices. It also keeps track of processes as they start, wait for work, and finish.

Responsibilities of the OS in Process Management:

- The OS performs process scheduling by using algorithms to determine the order in which processes run, ensuring fair distribution of CPU time among processes.
- Resource allocation is managed by the OS, allocating CPU time, memory, and I/O devices to processes as needed, preventing conflicts and ensuring efficient utilization.
- Through process isolation, each process operates in its isolated memory space, preventing interference from other processes and enhancing security and stability.
- Inter-process communication (IPC) mechanisms provided by the OS allow processes to communicate and synchronize with each other, using methods like message passing, shared memory, and semaphores.
- The OS manages process creation and termination, handling the creation of new processes and ensuring resources are properly released and reclaimed after a process terminates.

### Process Table

The OS maintains records of active processes, often described collectively as the process table. These records include each process's PID, state, scheduling information, and memory allocation. Together with saved thread state, they let the OS pause execution and resume it later.

### States of a Process

A process can be in one of several states during its lifecycle. The three primary states are:

- A process is running when one of its threads is executing on a CPU. A process with several threads may use more than one core at once.
- When a process is in the ready state, it is waiting for permission from the OS to use the CPU. It is ready to run but must wait for the CPU to become available. Multiple processes can be in the ready state simultaneously, queued up and waiting their turn.
- The waiting (or blocked) state occurs when a process is waiting for an external event to occur, such as user input, disk access, or a network response. In this state, the process is not using the CPU, allowing other processes to be executed in the meantime.

## Thread

A thread is a path of execution within a process. A process has one or more threads, each with its own program counter, registers, and call stack. Threads in the same process share its address space and resources such as open files. They are sometimes called lightweight processes because creating a thread usually requires fewer resources than creating a separate process.

For example, a text editor might use one thread to handle keyboard input and another to check spelling. This can keep typing responsive while the spelling check runs. Threads may also run in parallel, but whether they improve performance depends on the work, the runtime, and the available hardware.

### Role of the Operating System (OS)

For threads managed by the OS, the scheduler decides which ones get CPU time. Language runtimes can also manage their own tasks on top of these threads.

#### Thread Management and Scheduling

- The OS uses a component called the scheduler to control CPU access. The scheduler determines which threads are to be executed by the CPU at any given time. It switches between threads to ensure that all active threads get a chance to run, a process known as context switching.
- In a scheduler that uses time slices, a thread runs for a limited period before another ready thread gets a turn. A thread can also give up the CPU earlier, for example when it waits for I/O.
- Thread priorities influence scheduling decisions. The exact effect depends on the OS and its scheduling policy; priority does not guarantee a particular execution order.
- The OS supports multithreading and concurrency, allowing multiple threads to be executed concurrently. This improves the overall efficiency of the system by utilizing idle CPU cycles for other tasks.

#### Memory Allocation in a Program

A program's memory allocation is often divided into four segments:

- The code segment contains the compiled code of the program.
- Data segment stores global and static variables.
- Stack segment is used for function call management, including local variables and control flow.
- Heap segment is utilized for dynamic memory allocation during runtime.

## Processes vs Threads

The main distinction is how resources are owned and shared. Processes normally have separate address spaces, making them useful when tasks need isolation.

Characteristics of Processes:

- Processes operate with independence, each having its own memory space.
- State information for each process includes its current activity status, such as ready, running, waiting, or stopped.
- Address space for processes is separate, preventing direct access to each other's memory.
- Inter-process communication (IPC) mechanisms like pipes, sockets, or shared memory are used by processes to communicate, as they cannot access each other's memory directly.

Threads belong to a process and share its address space. This makes exchanging data convenient, but also means that one thread can change data another thread is using. Access to shared mutable data needs coordination.

Characteristics of Threads:

- A thread represents an execution path that is independent within a process.
- Threads are a subset of a process, sharing most of the process's resources.
- Scheduling of threads is managed by the OS, with threads within the same process sharing the same address space.
- Shared resources among threads within a process include the same memory segments (code, data, and heap), except for the stack. Each thread has its own call stack to manage its function calls and local variables.

Differences Between Processes and Threads:

| Aspect                  | Process                                 | Thread                                  |
|-------------------------|-----------------------------------------|-----------------------------------------|
| Independence        | Processes are independent instances that run in separate address spaces. | Threads are subsets of a process and run within the same address space as the process. |
| Memory              | Processes have separate address spaces, meaning each process has its own memory area. | Threads share the address space of their parent process, allowing them to access the same memory and data. |
| Communication       | Processes require Inter-Process Communication (IPC) mechanisms like pipes, message queues, or shared memory to communicate with each other. | Threads can communicate directly by accessing shared memory within the process. |
| State Information   | Processes carry considerable state information, including process ID, process state, memory information, and more. | Threads maintain minimal state information, typically just a thread ID, program counter, register set, and stack. |
| Resource Sharing    | Processes own separate resources but can explicitly share some, such as shared memory. | Threads share resources such as code, data, and open files with other threads within the same process. |
| Creation Overhead   | Process creation has higher overhead because it involves allocating a separate memory space and other resources. | Thread creation has lower overhead since threads share resources and memory with the parent process. |
| Context Switching   | Context switching between processes involves more overhead due to switching separate memory spaces and state information. | Context switching between threads is faster and involves less overhead because they share the same memory space and resources. |

Here's a table comparing common life cycle stages of threads and processes. The exact names vary by OS or runtime; suspended states appear in some models that include moving processes out of main memory.

| Life Cycle Stage       | Thread Life Cycle                        | Process Life Cycle                     |
|------------------------|------------------------------------------|----------------------------------------|
| New                | Thread is in the process of being created. | Process is in the process of being created. |
| Ready              | Thread is ready to run when CPU is available. | Process is ready to execute when CPU is available. |
| Running            | Thread is actively executing instructions. | Process is actively executing instructions. |
| Waiting/Blocked    | Thread is waiting for resources or I/O.  | Process is waiting for resources or I/O. |
| Timed Waiting      | Thread is waiting for a specified time.  | (Not typically a separate state, but can be considered under waiting) |
| Terminated         | Thread has completed execution or is aborted. | Process has completed execution or is terminated. |
| Ready Suspended    | (Not typically a separate state, but can be inferred from 'Ready' and 'Blocked') | Process is in secondary storage and ready to execute when moved to main memory. |
| Blocked Suspended  | (Not typically a separate state, but can be inferred from 'Blocked') | Process is in secondary storage and waiting for an event before moving to main memory. |

Practical Implications:

- Performance benefits arise because threads are generally faster to create and manage compared to processes. This efficiency is due to threads sharing the same memory space and resources, which reduces the overhead of context switching.
- Separate address spaces help contain faults: a process normally cannot overwrite another process's private memory. Shared resources still need care.
- Multithreading lets tasks overlap within a process. CPU-bound work benefits when those threads can run in parallel and the benefit outweighs coordination costs.

## CPU-Bound vs I/O-Bound

Before trying to speed up a program, find out where it spends its time. Is it mostly computing, or waiting for input/output (I/O), such as disk reads and network responses? The answer helps determine whether parallel computation or overlapping waits is likely to help. A program can also move between these bottlenecks during different stages of its work.

### CPU-Bound

A task is CPU-bound when computation is the main limit on its speed. For example, calculating a large set of prime numbers may keep a CPU core busy with very little I/O. A faster algorithm or parallel execution across cores may reduce the running time.

Characteristics of CPU-Bound Tasks:

- The CPU cores doing the work stay busy. A task using only one core can be CPU-bound even if overall CPU usage looks low.
- Relatively little time is spent waiting for disk reads, network responses, or other I/O.

Strategies for Optimizing CPU-Bound Tasks:

- Divide independent work across CPU cores using threads or processes, where the language runtime supports parallel execution.
- Look for a more efficient algorithm. Reducing the amount of computation can help more than adding workers.
- A faster CPU may help. Extra cores help only if the program can use them.

Example of a CPU-Bound Scenario:

```text
I/O waiting
CPU Processing  ----Task 1---->----Task 2---->
```

In this example, the CPU is continuously processing tasks with minimal waiting for I/O operations. The speed at which tasks are completed is primarily limited by the CPU's processing power.

### I/O-Bound

A task is I/O-bound when input or output is the main limit on its speed. Downloading files, querying a remote database, and reading from disk can involve substantial waiting. Overlapping independent requests can help, although the disk, network, or remote service still limits how much work can be completed.

Characteristics of I/O-Bound Tasks:

- Much of the task's time is spent waiting for I/O to complete.
- CPU usage for the task may be low during these waits, though other tasks can still keep the CPU busy.

Strategies for Optimizing I/O-Bound Tasks:

- Overlap independent I/O operations using asynchronous programming or threads, so one wait does not hold up all the work.
- Reduce I/O where possible: cache frequently used data, choose efficient data formats, or improve database queries.
- Faster storage or a faster network connection may help if that component is the bottleneck.

Example of an I/O-Bound Scenario:

```text
I/O waiting         -----request----->     ------request------>     ------request------>
CPU Processing  --->                  ---->                    ---->
```

In this example, the CPU is often waiting for I/O operations to complete before it can continue processing. The speed at which tasks are completed is primarily limited by the efficiency of the I/O operations.
