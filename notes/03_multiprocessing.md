## Multiprocessing

**Multiprocessing** runs work across multiple processes. Each process normally has its own virtual address space, so processes are more isolated from one another than threads, which typically share a process's memory. This isolation reduces accidental shared-state interference and improves fault isolation. Multiprocessing is often used to take advantage of multiple CPU cores for CPU-bound work. Processes communicate through inter-process communication (IPC) mechanisms such as pipes, sockets, message queues, or explicitly shared memory. The trade-off is higher creation, memory, and communication overhead than with threads.

### Introduction to Processes

In computing, a process is an instance of a program in execution. It includes the program code, execution state, and associated resources such as memory and file descriptors. Processes are fundamental to multitasking because they allow multiple programs to make progress concurrently on one system. A process can create other processes, called child processes. The creating process is the parent and can monitor, communicate with, and wait for its children.

#### Child Processes

A **child process** is a process created by another process, known as the **parent process**. Child processes let an application delegate work to separate execution contexts. Depending on the workload and available CPU cores, this can improve throughput, resource use, responsiveness, or fault isolation.

**Characteristics of Child Processes**

- Each child process normally operates independently with its own virtual address space.
- Multiple child processes can run concurrently and may run in parallel on different CPU cores.
- Parent and child processes can exchange information through inter-process communication (IPC).
- The parent can create, monitor, signal, and wait for child processes.

**Parent and Child Process Relationship**

The relationship between parent and child processes is hierarchical. Here are the roles and responsibilities of each:

**Parent Process**:

- Creates child processes.
- Monitors or signals child processes when needed.
- Can continue working concurrently and wait for children when their results or exit status are needed.

**Child Process**:

- May inherit attributes from the parent, such as environment variables and open file descriptors, depending on how it is created.
- Runs independently but can communicate with the parent or other processes through IPC.
- Terminates when its work finishes, when it exits explicitly, or when it is terminated externally.

**Diagram of Parent and Child Process Relationship**:

```
+-------------------+
|  Parent Process   |
|                   |
|  +-------------+  |
|  | Child Proc  |  |
|  | (Process 1) |  |
|  +-------------+  |
|                   |
|  +-------------+  |
|  | Child Proc  |  |
|  | (Process 2) |  |
|  +-------------+  |
|                   |
+-------------------+
```

**Creating Child Processes**

Creating child processes involves spawning new processes from an existing parent process. Different programming languages and operating systems provide methods and APIs for process creation. Here are the common approaches:

- In UNIX-based systems, forking involves the parent process creating a child process by duplicating itself, allowing both processes to continue execution independently from the point of the fork.  
- On both Windows and UNIX systems, spawning allows the parent process to start a new child process, with the child initializing independently rather than inheriting the exact state of the parent.  
- High-level libraries in programming languages, such as Python's `multiprocessing.Process`, offer simplified abstractions for process creation, reducing the complexity of manually handling system calls.  
- After `fork()`, the child has a separate virtual address space that initially mirrors the parent's. Operating systems commonly use copy-on-write so unchanged pages can share the same physical memory until either process writes to them.  
- Spawning generally involves starting a completely new process that does not share memory or state with the parent, making it more isolated but potentially slower to initialize.  
- On UNIX systems, the `fork()` system call is commonly used for process creation, whereas on Windows, functions like `CreateProcess()` handle similar tasks.  
- Libraries like Python's `multiprocessing` module provide a consistent interface for process management across different operating systems, abstracting away platform-specific details.  
- Forking can be efficient when a child needs much of the parent's initial state because copy-on-write delays physical memory duplication until pages are modified.  
- Spawning is more suitable for processes that require distinct and isolated environments to avoid accidental data interference.  
- High-level abstractions also handle inter-process communication (IPC) and synchronization mechanisms, such as pipes and queues, making it easier for developers to manage complex workflows.  
- The choice between forking and spawning often depends on the operating system, programming language, and specific application requirements.  
- A common UNIX pattern is `fork()` followed by an `exec` family call, which replaces the child's process image with a new program.  
- Both techniques require careful consideration of process termination and resource cleanup to prevent issues like zombie processes or memory leaks.  
- High-level libraries often include utilities to manage process lifecycles, allowing developers to terminate or join processes without directly handling low-level details.

**Managing Child Processes**

Parent processes have several mechanisms to manage child processes effectively:

- Waiting for a child process to finish execution can be achieved by the parent using system calls like `wait` or `waitpid`, which also allow the parent to collect the child’s exit status to assess if it terminated successfully.  
- Monitoring the state of a child process enables the parent to identify conditions such as running (actively executing), sleeping (waiting for a resource), stopped (suspended), or zombie (completed but not reaped).  
- Inter-process communication (IPC) facilitates data or signal exchange between parent and child processes, with mechanisms including pipes, sockets, message queues, and shared memory providing flexible options for different use cases.  
- Terminating a child process can be necessary when it misbehaves or exceeds a time limit, and this is commonly achieved using system calls like `kill`, which send signals to the target process.  
- Using the `wait` system call ensures that zombie processes are avoided by allowing the parent to properly reap the terminated child process and free its resources.  
- Shared memory is a high-speed IPC mechanism that allows parent and child processes to directly access a common memory segment, but it requires careful synchronization to avoid race conditions.  
- Pipes are commonly used for unidirectional data transfer between processes, while sockets provide bidirectional communication and can work across network boundaries.  
- The `kill` system call sends specific signals to processes, which can either terminate the process or trigger custom signal handlers, depending on the signal type and the process setup.  
- Monitoring tools like `/proc` on UNIX systems provide real-time information about the state and resources of running or stopped processes, aiding in diagnostics.  
- Synchronization primitives like semaphores and mutexes can be used alongside IPC mechanisms to ensure orderly communication and prevent issues like deadlocks.
  
Different Process States:

| **State**  | **Description**                                         |
|------------|---------------------------------------------------------|
| Running    | The process is actively executing.                     |
| Sleeping   | The process is waiting for a resource.                 |
| Stopped    | The process is suspended.                              |
| Zombie     | The process has completed but has not been reaped by the parent. |

#### Zombie Process

A zombie process is a child process that has finished execution but still has an entry in the process table because its parent has not yet collected its exit status. A zombie no longer runs and uses very few resources, but it still consumes a process-table entry. If many zombies accumulate, they can eventually exhaust available process identifiers or process-table capacity.

Normal Process Termination:

```
+----------------------+
| Parent Process       |
|                      |
|  +-------------+     |
|  | Child Proc  |     |
|  | (Running)   |     |
|  +-------------+     |
|         |            |
|         V            |
|  Child Terminates    |
|         |            |
|  Parent Calls wait() |
|         |            |
|  Child Removed       |
+----------------------+
```

Zombie Process Scenario:

