## Basic terminology

Let's start by defining some helpful terms and emphasizing the distinctions between related concepts. In general those concepts are universal and may be applied to any programming language. The differences between the languages will be discussed in greater detail later, when we attempt to explain the specific approach in implementing concurrency.

### Sequential vs Non-sequential Execution

The way tasks are executed in a program can be classified as either sequential or non-sequential. Understanding these concepts is crucial for effective programming and optimizing performance.

#### Sequential Execution
In sequential execution, tasks are carried out in the exact order they appear in the code. Each task must be completed before the next one begins. This approach is straightforward and ensures that operations dependent on previous results are handled correctly.

Characteristics of Sequential Execution:

- The **order of execution** in sequential execution ensures tasks are executed one after another in a specified sequence.
- **Dependence** among tasks means each step often relies on the outcome of the previous step.
- **Simplicity** of sequential execution makes it easier to implement and understand, making it suitable for simple tasks and algorithms.
- **Performance** can be slower for complex tasks, as each task must wait for the previous one to complete.
- An **analogy** for sequential execution is following a recipe step-by-step, where each action depends on completing the previous one. For instance, you must chop ingredients before mixing them.

#### Non-sequential Execution
Non-sequential execution allows tasks to be executed in any order, as the correct results do not depend on the order of execution. This approach is often used in situations where tasks are independent of each other.

Characteristics of Non-sequential Execution:

- **Flexibility** in non-sequential execution allows tasks to be executed in any order or even simultaneously if possible.
- The **independence** of tasks means each operates without depending on the completion of another.
- **Complexity** in implementation arises due to the need for managing the order of execution.
- **Performance** can be enhanced by processing tasks concurrently in non-sequential execution.
- An **analogy** for non-sequential execution is packing a suitcase, where items can be packed in any order and still achieve the desired outcome. The sequence in which items are packed does not matter as long as all items are included.

### Concurrency vs Parallelism

Concurrency and parallelism are concepts related to the execution of multiple tasks but differ in their implementation and purpose.

#### Concurrency
Concurrency refers to the design of a program to handle multiple tasks at the same time. It involves structuring the program to manage several tasks that make progress independently, though not necessarily simultaneously.

Characteristics of Concurrency:

- The **program structure** in concurrency focuses on organizing the program to handle multiple operations simultaneously.
- **Task management** in concurrency involves managing and executing multiple tasks in overlapping time periods.
- **Scheduling and synchronization** are essential as concurrency requires mechanisms to schedule tasks and synchronize their execution.
- **Efficiency** is improved in concurrency, enhancing program responsiveness and utilization of resources.
- An **analogy** for concurrency is restaurant chefs, where multiple chefs prepare different dishes concurrently. Each chef handles a specific task, working together to ensure all dishes are ready simultaneously, even though they may not be cooking at the exact same moment.

#### Parallelism
Parallelism involves performing multiple tasks simultaneously. This is achieved by dividing a task into smaller sub-tasks that can be executed at the same time on multiple processors or cores.

Characteristics of Parallelism:

- **Simultaneous execution** of tasks leads to faster completion as multiple tasks are executed at the same time.
- The **increased throughput** characteristic allows handling more operations in a given period, thereby enhancing overall performance.
- **Hardware requirements** for parallelism include the need for parallel hardware like multi-core processors.
- **Concurrency support** is often implemented alongside parallelism to manage tasks efficiently.
- An **analogy** for parallelism is house cleaning, where a group of friends clean different parts of the house simultaneously. Each person handles a specific task, resulting in faster overall completion compared to sequential cleaning.

### Synchronous vs Asynchronous Execution

Synchronous and asynchronous execution describe how tasks are initiated and completed in relation to other tasks.

#### Synchronous Execution
Synchronous execution requires that one task must be completed before the next one can start. This method is straightforward but can lead to inefficiencies if tasks involve waiting periods.

Characteristics of Synchronous Execution:

