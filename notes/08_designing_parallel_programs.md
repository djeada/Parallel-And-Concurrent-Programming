# Designing Parallel Programs

Designing parallel programs involves breaking down computational tasks into smaller, concurrent units to be executed simultaneously. This approach leverages the power of multiple processors to enhance performance and efficiency. Key steps in this process include partitioning, communication, agglomeration, and mapping.

## Partitioning

Partitioning is a fundamental concept in computational problem-solving, where a large problem is divided into smaller, more manageable tasks. This approach allows tasks to be executed concurrently, improving efficiency and performance. In essence, partitioning is about breaking a problem into smaller pieces.

```
#
      +-----------------------+
      |     Big Problem       |
      +-----------------------+
                 |
     +-----------+-----------+
     |           |           |
+---------+ +---------+ +---------+
| Part 1  | | Part 2  | | Part 3  |
+---------+ +---------+ +---------+
```

### Goals

I. Balance the Workload Among Tasks

- Ensure that each task receives an approximately equal amount of work to avoid bottlenecks.
- This balance is crucial for maximizing the efficiency and utilization of computational resources.

II. Minimize Dependencies Between Tasks

- Reduce the communication and synchronization overhead between tasks.
- Independent tasks can be processed in parallel without frequent data exchange, improving overall performance.

### Types of Decomposition

I. Domain Decomposition

Domain decomposition involves splitting the data into partitions, with each partition processed by a separate task. This method is highly effective in scenarios where the data can be naturally divided into distinct segments.

Example:

In a weather simulation, the geographical area can be divided into smaller regions, with each region processed independently.

II. Functional Decomposition

Functional decomposition divides the computation into separate functional tasks, each performing a distinct part of the overall computation. This method focuses on breaking down the operations rather than the data.

Example:

In a web application, different tasks such as user authentication, data retrieval, and UI rendering can be handled by separate services.

IIII. Cyclic Decomposition

Cyclic decomposition is primarily used in the context of permutations and group theory. It breaks down a permutation into a collection of disjoint cycles, where each cycle represents a sequence of elements that are permuted among themselves.

Example:

Consider the permutation of the set ${1, 2, 3, 4}$ given by:

$$
\sigma = \begin{pmatrix}
1 & 2 & 3 & 4 \\
2 & 3 & 1 & 4
\end{pmatrix}
$$

This permutation can be decomposed into disjoint cycles as follows:

$$
\sigma = (1 \to 2 \to 3 \to 1)(4)
$$

Here, the cycle $(1 → 2 → 3 → 1)$ indicates that 1 is sent to 2, 2 is sent to 3, and 3 is sent back to 1. The cycle $(4)$ indicates that 4 remains fixed.

IV. Block Decomposition

Block decomposition is a technique used in matrix theory and linear algebra. It involves partitioning a matrix into smaller submatrices (blocks). This can simplify computations and is useful for analyzing the structure of the matrix.

Example:

Consider a matrix $A$ of size $4 \times 4$:

$$
A = \begin{pmatrix}
a_{11} & a_{12} & a_{13} & a_{14} \\
a_{21} & a_{22} & a_{23} & a_{24} \\
a_{31} & a_{32} & a_{33} & a_{34} \\
a_{41} & a_{42} & a_{43} & a_{44}
\end{pmatrix}
$$

This matrix can be decomposed into blocks:

$$
A = \begin{pmatrix}
A_{11} & A_{12} \\
A_{21} & A_{22}
\end{pmatrix}
$$

where each block is a submatrix:

$$
A_{11} = \begin{pmatrix}
a_{11} & a_{12} \\
a_{21} & a_{22}
\end{pmatrix}, \quad A_{12} = \begin{pmatrix}
a_{13} & a_{14} \\
a_{23} & a_{24}
\end{pmatrix}, \quad A_{21} = \begin{pmatrix}
a_{31} & a_{32} \\
a_{41} & a_{42}
\end{pmatrix}, \quad A_{22} = \begin{pmatrix}
a_{33} & a_{34} \\
a_{43} & a_{44}
\end{pmatrix}
$$

Block decomposition helps in performing matrix operations more efficiently by working with smaller submatrices.

## Communication

Communication in parallel computing involves the exchange of data between tasks. This process is essential when tasks depend on each other's results to proceed. Efficient communication is crucial for maintaining performance and minimizing delays.

### Goals

I. Minimize the Volume and Frequency of Communication

- Reducing the amount of data exchanged and the number of communications can significantly decrease overhead and improve overall performance.

II. Optimize the Use of Network Resources:

- Efficient use of network bandwidth and minimizing latency ensures faster data transfer and better utilization of computational resources.

I apologize for the omission. Here is the full text with ASCII graphics and examples for each type of communication, including the example for point-to-point communication.

### Types of Communication

I. Local Communication

Local communication refers to data exchange between tasks that reside on the same physical processor. This type of communication is generally faster due to lower latency and higher bandwidth within a single processor.