```
+--------------------------+
| Parent Process           |
|                          |
|  +-------------+         |
|  | Child Proc  |         |
|  | (Terminated)|         |
|  +-------------+         |
|         |                |
|         V                |
| Parent Does Not wait()  |
|         |                |
|  Child Status: Zombie    |
+--------------------------+
```

Visualization of Zombie in Process Table:

```
Process Table:
+----+-----------------+----------+
| PID| Process Name    | Status   |
+----+-----------------+----------+
|1000| Parent Process  | Running  |
|1001| Child Process A | Zombie   |
|1002| Child Process B | Zombie   |
|... | ...             | ...      |
+----+-----------------+----------+
```

#### Orphan Process

An orphan process is a child whose parent terminates while the child is still running. On UNIX-like systems, the orphan is reparented to a designated system process, often PID 1 or a configured subreaper. The child continues running normally. When it later exits, its new parent is responsible for collecting its exit status so it does not remain a zombie.

Normal Process Hierarchy:

```
+-------------------+
|  Parent Process   |
|                   |
|  +-------------+  |
|  | Child Proc  |  |
|  | (Running)   |  |
|  +-------------+  |
|                   |
+-------------------+
```

Orphan Process Scenario:

```
Initial State:
+-------------------+
|  Parent Process   |
|                   |
|  +-------------+  |
|  | Child Proc  |  |
|  | (Running)   |  |
|  +-------------+  |
|                   |
+-------------------+

Parent Terminates:
+-------------------+
| Parent Process    | (Terminated)
|                   |
+-------------------+
        |
        V
+-------------------+
| init / subreaper  |
| (new parent)      |
|                   |
|  +-------------+  |
|  | Child Proc  |  | (Adopted)
|  | (Running)   |  |
|  +-------------+  |
|                   |
+-------------------+
```

Visualization of Orphan Adoption:

```
Original Hierarchy:
[Parent Process] ---> [Child Process]

After Parent Terminates:
[init/subreaper] ---> [Orphan Child Process]
```

### Communication Between Processes

Effective multiprocessing often requires processes to communicate with each other to share data, synchronize actions, or coordinate tasks. There are several methods to facilitate this communication, each with its own advantages and limitations. Choosing the appropriate method depends on factors like the amount of data being transferred, the need for synchronization, and whether the processes are running on the same or different machines.

#### Message Passing

Message passing is a communication method where processes exchange data through messages. This method can be implemented using various Inter-Process Communication (IPC) mechanisms, such as:

- **Message Queues** provide a queueing system where messages are stored until the receiving process retrieves them. They are useful for asynchronous communication, allowing processes to send and receive messages independently of each other's execution state.
- **Sockets** enable communication between processes over a network, making them suitable for both local and remote communication. They can be used for stream-based (TCP) or datagram-based (UDP) communication, depending on the needs for reliability and speed.
- **Signals** are simple notifications sent to a process to indicate an event has occurred. They are often used for simple synchronization or to trigger an action but carry limited information compared to other methods.
  
```
+-------------+          +---------------+          +-------------+
| Producer    |          | Message Queue |          | Consumer    |
| (Process A) | ------>  |               | ------>  | (Process B) | 
+-------------+          +---------------+          +-------------+

Flow of Messages:
Producer sends messages to the Message Queue.
Consumer retrieves messages from the Message Queue.
```

Message passing preserves process isolation because data is exchanged explicitly rather than through ordinary shared variables. Its main cost is communication overhead, which may include serialization, copying, buffering, and system calls. Ordering and delivery guarantees depend on the IPC mechanism and can become more complex in distributed systems.

#### Shared Memory

Shared memory maps a common memory region into multiple processes, allowing them to exchange data without repeatedly copying it through an IPC channel. It can be efficient for large or latency-sensitive data transfers, but the processes must coordinate access explicitly.

```
+---------------------+
|    Shared Memory    |
| +-----------------+ |
| |   counter: 0    | |
| +-----------------+ |
+---------------------+

+-----------+          +-----------+
| Process A |          | Process B |
+-----------+          +-----------+

Execution Timeline Without Synchronization:

Time Step | Process A Actions       | Process B Actions       | Shared Memory State
------------------------------------------------------------------------------------
   1      | Read counter (0)        | Read counter (0)        | counter = 0
   2      | Increment to 1          | Increment to 1          | counter = 0
   3      | Write 1 to counter      | Write 1 to counter      | counter = 1
```

However, shared memory requires careful management to prevent data corruption and ensure consistency. Challenges include:

- Without proper synchronization mechanisms like locks, semaphores, or condition variables, concurrent access by multiple processes can lead to race conditions, where the outcome depends on the non-deterministic order of operations.
- Shared memory weakens the normal isolation between participating processes, so permissions and access to the shared region must be managed carefully.
- Shared memory is generally **limited** to processes running on the same machine, as it involves direct access to memory. While there are techniques to extend shared memory across networked systems, such as distributed shared memory, they introduce additional complexity and overhead.

#### Pipes

Pipes are a simple IPC mechanism for streaming bytes between processes. A single pipe is typically unidirectional, so bidirectional communication normally uses two pipes or a duplex IPC mechanism. Two common forms are:

- **Anonymous Pipes** are used for communication between processes that have a common ancestor, typically a parent-child relationship. They are created using system calls and provide a unidirectional channel for data flow. Anonymous pipes are often used for simple data transfer where one process writes data to the pipe, and the other reads it. However, they do not support complex communication patterns, and their lifespan is tied to the processes that use them.
- **Named Pipes**, also known as FIFOs on POSIX systems, can be used by unrelated processes because they are referenced by a filesystem name. POSIX FIFOs are local IPC objects and are conceptually one-way byte streams; two FIFOs are commonly used for two-way communication. Windows named pipes have different semantics and can support duplex and remote communication. Permissions and correct endpoint cleanup still need to be managed carefully.

```
Initial State:
+-----------+                  +-----------+
| Process A |                  | Process B |
+-----------+                  +-----------+

Creating Pipe:
+-----------+        Pipe        +-----------+
| Process A | ------------------> | Process B |
| (Write)   |                      | (Read)    |
+-----------+                  +-----------+

Data Transmission:
1. Process A writes "1" to the pipe.
2. Process B reads "1" from the pipe.
3. Process A writes "2" to the pipe.
4. Process B reads "2" from the pipe.
5. ... and so on.

Final State:
+-----------+        Pipe          +-----------+
| Process A |       (Closed)       | Process B |
| (Closed)  |                      | (Closed)  |
+-----------+                      +-----------+
```

Pipes are advantageous because they are lightweight and provide a straightforward mechanism for data streaming. However, they have limitations in terms of buffering capacity and are primarily suited for unidirectional or limited bidirectional communication. Additionally, pipes do not provide built-in mechanisms for complex synchronization, so additional coordination may be necessary for more sophisticated communication patterns.

### Challenges with Multiprocessing

Multiprocessing introduces several challenges, particularly in managing and coordinating independent processes. These challenges include debugging, resource contention, ensuring process synchronization, and managing the overhead associated with inter-process communication.