- The **blocking** nature of synchronous execution means each task waits for the previous one to complete before starting.
- **Predictability** is an important characteristic, as the order of task completion is easier to predict.
- The **simplicity** of synchronous execution makes it straightforward to implement and understand.
- An **analogy** for synchronous execution is waiting in line, like standing in a queue where each person waits their turn and cannot proceed until the person in front has finished.

#### Asynchronous Execution
Asynchronous execution allows tasks to be initiated and run independently of each other. This method is non-blocking and can improve efficiency, especially for tasks involving I/O operations or network requests.

Characteristics of Asynchronous Execution:

- Asynchronous tasks that are **non-blocking** allow processes to proceed without waiting for other tasks to complete.
- **Efficiency** is significantly improved by handling multiple tasks more effectively, avoiding idle times during wait periods.
- The **complexity** of asynchronous tasks arises from the need for mechanisms to manage tasks and handle completion callbacks or events.
- An **analogy** for asynchronous tasks is cooking multiple dishes simultaneously. For instance, while one dish is baking in the oven, you can chop vegetables for another, thereby making the overall cooking process more efficient.

### Comparison of Execution Paradigms 

| **Characteristic**              | **Sequential**                                                                                                                                         | **Non-Sequential**                                                                                                                      | **Concurrent**                                                                                                                        | **Parallel**                                                                                                                           | **Asynchronous**                                                                                                                       | **Synchronous**                                                                                                                         |
|---------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------|
| **Definition**                  | Tasks are executed one after another in a specific order.                                                                                               | Tasks do not follow a strict order of execution.                                                                                       | Multiple tasks make progress within overlapping time periods.                                                                         | Multiple tasks are executed simultaneously on multiple processors.                                                                     | Tasks are initiated and proceed independently of other tasks, often with callbacks or promises to handle completion.                    | Tasks are executed one after another, where each task waits for the previous one to complete.                                           |
| **Order of Execution**          | Strict and predictable order.                                                                                                                           | Flexible order.                                                                                                                         | Overlapping time periods, but not necessarily simultaneous.                                                                           | Simultaneously on different processors.                                                                                                | Initiated independently, can complete at any time.                                                                                      | Strict and predictable order, waiting for each task to complete before starting the next.                                                |
| **Task Scheduling**             | Simple and straightforward.                                                                                                                             | Flexible, can change dynamically.                                                                                                       | Managed by the scheduler to ensure progress of all tasks.                                                                             | Managed by the system, often requires multiple processors.                                                                             | Can be scheduled to run at any time, often managed by event-driven programming models.                                                  | Requires each task to wait for the previous one to finish.                                                                               |
| **Complexity**                  | Low complexity, easy to design and debug.                                                                                                               | More complex due to flexibility.                                                                                                        | Moderate complexity, requires careful management of resources.                                                                        | High complexity, requires handling of synchronization and resource sharing.                                                            | Moderate to high complexity, requires handling of callbacks, promises, or other mechanisms to manage independent tasks.                 | Low complexity, but can lead to inefficiencies if tasks have to wait for long periods.                                                   |
| **Performance**                 | Can be slower due to waiting for each task to complete before starting the next.                                                                         | Can be efficient if tasks are independent and do not need a specific order.                                                            | Can improve performance by overlapping tasks, but may involve context switching overhead.                                              | Can significantly improve performance for CPU-bound tasks by using multiple processors.                                                 | Can improve responsiveness and resource utilization, especially in I/O-bound tasks.                                                     | Can lead to inefficiencies if tasks involve a lot of waiting, as each task has to wait for the previous one to complete.                  |
| **Use Cases**                   | Simple scripts, batch processing where order is crucial.                                                                                                | Event-driven systems, where tasks can be handled as they come.                                                                         | Multithreading applications, GUI applications where multiple tasks need to be handled concurrently.                                   | High-performance computing, data processing tasks that can be divided into smaller, independent tasks.                                  | Web servers handling multiple requests, applications with I/O operations where waiting for responses would be inefficient.              | Systems where order of operations is crucial and tasks are dependent on the completion of previous tasks, such as transaction systems. |