**Example:**

In a multi-core processor, tasks running on different cores can share data through shared memory.

```
+-----------+     +-----------+
| Core 1    | <-> | Core 2    |
| Task A    |     | Task B    |
+-----------+     +-----------+
      |               |
      +-------+-------+
              |
       Shared Memory
```

II. Global Communication

Global communication involves data exchange between tasks located on different physical processors. This type of communication often involves higher latency and lower bandwidth due to the physical distance and network constraints.

**Example:**

In a distributed computing system, tasks running on separate machines need to communicate over a network.

```
+-------------+       +-------------+
| Processor 1 | <----> | Processor 2 |
| Task A      |       | Task B      |
+-------------+       +-------------+
      |                     |
      +---------------------+
             Network
```

III. Point-to-Point Communication

Point-to-point communication is direct data exchange between pairs of tasks. It is the simplest form of communication, where one task (sender) sends data directly to another task (receiver).

```
+---------+     +---------+
| Sender  | --> | Receiver |
+---------+     +---------+
```

IV. Collective Communication

Collective communication involves multiple tasks and includes operations such as broadcast, scatter, and gather.

**Scatter Communication**

In scatter communication, a single process sends different pieces of data to multiple processes. Each process receives a unique piece of the data.

```
      +-----------------------+
      |         ABC           |
      +-----------------------+
                 |
     +-----------+-----------+
     |           |           |
+---------+ +---------+ +---------+
|    A    | |    B    | |    C    |
+---------+ +---------+ +---------+
```

**Broadcast Communication**

In broadcast communication, a single process sends the same piece of data to all other processes.

```
      +-----------------------+
      |         ABC           |
      +-----------------------+
                 |
     +-----------+-----------+
     |           |           |
+---------+ +---------+ +---------+
|   ABC   | |   ABC   | |   ABC   |
+---------+ +---------+ +---------+
```

### Communication Modes

I. Synchronous Blocking Communication

- Tasks wait until the entire communication process is complete before proceeding.
- Tasks cannot perform other work while waiting for the communication to finish.
- Ensures data integrity but can lead to idle times.

II. Asynchronous Nonblocking Communication

- Tasks initiate communication and then proceed with other work without waiting for the communication to complete.
- Allows for overlapping communication with computation, potentially improving efficiency.

### Key Concepts in Communication

I. Overhead

- The computational time and resources spent on communication.
- High overhead can reduce the efficiency of parallel programs. Optimizing communication patterns and reducing overhead is essential for performance.

II. Latency

- The time it takes for a message to travel from the sender to the receiver, typically measured in microseconds.
- High latency can delay the execution of dependent tasks. Minimizing latency is critical for improving the speed of data exchange.

III. Bandwidth

- The amount of data that can be communicated per second, typically measured in gigabytes per second (GB/s).
- Higher bandwidth allows more data to be transferred in less time, enhancing the overall performance of communication-intensive applications.

## Agglomeration

Agglomeration is the process of combining smaller tasks and data partitions into larger tasks. This technique aims to reduce communication overhead and improve overall efficiency by grouping tasks that frequently interact and ensuring the aggregated tasks fit within the processor’s memory.

### Goals

I. Improve Computational Granularity:

- By reducing the number of tasks, agglomeration increases the computation-to-communication ratio, leading to more efficient execution.
- Larger tasks mean fewer context switches and less communication overhead.

II. Minimize Communication:

- Localizing data and computation within agglomerated tasks decreases the need for frequent data exchanges between tasks.
- This reduction in communication can significantly enhance performance, especially in distributed computing environments.

### Granularity

Granularity in parallel computing refers to the ratio of computation to communication in a task or set of tasks. It indicates the amount of work done between communication events.

$$
\text{Granularity} = \frac{\text{computation}}{\text{communication}}
$$

- **Fine-grained parallelism** is characterized by a high frequency of communication compared to computation, which results in low granularity.
- **Coarse-grained parallelism**, on the other hand, is characterized by a low frequency of communication compared to computation, which results in high granularity.

### Types of Parallelism

I. Fine-Grained Parallelism

- Fine-grained parallelism involves a large number of small tasks.
- These tasks perform relatively small amounts of computation before needing to communicate with other tasks.
- One of the key advantages of fine-grained parallelism is its excellent load balancing due to the fine distribution of tasks across processors.
- Each processor is given an equal share of the workload, which helps prevent bottlenecks.
- However, fine-grained parallelism has notable disadvantages, including a low computation-to-communication ratio, which leads to high communication overhead.
- The frequent need for communication between tasks can cause delays and reduce the overall efficiency of the system.

II. Coarse-Grained Parallelism