#### Debugging

Debugging multiprocessing applications is inherently more complex than debugging single-process applications. Each process may have its own set of bugs and issues, and the interaction between processes can introduce additional challenges. Standard debugging tools often focus on a single process, making it necessary to debug each process individually and then understand how they interact. Specific challenges include:

- Timing-dependent race conditions can be difficult to reproduce because they may appear only under particular scheduling or load conditions.
- The parallel nature of multiprocessing can lead to nondeterministic behavior, where the same inputs do not always produce the same outputs due to the variability in process scheduling and execution order.
- Effective logging and monitoring are essential but challenging in a multiprocessing environment, as logs from different processes need to be correlated accurately.

#### Deadlocks

Deadlocks occur when a set of processes are unable to proceed because each process is waiting for resources held by others, creating a cycle of dependencies that prevents any process from continuing. 

```
Initial State:
+----------+          +----------+
| Resource |          | Resource |
|    R1    |          |    R2    |
+----------+          +----------+

+-----------+          +-----------+
| Process A |          | Process B |
+-----------+          +-----------+

Execution Timeline:

1. Process A requests R1
2. Process B requests R2
3. Process A acquires R1
4. Process B acquires R2
5. Process A requests R2
6. Process B requests R1

Deadlock State:
+-------------+          +-------------+
| Process A   |          | Process B   |
| Holds R1    |          | Holds R2    |
| Waiting: R2 | <------> | Waiting: R1 |
+-------------+          +-------------+
```

Deadlocks are characterized by four conditions:

- Resources are allocated to only one process at a time.
- A set of processes are waiting on each other in a circular chain, with each process holding a resource the next process needs.
- Resources cannot be forcibly taken from a process; they must be released voluntarily.
- Processes hold onto resources they already have while waiting for additional resources.

Preventing or mitigating deadlocks requires careful design, such as implementing resource allocation strategies, imposing ordering on resource acquisition, or employing deadlock detection and recovery mechanisms.

#### Data Races

When processes explicitly share memory, unsynchronized concurrent accesses can create race conditions similar to data races in multithreaded code. If multiple participants access the same location and at least one writes without adequate synchronization, updates can be lost or inconsistent values can be observed.

```
Shared Variable:
+---------+
| counter | 
|   0     |
+---------+

Processes:
+-----------+                 +-----------+
| Process A |                 | Process B |
+-----------+                 +-----------+

Execution Timeline:

Time Step | Process A Actions        | Process B Actions        | Shared Variable State
-----------------------------------------------------------------------------------------
   1      | Read counter (0)         |                           | counter = 0
   2      |                         | Read counter (0)          | counter = 0
   3      | Increment value to 1     |                           | counter = 0
   4      | Write 1 to counter       |                           | counter = 1
   5      |                         | Increment value to 1     | counter = 1
   6      |                         | Write 1 to counter       | counter = 1

Final Value of `counter` = 1

Expected Value if Synchronized Properly = 2
```

 Challenges include:

- Identifying data races is challenging because they depend on the specific timing of process execution.
- To prevent data races, synchronization mechanisms like locks, semaphores, or message passing are used, which can introduce significant overhead and complexity, potentially reducing performance.

#### Resource Contention

Resource contention arises when multiple processes compete for the same limited resources, such as CPU time, memory, disk I/O, or network bandwidth. This competition can lead to:

- Processes may experience slowdowns due to waiting for resources to become available.
- Some processes may be perpetually delayed or blocked from accessing required resources, leading to inefficiencies.
- Efficiently scheduling processes to optimize resource use while minimizing contention is a complex problem, often requiring dynamic adjustment based on current system load.

### Process Management Techniques 

Process management keeps concurrent work safe and fast by following these best practices:

- Guard shared state with the right synchronization primitive (e.g., mutex, read-write lock, semaphore).
- Enforce a consistent lock order to prevent deadlocks.
- For shared-memory designs, use process-shared atomic operations for small hot counters or flags when the platform supports them.
- Shape workflow with bounded queues and rate limits to avoid overload.
- Balance work dynamically using techniques like a central queue or work stealing.
- Scale out behind a load balancer (LB) when possible.
- Track key metrics such as p95 latency and queue depth to monitor system health.
### Process Synchronization

Process synchronization coordinates concurrent processes so they can share resources and order dependent operations safely. When processes use shared memory, files, devices, or other common resources, synchronization primitives such as locks, semaphores, monitors, and condition variables can enforce mutual exclusion or ordering. Used correctly, they prevent race conditions; used poorly, they can also introduce deadlock, starvation, or unnecessary contention.

**Pick the right primitive**

* **Mutex**: single owner of a critical section.
* **RW Lock**: many readers or one writer.
* **Counting Semaphore**: N identical resources (e.g., a pool of 8 DB conns).
* **Condition Variable / Monitor**: wait for a state change.
* **Barrier**: wait until all participating workers reach a synchronization point.
* **Atomics (CAS/fetch-add)**: small shared counters or flags when the platform provides process-shared atomic access.

```
[Processes] -> [Wait Queue] -> [ CRITICAL SECTION ] -> [Release]
                 ^                                     |
                 +----------(blocked)------------------+
```

**Deadlock snapshot + prevention**

In an operating system with multiple *processes*, deadlock can occur when processes compete for limited resources (files, devices, memory segments). The four necessary conditions are:

1. *Mutual exclusion* – a resource can only be held by one process at a time.
2. *Hold and wait* – a process holds resources while requesting more.
3. *No preemption* – the OS cannot forcibly take resources away.
4. *Circular wait* – processes form a cycle, each waiting on the next.

*Example snapshot:*

```
 P1 holds DB, waiting for Cache
 P2 holds Cache, waiting for DB
 → Both processes stuck forever
```

*Prevention (resource ordering):*

Impose a strict global order for resource acquisition. If every process requests resources in the same order, cycles cannot form.

```
Order: DB (1) → Cache (2) → Log (3)

P1: lock(DB) -> lock(Cache) -> ...
P2: lock(DB) -> lock(Cache) -> ...
# Never request Cache before DB
```

This breaks the *circular wait* condition, preventing deadlock among processes.

**Semaphore patterns (bounded pool)**

A *counting semaphore* can control access to a *pool of limited resources* (e.g., DB connections, worker threads, file handles). The semaphore’s counter tracks how many slots are available.

* `wait(sem)` acquires a slot: if the counter is greater than zero, it decrements it; otherwise the process waits.
* `signal(sem)` releases a slot by incrementing the counter and potentially waking a waiter.
* At most `capacity` processes can use the resource concurrently.

```c
// counting semaphore initialized to 8
wait(sem);          // acquire a slot (block if all 8 in use)
resource_use();     // critical work using one resource
signal(sem);        // release slot (increment counter)
```