## Process

A process is an instance of a program that is currently executing on a computer. It represents a program in action, containing the program code and its current activity. When you use any application on your computer, such as a web browser, text editor, or video player, it runs as a process. In some cases, a single application can have multiple processes running simultaneously to handle different tasks more efficiently. Additionally, the operating system uses processes to perform tasks in the background while you work, such as managing system resources, running scheduled tasks, or handling network communications.

### Characteristics of a Process

- Each process is assigned a **unique process identifier (PID)**, which helps the OS manage and differentiate between various processes.
- The **program counter** keeps track of the address of the next instruction to be executed for the process.
- **Process state** represents the current status of the process, such as running, waiting, etc.
- **Memory allocation** for each process includes several segments: the code segment contains the executable code, the data segment stores global and static variables, the stack segment manages function calls and local variables, and the heap segment is used for dynamic memory allocation during the process runtime.

### Role of the Operating System (OS)

The operating system is responsible for managing all processes running on the computer. It ensures that each process gets the necessary resources and time to execute its tasks while maintaining the overall efficiency and stability of the system.

Responsibilities of the OS in Process Management:

- The OS performs **process scheduling** by using algorithms to determine the order in which processes run, ensuring fair distribution of CPU time among processes.
- **Resource allocation** is managed by the OS, allocating CPU time, memory, and I/O devices to processes as needed, preventing conflicts and ensuring efficient utilization.
- Through **process isolation**, each process operates in its isolated memory space, preventing interference from other processes and enhancing security and stability.
- **Inter-process communication (IPC)** mechanisms provided by the OS allow processes to communicate and synchronize with each other, using methods like message passing, shared memory, and semaphores.
- The OS manages **process creation and termination**, handling the creation of new processes and ensuring resources are properly released and reclaimed after a process terminates.

### Process Table

The OS maintains a table known as the process table, which keeps track of all processes' status. This table contains information about each process, including its PID, state, priority, program counter, memory allocation, and other attributes. The process table allows the OS to efficiently manage and switch between processes.

### States of a Process

A process can be in one of several states during its lifecycle. The three primary states are:

- A process in the **running** state is currently using the CPU, has all the resources it needs, and has been granted permission by the OS to execute. At any given moment, only one process per CPU core can be in this state.
- When a process is in the **ready** state, it is waiting for permission from the OS to use the CPU. It is ready to run but must wait for the CPU to become available. Multiple processes can be in the ready state simultaneously, queued up and waiting their turn.
- The **waiting (or blocked)** state occurs when a process is waiting for an external event to occur, such as user input, disk access, or a network response. In this state, the process is not using the CPU, allowing other processes to be executed in the meantime.

## Thread

In computer science, a thread refers to a sequence of instructions that can be executed concurrently with other threads within a program. A process can be comprised of one or more threads, with each thread being provided with its own program counter (PC), register set, and stack space. Threads share resources such as code section, data section, and OS resources such as open files and signals with other threads within the same process. Threads are sometimes called lightweight processes because they have some similarities to processes.

Threads can be used to improve the performance and concurrency of programs. For example, a text editor can employ several threads, one to format the text, another to receive inputs, and so on. The use of multiple threads within a program can allow multiple operations to be performed simultaneously, which can lead to faster execution times and improved performance.

### Role of the Operating System (OS)

The operating system (OS) plays a pivotal role in the management of threads and processes. It acts as an intermediary between the hardware and the software applications, ensuring efficient and fair allocation of system resources. Here's a detailed explanation of how the OS manages threads and processes:

#### Thread Management and Scheduling

- The OS uses a component called the **scheduler** to control CPU access. The scheduler determines which threads are to be executed by the CPU at any given time. It switches between threads to ensure that all active threads get a chance to run, a process known as context switching.
- Each thread is allocated a **time slice**, which is a small duration of CPU time. If a thread does not complete its task within its time slice, it is put back in the queue to wait for its next turn.
- **Prioritization** of threads is managed by assigning numerical priority values. Higher priority threads are scheduled more frequently than lower priority threads. This allows critical tasks to be executed more promptly while less critical tasks wait longer.
- The OS supports **multithreading and concurrency**, allowing multiple threads to be executed concurrently. This improves the overall efficiency of the system by utilizing idle CPU cycles for other tasks.