- Coarse-grained parallelism involves a small number of large tasks.
- These tasks perform substantial amounts of computation with only infrequent communication.
- The primary advantage of coarse-grained parallelism is its high computation-to-communication ratio, which results in reduced communication overhead.
- As tasks spend more time on computation, the overall process becomes more efficient.
- Despite these advantages, coarse-grained parallelism also has its drawbacks, such as poor load balancing because the workload is divided into fewer, larger tasks.
- This can lead to situations where some processors are idle while others are overloaded, resulting in inefficiencies.

### Practical Considerations

I. Task Grouping

- Identify and group smaller tasks that frequently communicate to form larger tasks.
- Ensure that these agglomerated tasks fit well within the processor’s memory to avoid excessive swapping and paging.

II. Memory Management

- Agglomerated tasks should be designed to make efficient use of available memory.
- Proper memory management ensures that tasks run smoothly without causing memory bottlenecks.

III. Optimization Balance

- Striking a balance between fine-grained and coarse-grained parallelism is crucial.
- The ideal granularity depends on the specific application and the architecture of the computing system.

## Mapping

Mapping is the process of assigning agglomerated tasks to specific processors in a parallel computing environment. Effective mapping is crucial for balancing the computational load and minimizing communication overhead.

### Goals

I. Balance the Computational Load Across Processors

- Distribute tasks evenly to ensure that no single processor is overloaded while others are underutilized.
- Balanced load distribution maximizes the utilization of available computational resources.

II. Minimize Communication Overhead

- Assign tasks that frequently communicate with each other to the same processor or to nearby processors.
- Reducing the distance and frequency of data exchanges can significantly decrease communication delays and overhead.

III. Minimize the Total Execution Time

- Optimize the assignment of tasks to reduce the overall time required for computation and communication.
- Efficient mapping leads to faster completion of the entire workload.

### Strategies

I. Static Mapping

- Tasks are assigned to processors before execution begins and remain fixed throughout.
- Suitable for problems where the workload is predictable and uniform.

II. Dynamic Mapping

- Tasks are assigned to processors during execution based on current load and availability.
- Ideal for applications with unpredictable workloads or where tasks vary significantly in size.

III. Hierarchical Mapping

- Combines static and dynamic approaches by dividing tasks into groups that are statically assigned to clusters of processors.
- Within each cluster, tasks can be dynamically mapped to balance the load.

IV. Task Clustering

- Group tasks with high inter-task communication into clusters and assign each cluster to a single processor or a set of closely located processors.
- Reduces the communication overhead and improves overall efficiency.

V. Graph Partitioning

- Represent tasks and their communication as a graph, where nodes represent tasks and edges represent communication.
- Use graph partitioning algorithms to divide the graph into subgraphs with minimal edge cuts, ensuring tasks with high communication needs are grouped together.

### Practical Considerations

I. Task Placement

- Specify where each task will execute, considering factors like processor capabilities, memory hierarchy, and network topology.
- Placement decisions impact both computational performance and communication efficiency.

II. Load Balancing

- Continuously monitor and adjust the load distribution to prevent any processor from becoming a bottleneck.
- Dynamic load balancing techniques can help in maintaining an even distribution of work.

III. Communication Patterns

- Analyze the communication patterns of tasks to identify high-communication pairs or groups.
- Optimize mapping to localize these communication patterns as much as possible.

### Does Not Apply To

- In the context of **single-core processors**, mapping is irrelevant since all tasks are executed on the same processor.
- Systems equipped with **automated task scheduling** manage task assignment internally, rendering manual mapping unnecessary.

## Example Workflow in Designing Parallel Programs

I. Partitioning

- Analyze the problem to identify independent tasks.
- Choose between domain decomposition and functional decomposition based on the nature of the problem.
- Ensure the tasks are roughly equal in computational load.

II. Communication

- Determine the data dependencies between tasks.
- Plan the communication strategy (point-to-point or collective).
- Optimize the communication pattern to minimize overhead.

III. Agglomeration

- Combine smaller tasks to form larger ones, reducing the total number of tasks.
- Balance the trade-off between computational granularity and communication overhead.
- Ensure the combined tasks fit within the memory limits of a processor.

IV. Mapping

- Assign agglomerated tasks to processors.
- Use static mapping if the workload is predictable; otherwise, opt for dynamic mapping.
- Consider the network topology to minimize communication delays.

## Key Principles

- Effective **load balancing** aims to distribute work evenly across processors to avoid idle times.
- **Minimizing communication** is essential to reduce the amount and frequency of data exchange between tasks.
- **Scalability** is crucial, ensuring the system can handle increasing problem sizes and processor counts efficiently.
- Maintaining **locality** by keeping frequently communicating tasks close to each other helps reduce communication delays.

## Tools and Techniques

- The **Message Passing Interface** (MPI) is used for communication in distributed memory systems.
- **OpenMP** is employed for parallel programming in shared memory systems.
- **Profiling** tools, such as Intel VTune and GNU gprof, are utilized to analyze performance and identify bottlenecks.