This ensures *bounded concurrency*: no more than eight processes can hold a slot at the same time.

**Producer–Consumer with semaphores (bounded buffer)**

The *producer–consumer problem* models two types of processes:

* *Producers* generate items and place them into a buffer.
* *Consumers* take items out of the buffer for processing.

To ensure *synchronization* and avoid race conditions:

* `empty` counts free slots in the buffer.
* `full` counts filled slots.
* `m` (a binary semaphore) enforces mutual exclusion on the buffer itself.

```pseudo
sem empty = N;   // free slots available
sem full  = 0;   // items available
sem m     = 1;   // mutex for buffer

Producer:                  Consumer:
wait(empty)                wait(full)     // wait if none full
wait(m)                    wait(m)        // lock buffer
enqueue(x)                 x = dequeue()
signal(m)                  signal(m)      // unlock buffer
signal(full)               signal(empty)  // update counters
```

Producers wait when the buffer is full, and consumers wait when it is empty. The binary semaphore `m` ensures that only one process manipulates the buffer at a time.

**Monitor sketch (one place to put state + waiting)**

A *monitor* is a high-level concurrency construct that combines:

* *Shared state* (e.g., a queue, buffer, or counter).
* *Mutual exclusion* (only one process/thread active inside).
* *Condition variables* to allow processes to wait and signal events.

**Bounded Buffer example (producer/consumer):**

```pseudo
monitor BoundedQ {
  queue q; int cap;
  cond not_full, not_empty;

  put(x):
    while q.size == cap: wait(not_full)
    q.push(x)
    signal(not_empty)

  get():
    while q.empty(): wait(not_empty)
    x = q.pop()
    signal(not_full)
    return x
}
```

Producers call `put` and wait if the buffer is full; consumers call `get` and wait if it is empty. The monitor provides mutual exclusion and condition-based waiting. Progress still depends on correct signaling and fair enough scheduling.

**Readers–Writers (favor readers, simple)**

The *readers–writers problem* arises when multiple processes/threads need *concurrent read access* to a shared resource, but *writes must be exclusive*.

* *Readers* can share the lock as long as no writer holds it.
* *Writers* need exclusive access, blocking all readers.
* In this simple version, *readers are favored*: many readers can “pile in” and delay a waiting writer.

```pseudo
rw_lock:
  read_lock():
    lock(rmutex)
    readers += 1
    if readers == 1: lock(wmutex)     # first reader blocks writers
    unlock(rmutex)

  read_unlock():
    lock(rmutex)
    readers -= 1
    if readers == 0: unlock(wmutex)   # last reader releases writers
    unlock(rmutex)

  write_lock():
    lock(wmutex)                      # exclusive access

  write_unlock():
    unlock(wmutex)
```

*Pros:* simple and efficient for read-heavy workloads.  
*Con:* writers may starve if readers keep arriving.

*When to avoid locks*: For very small shared counters or flags, process-shared atomics may be appropriate. For communication-heavy designs, queues or specialized lock-free structures such as MPSC/SPSC queues may reduce lock contention when their assumptions fit the workload.

#### Load Balancing

Load balancing in multiprocessing is the strategy of distributing tasks across multiple workers so that no single worker becomes a bottleneck. A good load balancer keeps all workers busy, minimizes idle time, and adapts to uneven or unpredictable workloads. Approaches range from simple central queues (easy but can bottleneck), to static splits (fast but fragile to imbalance), to more scalable techniques like dynamic scheduling, power of two choices, or work stealing, each trading off simplicity, overhead, and scalability.

**Central queue (simple & effective)**

* All tasks go into a *single shared queue*.
* *Workers* repeatedly pop from it.
* Queue length is a useful overload signal; a bounded queue can also apply backpressure when producers outpace workers.

```
          +-----------+
Producers→|   QUEUE   |← shared
          +-----------+
            ^   ^   ^
            |   |   |
           W1  W2  W3     (workers pop tasks)
```

*Pros:* simple, fair, and effective at balancing load.
*Cons:* contention on the queue can become a bottleneck at high worker counts.

**Work stealing (scales across many workers)**

* Each worker owns a *double-ended queue (deque)* of tasks.
* *Owner* takes tasks from the *bottom* (LIFO, good for cache locality).
* *Idle workers* steal from the *top* of others’ deques (FIFO, avoids contention).

```
 Worker 1 (busy)                  Worker 2 (idle)
 ┌─────────────────┐              ┌─────────────────┐
 │ [ d | e | q | u | e ] <---- steal from top ----  │
 └─────────────────┘              └─────────────────┘
             ^                                   ^
          pops bottom                       steals top
```

*Why it can scale well:*

* No central queue → avoids bottlenecks.
* Most operations are local (fast).
* Stealing is rare (only when idle), so contention is low.
* Balances work automatically across many threads.

**“Power of Two Choices” (low-cost balancing)**

Instead of checking **all workers**, each task just samples **two random workers** and goes to the one with the shorter queue.

$$
\text{Assign}(T) = \arg\min{Q(A), Q(B)}
$$

This tiny bit of choice dramatically reduces the maximum queue length compared to pure random assignment.

```
        Incoming Task T
               │
       ┌───────┴────────┐
   Pick 2 random      Workers
   {A, B}              ...
        │
   Compare queues
        │
  Send T to shorter
```

**Why Dynamic Scheduling Helps**

We have **10 tasks (in ms):**

$$
[50, 50, 50, 50, 50, 50, 50, 50, 50, 500]
$$

**Static split (5 + 5 tasks):**

* Worker A: $5 \times 50 = 250 \,\text{ms}$
* Worker B: $4 \times 50 + 500 = 700 \,\text{ms}$
* Both run in parallel → total time ≈ **700 ms** (dominated by slow worker).

```
Static:
 Worker A: [50][50][50][50][50]   → 250 ms
 Worker B: [50][50][50][50][500]  → 700 ms
 Total = 700 ms
```

**Dynamic (central queue):**

* Workers keep pulling tasks as they finish.
* If the 500 ms task is dequeued early, one worker can spend 500 ms on it while the other processes the short tasks.
* In that favorable schedule, the total time is about **500 ms**. If the long task is dequeued late, the total can still approach **700 ms**.

```
Dynamic (long task dequeued early):
 Worker A: [500]                  → 500 ms
 Worker B: [50]...[50] (9×)       → 450 ms
 Total ≈ 500 ms
```

Dynamic scheduling can reduce idle time and handle uneven workloads better than a fixed partition, but it does not guarantee an optimal schedule.

**Cluster patterns you’ll actually use**

* *Round-robin* (evenly spread): web backends w/ similar handlers.
* *Least-connections* (reduce tail latency): mixed endpoints.
* *Consistent hashing* (stickiness, cache locality):

```
[  hash ring  ]
A----B--------C---------A----B--------C
key→hash(key) -> next node clockwise
```

**What to measure (and act on)**