#### Memory Allocation in a Program

A program's memory allocation is often divided into four segments:

- The **code** segment contains the compiled code of the program.
- **Data** segment stores global and static variables.
- **Stack** segment is used for function call management, including local variables and control flow.
- **Heap** segment is utilized for dynamic memory allocation during runtime.

## Processes vs Threads
A process is an independent instance of a program in execution. It contains the program code and its current activity. Processes are isolated from each other and do not share memory space directly, which makes them suitable for running independent applications.

Characteristics of Processes:

- Processes operate with **independence**, each having its own memory space.
- **State information** for each process includes its current activity status, such as ready, running, waiting, or stopped.
- **Address space** for processes is separate, preventing direct access to each other's memory.
- **Inter-process communication (IPC)** mechanisms like pipes, sockets, or shared memory are used by processes to communicate, as they cannot access each other's memory directly.

A thread is the smallest unit of execution within a process. Multiple threads can exist within the same process and share the same memory space, which allows them to access the same data and resources efficiently.

Characteristics of Threads:

- A thread represents an **execution path** that is independent within a process.
- Threads are a **subset of a process**, sharing most of the process's resources.
- **Scheduling** of threads is managed by the OS, with threads within the same process sharing the same address space.
- **Shared resources** among threads within a process include the same memory segments (code, data, and heap), except for the stack. Each thread has its own call stack to manage its function calls and local variables.

Differences Between Processes and Threads:

| Aspect                  | Process                                 | Thread                                  |
|-------------------------|-----------------------------------------|-----------------------------------------|
| **Independence**        | Processes are independent instances that run in separate address spaces. | Threads are subsets of a process and run within the same address space as the process. |
| **Memory**              | Processes have separate address spaces, meaning each process has its own memory area. | Threads share the address space of their parent process, allowing them to access the same memory and data. |
| **Communication**       | Processes require Inter-Process Communication (IPC) mechanisms like pipes, message queues, or shared memory to communicate with each other. | Threads can communicate directly by accessing shared memory within the process. |
| **State Information**   | Processes carry considerable state information, including process ID, process state, memory information, and more. | Threads maintain minimal state information, typically just a thread ID, program counter, register set, and stack. |
| **Resource Sharing**    | Processes do not share resources directly with each other; each process has its own resources. | Threads share resources such as code, data, and open files with other threads within the same process. |
| **Creation Overhead**   | Process creation has higher overhead because it involves allocating a separate memory space and other resources. | Thread creation has lower overhead since threads share resources and memory with the parent process. |
| **Context Switching**   | Context switching between processes involves more overhead due to switching separate memory spaces and state information. | Context switching between threads is faster and involves less overhead because they share the same memory space and resources. |

Here's a table comparing the life cycle stages of threads and processes:

| Life Cycle Stage       | Thread Life Cycle                        | Process Life Cycle                     |
|------------------------|------------------------------------------|----------------------------------------|
| **New**                | Thread is in the process of being created. | Process is in the process of being created. |
| **Ready**              | Thread is ready to run when CPU is available. | Process is ready to execute when CPU is available. |
| **Running**            | Thread is actively executing instructions. | Process is actively executing instructions. |
| **Waiting/Blocked**    | Thread is waiting for resources or I/O.  | Process is waiting for resources or I/O. |
| **Timed Waiting**      | Thread is waiting for a specified time.  | (Not typically a separate state, but can be considered under waiting) |
| **Terminated**         | Thread has completed execution or is aborted. | Process has completed execution or is terminated. |
| **Ready Suspended**    | (Not typically a separate state, but can be inferred from 'Ready' and 'Blocked') | Process is in secondary storage and ready to execute when moved to main memory. |
| **Blocked Suspended**  | (Not typically a separate state, but can be inferred from 'Blocked') | Process is in secondary storage and waiting for an event before moving to main memory. |