* *Queue length* → add workers / increase concurrency.
* *p95/p99 latency* → skew/hotspots; enable stealing or least-loaded.
* *Throughput vs. saturation point* → stop increasing concurrency past knee.
* *Steal rate / rebalancing rate* → if high, partitioning is off.

#### Scalability

Multiprocessing scalability is the ability of a system to effectively use additional CPUs or cores to achieve higher performance as workload increases. A scalable multiprocessing design keeps all processors busy with minimal contention, balances load across workers, and reduces overhead from coordination. The goal is near-linear speedup—doubling the number of processors should ideally almost double throughput—though in practice limits arise from synchronization, communication costs, and sequential portions of code.

**Vertical vs Horizontal Scaling**

*Vertical (scale-up):* 

Add more power (CPU/RAM) to a single machine.
→ Simpler, but limited by hardware ceiling.

```
┌───────────────┐
│   BIGGER BOX  │   (more CPU / RAM)
└───────────────┘
```

*Horizontal (scale-out):*

Add more machines behind a load balancer.
→ Scales further, but adds complexity (sync, failures, distribution).

```
              ┌───────────┐
              │   LB      │
              └─────┬─────┘
        ┌───────────┼───────────┐
        │           │           │
   ┌────▼────┐ ┌────▼────┐ ┌────▼────┐
   │ Server1 │ │ Server2 │ │ Server3 │  (N copies)
   └─────────┘ └─────────┘ └─────────┘
```

**Amdahl’s Law:**

For a fixed-size problem, the speedup is limited by the sequential fraction $S$:

$$
\text{Speedup}_{\text{Amdahl}} = \frac{1}{S + \frac{1-S}{P}}
$$

Example: if $S = 0.2$ (20% sequential, 80% parallel) and $P = 8$:

$$
\text{Speedup} = \frac{1}{0.2 + \frac{0.8}{8}} = 3.33\times
$$

**Gustafson’s Law:**

For scaling problem sizes with more processors, the speedup is estimated as:

$$
\text{Speedup}_{\text{Gustafson}} \approx P - S \cdot (P - 1)
$$

Example: with $S = 0.05$ and $P = 32$:

$$
\text{Speedup} \approx 32 - 0.05 \times 31 = 30.45\times
$$

**Contention & false sharing**

When multiple cores update different variables that reside in the *same shared cache line*, they can cause *false sharing*: cache-coherence traffic repeatedly moves or invalidates the line even though the variables are logically independent. In multiprocessing, this matters when the processes actually share the underlying memory.

*Bad (false sharing):*

```c
struct Counters { 
    int a; 
    int b;   // shares cache line with a
};
```

*Fix (pad or align to cache line, e.g. 64B):*

```c
struct Counters { 
    alignas(64) int a; 
    alignas(64) int b;  // each on its own cache line
};
```

With suitable alignment and layout, `a` and `b` are less likely to share a cache line, reducing false sharing.

**NUMA awareness checklist**

* Pin processes or threads when affinity is useful: `taskset -c 0-7 ./app`.
* Prefer NUMA-local allocation when workers mostly access local data.
* Consider per-NUMA-node worker or memory pools to reduce cross-node traffic.

**Backpressure & queues**

* Prefer *bounded* queues; drop/timeout instead of infinite growth.
* Use *circuit breakers* for downstreams; add *jittered backoff*.

### Typical Applications

| Use case | Pattern summary | C++ (preferred) | Python (preferred) | Why this choice | Shutdown behavior |
|---|---|---|---|---|---|
| CPU-bound data processing (e.g., image filtering, simulations) | Split data into chunks; run each chunk in a separate process | Worker processes via fork/exec + waitpid (or job system); supervised | ProcessPoolExecutor / multiprocessing.Pool (joinable) | True parallelism across cores; isolates crashes | Close/terminate pool, join with timeout; handle partial results |
| Batch video/audio transcoding | One media file per process; reuse a process pool | Process pool; spawn encoder subprocesses; wait on children | ProcessPoolExecutor / Pool.imap_unordered | Heavy CPU; process-per-job isolates codec crashes/leaks | Graceful: close & join; on cancel: terminate outstanding workers |
| Parallel compilation/build systems | Spawn compiler/linker processes concurrently | Subprocess fan-out; wait for all; cap by cores | ProcessPoolExecutor for compile tasks | Compilers are external processes; natural fit | Propagate cancel; kill/terminate remaining jobs |
| Web server CPU-heavy tasks (e.g., image resize, PDF render) | Pre-fork N worker processes; jobs via IPC/queue | Pre-fork model; master accepts, workers process | Gunicorn-style worker processes / ProcessPoolExecutor | Avoids GIL; isolates per-request crashes | Stop accepting, drain queue, graceful worker shutdown; SIGTERM then SIGKILL |
| ML inference (CPU) at scale | Warm worker processes with loaded models; serve via IPC/RPC | Worker processes pinned to cores/NUMA; shared mem for tensors | Multiprocessing workers; torch.multiprocessing or ProcessPoolExecutor | Reuse loaded models; parallelize safely; isolate OOMs | Stop intake, finish inflight, join; recycle on memory creep |
| Distributed training (1 process per GPU) | Launcher spawns one worker per device; collective communication | MPI processes or custom multiprocess with NCCL | torchrun/torch.multiprocessing spawn (joinable) | Process-per-GPU is the standard isolation model | Barrier then orderly exit; abort on failed rank |
| ETL / data pipeline (extract→transform→load) | Stage-per-process; bounded queues for back-pressure | Multiple processes with POSIX MQ/shared memory or ZeroMQ | multiprocessing.Process + Queue / Pool | Transforms are CPU-heavy; isolate faults; control memory | Send sentinels; close queues; join in stage order |
| Secure sandbox for untrusted code/plugins | Run plugin in locked-down process w/ low privileges | Fork, drop caps/seccomp/chroot; monitor via supervisor | Spawn Process with reduced perms; communicate over pipes | Process boundaries improve security/isolation | Timeout and terminate; collect logs/core for audit |
| Memory-leaky native libraries | Encapsulate leak-prone work in short-lived child processes | Spawn helper process per batch; restart often | ProcessPoolExecutor with max_tasks_per_child | Let OS reclaim memory on process exit | Let tasks finish, then recycle workers |
| CLI orchestration / pipelines | Chain external tools; stream via pipes | spawn/exec pipeline; wait in order | subprocess + multiprocessing for fan-out stages | Leverage existing tools; parallelize independent steps | Close unused pipe ends; terminate on error; collect return codes |
| GUI app: heavy compute off main process | Compute in child process; UI talks over IPC | Helper process; shared memory/ring buffer | multiprocessing.Process (joinable) with Queue/Manager | Avoid UI freezes/crashes; bypass GIL | Cancel, flush IPC, join before closing UI |
| Real-time market data parsing (CPU-heavy decode) | I/O thread feeds a parse process pool | Dedicated I/O proc + parse pool; lock-free shared mem | Process pool for decode; threads for I/O | Keep latency low; parallelize CPU parse safely | Stop intake, drain, terminate leftover workers |
| Web scraping at scale (parse/compute heavy) | Async/threads fetch; processes parse/compute | Fetcher process + parse pool; queue between | asyncio + ProcessPoolExecutor for CPU parsing | Split I/O-bound from CPU-bound for throughput | Cancel fetchers, finish parse queue, join pool |
| High-availability service: master + worker processes | Master supervises N workers; respawns on crash | Pre-fork with master; managed by systemd on prod | multiprocessing with a master; or supervisor/gunicorn prefork | Crash isolation and auto-restart resilience | Master stops intake, signals workers, waits; force-kill stragglers |
| Scheduled jobs / background services | Run as daemon/service outside main app | Daemonized process (double-fork/setsid) or systemd service | Standalone service process; avoid daemonic=True for critical work | Independent lifecycle; restart policy via OS | Handle SIGTERM cleanly; flush and exit |
| Large file compression/decompression | Chunk file and process chunks across workers | Worker processes; shared memory for chunk buffers | ProcessPoolExecutor; map over chunks | CPU-bound; avoids GIL and limits memory contention | Join pool; verify chunk order and final assembly |
| Scientific Monte Carlo simulations | Independent trials per process; aggregate results | Fork/exec workers; RNG seeds per process | multiprocessing.Pool with initializer seeding | Embarrassingly parallel workload | Close & join; checkpoint partial aggregates |
| MapReduce-style local batch | Map tasks in pool; reduce in parent process | Process pool; IPC for intermediate results | Pool.map + reduce step; or joblib (loky backend) | Clear separation of compute and aggregation | Complete reducers; handle failures idempotently |
| Image thumbnail generation service | Queue of images; N worker processes | Pre-fork workers, shared cache via shm/IPC | ProcessPoolExecutor; pre-load libraries per worker | CPU-heavy libraries; avoids GIL; isolates crashes | Stop intake, drain queue, join pool |
| PDF rendering/sanitization sandbox | Render in low-privilege process; return raster | Sandboxed child (seccomp/AppArmor); monitor | Spawn Process; drop privileges; time limits | Mitigate exploit risk from hostile inputs | Timeout -> terminate; clean temp files |
| Geo-spatial tiling/rasterization | Partition space into tiles; process tiles in parallel | Process pool; memory-mapped files for tiles | ProcessPoolExecutor; chunked tile jobs | Compute & memory intensive; isolation helps | Flush tile cache; join and verify coverage |

### Alternatives to Multiprocessing

Multiprocessing is one way to gain parallelism and isolation, but broader architectural approaches can address some of the same goals. The following options are not direct replacements for operating-system processes—in fact, many use processes internally—but they provide different ways to structure, deploy, isolate, or scale work:

#### Containers

Containers package applications and their dependencies into isolated runtime environments while sharing the host kernel. They are often used with microservices and deployment platforms to simplify packaging, rollout, and scaling. Advantages include:

- Containers isolate filesystems, processes, and other resources, reducing dependency conflicts and limiting some forms of interference between applications.
- Containers can be easily scaled up or down to meet demand, enabling efficient use of resources.
- By packaging all necessary components together, containers ensure that applications run consistently across different environments, from development to production.
- Containers can be quickly started or stopped, making them ideal for environments where quick scaling and deployment are necessary.

```
+-----------------------------------------------------+
|                     Host OS                         |
| +------------------+     +-----------------------+  |
| | Container Engine |<--->|    Container Image    |  |
| |  (e.g., Docker)  |     |  (App + Dependencies) |  |
| +------------------+     +-----------------------+  |
|          |                          |               |
|          |                          |               |
|          |                          |               |
|          V                          V               |
| +-----------------------------------------------+   |
| |               Container Runtime               |   |
| | +-----------+  +-----------+  +-----------+   |   |
| | | Container |  | Container |  | Container |   |   |
| | | Instance  |  | Instance  |  | Instance  |   |   |
| | | (App 1)   |  | (App 2)   |  | (App 3)   |   |   |
| | +-----------+  +-----------+  +-----------+   |   |
| +-----------------------------------------------+   |
+-----------------------------------------------------+
```

Containers may introduce overhead for short-lived processes due to the need to set up and tear down the container environment.

#### Event-Driven Architectures

Event-driven architectures revolve around the concept of events, which are messages or signals that indicate a change in state or the occurrence of an action. This architecture is often used in systems where activities are triggered by external inputs, such as user actions or sensor readings. Key benefits include:

- Event-driven systems can respond quickly to changes and inputs, providing a high level of interactivity.
- Components in an event-driven system are loosely coupled, making the system more modular and easier to maintain.
- These systems can handle high volumes of events by distributing the load across multiple event handlers.

Event-driven architectures are particularly well-suited for applications like real-time data processing, user interfaces, and IoT (Internet of Things) systems.

#### Microservices

Microservices architecture decomposes applications into smaller, independently deployable services that communicate through APIs. Each microservice handles a specific business functionality and can be developed, deployed, and scaled independently. Advantages include:

- Microservices can be updated and deployed without affecting the entire application, reducing downtime and deployment risks.
- Individual services can be scaled independently based on demand, optimizing resource usage.
- Different microservices can use different technologies and programming languages, allowing teams to choose the best tool for each job.
- Failures in one microservice do not necessarily affect others, improving the overall resilience of the system.

Managing a microservices architecture can be complex due to the need for effective communication and coordination between services.

#### Serverless Computing

Serverless computing, also known as Function-as-a-Service (FaaS), abstracts server management by allowing developers to run code in response to events without managing the underlying infrastructure. Code is executed in stateless functions, triggered by specific events such as HTTP requests or database changes. Key benefits include:

- Developers focus on writing code without worrying about server provisioning, maintenance, or scaling.
- Users pay only for the compute time consumed by their functions, potentially reducing costs for infrequently used applications.
- Serverless platforms automatically scale functions based on demand, ensuring efficient resource usage.
- Functions can be deployed rapidly, enabling faster development cycles.

Serverless computing is ideal for applications with variable workloads, such as data processing pipelines, real-time analytics, and microservices.

#### Virtual Machines (VMs)

Virtual machines provide a more traditional approach to achieving process isolation by running a complete operating system environment within a host system. Each VM has its own virtualized hardware, operating system, and applications. Benefits include:

- VMs provide strong isolation between different environments, making them suitable for running untrusted applications or multiple users on the same hardware.
- VMs can run different operating systems and configurations, offering flexibility for various applications and use cases.
- VMs allow precise allocation of resources (CPU, memory, storage) to different environments, ensuring optimal use of hardware.
- VMs can be used to run legacy applications that require specific older operating systems or configurations.