Practical Implications:

- **Performance** benefits arise because threads are generally faster to create and manage compared to processes. This efficiency is due to threads sharing the same memory space and resources, which reduces the overhead of context switching.
- **Isolation** provided by processes is better and more secure since they do not share memory. This robust isolation prevents one process from affecting another.
- **Concurrency** is enhanced through multithreading within a process, allowing for concurrent execution of tasks. This improves efficiency, especially in CPU-bound operations.

## CPU-Bound vs I/O-Bound

To optimize a software program's performance, it's crucial to identify the primary source of slowdown or the bottleneck. This bottleneck could either be due to I/O (input/output) activities or an underutilized CPU. Different strategies can be applied based on the cause of the bottleneck. Understanding the nature of the bottleneck helps in choosing the right optimization techniques to enhance the software's performance.

### CPU-Bound

When a task's completion time mainly depends on the speed of the CPU, it is considered CPU-bound. In simpler terms, the CPU's processing power determines how quickly the task finishes. A task is CPU-bound if it spends most of its time executing computations rather than waiting for I/O operations.

Characteristics of CPU-Bound Tasks:

- **High CPU usage** is typically exhibited when tasks cause the CPU to be almost always busy, leading to the system showing high CPU utilization.
- Tasks that have **minimal I/O wait time** indicate that they spend very little time waiting for I/O operations, such as disk reads or writes.

Strategies for Optimizing CPU-Bound Tasks:

- One effective strategy is **utilizing parallelism** by dividing a task into smaller subtasks that are processed simultaneously. This approach can be implemented using multithreading or multiprocessing techniques, where multiple threads or processes run concurrently, effectively leveraging multiple CPU cores.
- Another strategy involves **improving the efficiency of algorithms** used in the task. Optimized algorithms can significantly reduce both the computational load and the overall processing time required for the task.
- A hardware-related strategy includes **upgrading to a CPU** with a higher clock rate or more cores. A faster CPU can process tasks more quickly, while multiple cores can handle parallel execution more effectively, enhancing overall performance.
  
Example of a CPU-Bound Scenario:

```bash
I/O waiting
CPU Processing  ----Task 1---->----Task 2---->
```

In this example, the CPU is continuously processing tasks with minimal waiting for I/O operations. The speed at which tasks are completed is primarily limited by the CPU's processing power.

### I/O-Bound

A task is I/O-bound when its completion time mostly relies on the time spent waiting for input/output operations to finish. In simpler terms, the efficiency of I/O components determines how fast the task is completed. A task is I/O-bound if it spends most of its time waiting for I/O operations, such as reading from or writing to a disk, network communication, or other I/O devices.

Characteristics of I/O-Bound Tasks:

- Tasks with **high I/O wait time** spend a significant amount of time waiting for I/O operations to complete.
- **Low CPU usage** is observed when the CPU is often idle, waiting for I/O operations to finish before it can continue processing.

Strategies for Optimizing I/O-Bound Tasks:

- Improving performance through **concurrency** involves executing multiple tasks in overlapping time periods. This can be achieved using asynchronous programming, where tasks are initiated, and the CPU continues to execute other tasks while waiting for I/O operations to complete.
- **I/O optimization** can significantly enhance performance by improving the efficiency of I/O operations. Techniques such as caching frequently accessed data, using efficient data formats, or optimizing database queries can be employed.
- Upgrading to **faster I/O components** can also be a beneficial strategy. This includes using quicker memory I/O, faster hard drives (e.g., SSDs instead of HDDs), or improved network interfaces, which can reduce the time spent on I/O operations.

Example of an I/O-Bound Scenario:

```bash
I/O waiting         -----request----->     ------request------>     ------request------>
CPU Processing  --->                  ---->                    ---->
```

In this example, the CPU is often waiting for I/O operations to complete before it can continue processing. The speed at which tasks are completed is primarily limited by the efficiency of the I/O operations.