```
+-------------------------------------------------------+
|                     Host Hardware                     |
| +--------------------+     +-----------------------+  |
| |     Hypervisor     |<--->|      VM Manager       |  |
| | (Type 1 or Type 2) |     |  (e.g., VMware, KVM)  |  |
| +--------------------+     +-----------------------+  |
|          |                          |                 |
|          |                          |                 |
|          |                          |                 |
|          V                          V                 |
| +---------------------------------------------------+ |
| |                  Virtual Machines                 | |
| | +-------------+  +-------------+  +-------------+ | |
| | | VM Instance |  | VM Instance |  | VM Instance | | |
| | | (Guest OS   |  | (Guest OS   |  | (Guest OS   | | |
| | | 1)          |  | 2)          |  | 3)          | | |
| | +-------------+  +-------------+  +-------------+ | |
| | | App + OS    |  | App + OS    |  | App + OS    | | |
| | +-------------+  +-------------+  +----------  -+ | |
| +---------------------------------------------------+ |
+-------------------------------------------------------+
```

VMs tend to have higher overhead compared to containers, as they require running a full operating system instance for each VM.

**Comparison Summary**

| Feature           | Containers                                         | Virtual Machines                            |
|-------------------|----------------------------------------------------|---------------------------------------------|
| **Isolation**     | Application-level isolation using namespaces       | OS-level isolation with separate kernels    |
| **Resource Usage**| Lightweight, shares host OS kernel                 | Heavier, each VM runs its own OS             |
| **Startup Time**  | Usually fast (often seconds or less)               | Usually slower because a guest OS must boot   |
| **Portability**   | Highly portable across environments                | Portable but less flexible due to OS dependencies |
| **Use Cases**     | Microservices, scalable applications, CI/CD pipelines | Running multiple OSes, legacy application support, full isolation for security |

### Examples

#### Examples in C++

In C++, process creation and management are provided mainly by operating-system APIs rather than by a standard C++ process class. Processes normally have separate virtual address spaces and resources. Unlike threads in one process, they do not share ordinary application memory by default, so IPC or explicitly shared memory is required to exchange data.

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

A process can terminate normally with `exit()` or by returning from `main()`. A parent can wait for a child with `wait()` or `waitpid()` and then inspect the returned status to determine how the child exited.

```cpp
#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        std::cout << "Child process terminating." << std::endl;
        std::exit(0);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Child process exited with code "
                      << WEXITSTATUS(status) << std::endl;
        }
    } else {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }

    return 0;
}
```

Here, the parent waits for the child and uses the wait-status macros to obtain the child's normal exit code.

##### Inter-Process Communication (IPC)

Processes can communicate with each other using IPC mechanisms, such as pipes, message queues, shared memory, and sockets. Pipes are a simple and commonly used IPC method for one-way communication between processes.

```cpp
#include <iostream>
#include <cstring>
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

Shared memory maps the same region into multiple processes, providing a fast way to exchange data without copying it through a pipe or socket. Concurrent access still requires synchronization when operations can conflict.

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

Processes can be synchronized with primitives such as semaphores or process-shared mutexes. The following example uses a POSIX semaphore to signal that data in shared memory is ready to be read.

```cpp
#include <cstring>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    const int SIZE = 4096;

    void* shared_memory = mmap(
        nullptr, SIZE, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_t* sem = static_cast<sem_t*>(mmap(
        nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    sem_init(sem, 1, 0); // Shared between processes; initially not signaled

    pid_t pid = fork();

    if (pid == 0) {
        // Child process: produce data, then signal the parent.
        std::strcpy(static_cast<char*>(shared_memory),
                    "Child writes to shared memory");
        sem_post(sem);

        munmap(shared_memory, SIZE);
        munmap(sem, sizeof(sem_t));
        return 0;
    }

    if (pid > 0) {
        // Parent process: wait until the child has produced the data.
        sem_wait(sem);
        std::cout << "Parent reads: "
                  << static_cast<char*>(shared_memory) << std::endl;

        waitpid(pid, nullptr, 0);
        sem_destroy(sem);
        munmap(shared_memory, SIZE);
        munmap(sem, sizeof(sem_t));
        return 0;
    }

    std::cerr << "Fork failed!" << std::endl;
    return 1;
}
```

Here, the semaphore provides ordering: the parent blocks until the child has written the shared data and signaled that it is ready.

##### Performance Considerations and Best Practices

- Creating processes is generally more expensive than creating threads due to the need to allocate separate memory and resources. Minimize process creation by reusing processes when possible.
- IPC mechanisms like pipes and shared memory involve overhead. Use the most efficient IPC method for the task at hand and minimize the amount of data transferred.
- Ensure that IPC and synchronization mechanisms are used correctly to avoid deadlocks, where two or more processes are waiting indefinitely for each other.
- Properly manage resources such as file descriptors and shared memory to avoid resource leaks and ensure that resources are cleaned up when no longer needed.


| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/basic_process.cpp) | Create and start a basic process                      |
| 2   | [multiple_processes.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/multiple_processes.cpp) | Integrate multiple processes for a complex task       |
| 3   | [exec_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/exec_process.cpp) | fork+exec family (execvp/execl/execve), FD_CLOEXEC, pipe+dup2+exec pipeline |
| 4   | [signals.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/signals.cpp) | POSIX signals: sigaction, sig_atomic_t, SIGCHLD reaping, kill()+SIGUSR1 |
| 5   | [named_pipe.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/named_pipe.cpp) | Named pipes (FIFOs): mkfifo, unrelated-process IPC, EPIPE handling |
| 6   | [deadlock.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/deadlock.cpp) | ⚠️ ANTIPATTERN — circular-wait deadlock; exits via sem_timedwait timeout |
| 7   | [deadlock_avoidance.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/deadlock_avoidance.cpp) | Lock-ordering fix for deadlock.cpp; both processes acquire locks in same order |
| 8   | [process_pool.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/process_pool.cpp) | Use a process pool to manage concurrent tasks         |
| 9   | [message_queue.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/message_queue.cpp) | Communicate between processes using a message queue   |
| 10  | [pipe_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/pipe_communication.cpp) | Communicate between processes using a Pipe            |
| 11  | [shared_memory.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/shared_memory.cpp) | Use shared memory to store data between processes     |
| 12  | [shared_array.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/shared_array.cpp) | ⚠️ ANTIPATTERN — mmap shared array without synchronization (torn reads/writes) |
| 13  | [process_manager.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/process_manager.cpp) | Manager/worker pattern with fcntl file locking for safe concurrent appends |
| 14  | [process_mutex.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/process_mutex.cpp) | Use a mutex to synchronize access to shared resources |
| 15  | [process_semaphore.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/process_semaphore.cpp) | Use a semaphore to control access to shared resources |
| 16  | [process_barrier.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/process_barrier.cpp) | Use a barrier to synchronize multiple processes       |
| 17  | [orphan_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/orphan_process.cpp) | Demonstrate an orphan process scenario                |
| 18  | [zombie_process.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multiprocessing/zombie_process.cpp) | Demonstrate a zombie process scenario                 |

#### Examples in Python

In Python, the `multiprocessing` module creates and manages separate processes. Each process has its own interpreter and ordinary memory space, which provides stronger isolation than threads and allows CPU-bound Python code to run in parallel across cores in CPython.

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

A process can be asked to terminate with the `terminate()` method. This is an abrupt termination mechanism, so cleanup code in the child may not run. After `join()`, the `exitcode` attribute can be inspected to see how the process exited.

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

Python provides several ways for processes to communicate, including pipes, queues, and shared memory. Queues are convenient for safe message passing because the processes do not need to manipulate the same ordinary Python objects concurrently.

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

Synchronization between processes can use locks, events, conditions, semaphores, and related primitives. Locks and binary semaphores can provide mutual exclusion, while events and conditions are mainly used for notification and coordination.

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

Python's `multiprocessing` module provides process-based parallelism and isolation. It is especially useful for CPU-bound work in CPython and for designs that benefit from separate process state, at the cost of higher startup and IPC overhead.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/basic_process.py) | Create and start a basic process                      |
| 2   | [multiple_processes.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/multiple_processes.py) | Integrate multiple processes for a complex task       |
| 3   | [deadlock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/deadlock.py) | Demonstrate a deadlock scenario in multiprocessing    |
| 4   | [process_pool.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/process_pool.py) | Use a process pool to manage concurrent tasks         |
| 5   | [queue_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/queue_communication.py) | Communicate between processes using a Queue           |
| 6   | [pipe_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/pipe_communication.py) | Communicate between processes using a Pipe            |
| 7   | [shared_value.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/shared_value.py) | Use a shared value to store data between processes    |
| 8   | [shared_array.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/shared_array.py) | Use a shared array to store data between processes    |
| 9   | [manager.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/manager.py) | Use a manager to share complex data structures        |
| 10  | [process_lock.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/process_lock.py) | Use a lock to synchronize access to shared resources  |
| 11  | [process_semaphore.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/process_semaphore.py) | Use a semaphore to control access to shared resources |
| 12  | [process_barrier.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/process_barrier.py) | Use a barrier to synchronize multiple processes       |
| 13  | [orphan.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/orphan.py) | Demonstrate an orphan process scenario                |
| 14  | [zombie.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multiprocessing/zombie.py) | Demonstrate a zombie process scenario                 |

#### Examples in JavaScript

In Node.js, the `child_process` module can spawn programs, run commands, and communicate with child processes. JavaScript callbacks normally run on a single event-loop thread within each Node.js process, while the runtime may also use background threads internally. Child processes provide a separate process boundary and can run CPU work in parallel on other cores.

##### Creating Processes

Node.js provides several ways to create child processes, including `spawn`, `exec`, `execFile`, and `fork`. `spawn` launches a command directly, while Node's `fork` is a specialized helper for starting another Node.js process with an IPC channel; it is not the POSIX `fork()` system call.

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

const child = spawn('node', ['-e', 'console.log("Running"); setTimeout(() => {}, 10000)']);

setTimeout(() => {
  child.kill('SIGTERM');
  console.log('SIGTERM sent');
}, 2000);

child.on('exit', (code, signal) => {
  console.log(`Child exited: code=${code}, signal=${signal}`);
});
```

Here, the parent sends `SIGTERM` after two seconds and listens for the child's `exit` event to confirm that it has actually ended.

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

Node.js does not provide a general-purpose `ProcessPool` abstraction in `child_process`. A program can maintain a fixed set of child processes and reuse them, and server applications can also use the `cluster` module or external process managers for worker-style designs.

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

- Keep CPU-intensive work off the main event-loop thread when it would block request or event handling; child processes are one option when process isolation or separate CPU execution is useful.
- When using IPC, minimize the data transferred between processes to reduce overhead. Use efficient data formats and avoid unnecessary serialization.
- Ensure that processes are terminated gracefully. Handle cleanup tasks like closing database connections and freeing resources before terminating.
- Always handle errors when working with child processes. Use event listeners for `error`, `exit`, and `close` events to handle unexpected situations.
- Be cautious when executing external commands to avoid security vulnerabilities, such as command injection attacks. Sanitize inputs and validate all arguments.

Node.js's `child_process` module is useful for running external programs, isolating work, and executing CPU-heavy tasks in separate processes. Ordinary asynchronous I/O usually does not need a child process because Node's event loop already handles it efficiently.

| No. | Filename                                          | Description                                           |
|-----|---------------------------------------------------|-------------------------------------------------------|
| 1   | [basic_process.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/01_basic_process.js) | Create a child process with fork(); show PID and parent PID hierarchy |
| 2   | [multiple_processes.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/02_multiple_processes.js) | Integrate multiple processes for a complex task       |
| 3   | [deadlock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/03_deadlock.js) | ⚠️ ANTIPATTERN: circular lock acquisition causing deadlock; auto-detected after timeout |
| 4   | [process_pool.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/04_process_pool.js) | True process pool: persistent workers receive multiple CPU-bound tasks over IPC |
| 5   | [queue_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/05_queue_communication.js) | Communicate between processes using a Queue           |
| 6   | [pipe_communication.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/06_pipe_communication.js) | Communicate between processes using a Pipe            |
| 7   | [shared_value.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/07_shared_value.js) | Shared counter via parent-managed IPC state proxy     |
| 8   | [shared_array.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/08_shared_array.js) | Shared array via parent-managed IPC state proxy       |
| 9   | [manager.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/09_manager.js) | ProcessManager: spawn workers, collect output into a shared file |
| 10  | [process_lock.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/10_process_lock.js) | File-based mutex (wx flag) for cross-process critical sections |
| 11  | [process_semaphore.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/11_process_semaphore.js) | IPC-managed counting semaphore to limit concurrent workers |
| 12  | [process_barrier.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/12_process_barrier.js) | IPC-managed barrier: all workers complete phase 1 before any start phase 2 |
| 13  | [orphan.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/13_orphan.js) | ⚠️ ANTIPATTERN: parent exits before child; child becomes orphan (adopted by init/subreaper) |
| 14  | [zombie.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/14_zombie.js) | How Node.js/libuv prevents zombie processes via automatic waitpid() on child exit |
| 15  | [run_commands.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/15_run_commands.js) | exec() vs execFile() vs execSync(); maxBuffer; timeout; ⚠️ shell injection antipattern |
| 16  | [cluster_module.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/16_cluster_module.js) | cluster module: shared TCP port, load balancing, crash restart, graceful shutdown |
| 17  | [process_signals.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/17_process_signals.js) | SIGTERM/SIGINT graceful shutdown; forwarding signals from parent to forked children |
| 18  | [process_restart.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multiprocessing/18_process_restart.js) | Supervisor pattern: automatic restart with exponential backoff and max-restart limit |
