## Designing Parallel Programs

Designing parallel programs involves breaking down computational tasks into smaller, concurrent units to be executed simultaneously. This approach leverages the power of multiple processors to enhance performance and efficiency. Key steps in this process include partitioning, communication, agglomeration, and mapping.

### Partitioning

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

#### Goals

I. Balance the Workload Among Tasks

- Ensure that each task receives an approximately equal amount of work to avoid bottlenecks.
- This balance is crucial for maximizing the efficiency and utilization of computational resources.

II. Minimize Dependencies Between Tasks

- Reduce the communication and synchronization overhead between tasks.
- Independent tasks can be processed in parallel without frequent data exchange, improving overall performance.

#### Types of Decomposition

I. Domain Decomposition

Domain decomposition splits your **data space** into chunks that different workers handle in parallel. The trick is to choose chunks so each worker mostly uses its own data, only syncing at boundaries.

Example (concrete & step-by-step):

* Suppose you’re blurring a **4000×4000 image**. Cut it into 4 tiles of **2000×2000**: top-left, top-right, bottom-left, bottom-right.
* Each worker blurs its own tile. Because blur needs neighbor pixels, each tile also reads a tiny border from adjacent tiles (often called **halo/ghost cells**).
* After processing, you stitch the 4 blurred tiles back together.
  Why this works: most computations are **local** (each pixel depends on nearby pixels), so each tile can run independently and fast, with small, predictable communication at the edges.

(Another mental model: In a weather sim, split the map into regions; each core updates temperature/wind in its region, only exchanging **border values** with neighbors after each timestep.)

II. Functional Decomposition

Functional decomposition splits by **kind of work**, not by data. You build a pipeline of stages; each stage does one job well.

Example (web request pipeline):

* **Auth stage:** check the token/cookie; if invalid, stop.
* **Data stage:** query the database for the user’s posts.
* **Transform stage:** sort and format the posts.
* **Render stage:** turn the result into HTML or JSON.
  Each stage can scale independently (lots of data? scale the Data stage), and it’s easier to test (mock inputs/outputs per stage).

(Another quick one: for large matrix multiplication on a GPU pipeline: **Load → Multiply → Accumulate → Store**. Same idea—different steps, specialized workers.)

III. Cyclic Decomposition

Cyclic decomposition (group theory) takes a **permutation**—a rule that reorders items—and breaks it into **disjoint cycles**. A cycle tells you how items rotate among themselves; disjoint means cycles don’t share elements, so they’re independent “mini-loops.”

Where does the permutation come from?
Anywhere you reorder labels/indices. Examples:

* **Rotating seats** at a table: everyone moves to the next chair.
* **Shuffling an array** by a fixed pattern (e.g., “send item i to position p(i)”).
* **Renaming variables** in a compiler pass.

Concrete example:

You have items labeled $\{1,2,3,4\}$. A “rotate left among the first three, keep 4” rule sends

* $1 \mapsto 2$, $2 \mapsto 3$, $3 \mapsto 1$, and $4 \mapsto 4$.

In **two-line notation** that’s

$$
\sigma =
\begin{pmatrix}
1 & 2 & 3 & 4 \\
2 & 3 & 1 & 4
\end{pmatrix}.
$$

How to decompose into cycles (step-by-step):

1. Start with the smallest label not yet used: **1**.  Follow where it goes: $1 \to 2$, then $2 \to 3$, then $3 \to 1$. You’ve returned to 1, so close the cycle: **$(1\,2\,3)$**.
2. Next unused label: **4**. Follow it: $4 \to 4$. That’s a 1-cycle (a fixed point): **$(4)$**.

So the cyclic decomposition is:

$$
\sigma = (1\,2\,3)(4).
$$

What this tells you:

* Apply $\sigma$ once: $1\!\to\!2,\,2\!\to\!3,\,3\!\to\!1$; 4 stays put.
* Apply $\sigma$ three times: the 3-cycle returns everyone in that cycle to start (order 3); 4 still stays put.

Mini table to visualize the mapping:

|  input | 1 | 2 | 3 | 4 |
| -----: | - | - | - | - |
| output | 2 | 3 | 1 | 4 |

You can trace arrows $1\to2\to3\to1$ and $4\to4$ to “see” the cycles.

IV. Block Decomposition

Block decomposition slices a matrix into **submatrices (blocks)** so you operate on them as units. This clarifies structure (e.g., block-diagonal) and speeds things up (cache-friendly, reuse kernels).

Example (numbers + why it helps):

Let

$$
A=\begin{pmatrix}
1 & 2 & 9 & 8\\
0 & 3 & 7 & 6\\
4 & 5 & 0 & 0\\
4 & 5 & 0 & 0
\end{pmatrix}
=
\begin{pmatrix}
A_{11} & A_{12}\\
A_{21} & A_{22}
\end{pmatrix}
$$

$$
A_{11}=\begin{pmatrix}1&2\\0&3\end{pmatrix}, 
A_{12}=\begin{pmatrix}9&8\\7&6\end{pmatrix}, 
A_{21}=\begin{pmatrix}4&5\\4&5\end{pmatrix}, 
A_{22}=\begin{pmatrix}0&0\\0&0\end{pmatrix} 
$$

* **Block multiplication:** $C = A B$ can be done via
  $C_{11}=A_{11}B_{11}+A_{12}B_{21}$, $C_{12}=A_{11}B_{12}+A_{12}B_{22}$, etc. You reuse fast kernels on smaller chunks.
* **Solving block systems:** If $A_{11}$ is easy to invert (e.g., triangular) and $A_{22}$ is simple (here zeros), you can use **Schur complements** to solve $Ax=b$ in stages.
* **Structure spotting:** If $A_{12}$ and $A_{21}$ were zero, $A$ would be block-diagonal, meaning two independent smaller problems—great for parallelism.

Bottom line: with blocks, you trade one huge problem for a few tidy medium-sized ones, which are simpler to reason about and often faster to compute.

### Communication

Communication in parallel computing involves the exchange of data between tasks. This process is essential when tasks depend on each other's results to proceed. Efficient communication is crucial for maintaining performance and minimizing delays.

#### Goals

I. Minimize the Volume and Frequency of Communication

- Reducing the amount of data exchanged and the number of communications can significantly decrease overhead and improve overall performance.

II. Optimize the Use of Network Resources:

- Efficient use of network bandwidth and minimizing latency ensures faster data transfer and better utilization of computational resources.

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
+-------------+        +-------------+
| Processor 1 | <----> | Processor 2 |
| Task A      |        | Task B      |
+-------------+        +-------------+
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

#### Concepts in Communication

I. Overhead

- The computational time and resources spent on communication.
- High overhead can reduce the efficiency of parallel programs. Optimizing communication patterns and reducing overhead is essential for performance.

II. Latency

- The time it takes for a message to travel from the sender to the receiver, typically measured in microseconds.
- High latency can delay the execution of dependent tasks. Minimizing latency is critical for improving the speed of data exchange.

III. Bandwidth

- The amount of data that can be communicated per second, typically measured in gigabytes per second (GB/s).
- Higher bandwidth allows more data to be transferred in less time, enhancing the overall performance of communication-intensive applications.

### Agglomeration

Agglomeration is the process of combining smaller tasks and data partitions into larger tasks. This technique aims to reduce communication overhead and improve overall efficiency by grouping tasks that frequently interact and ensuring the aggregated tasks fit within the processor’s memory.

#### Goals

I. Improve Computational Granularity:

- By reducing the number of tasks, agglomeration increases the computation-to-communication ratio, leading to more efficient execution.
- Larger tasks mean fewer context switches and less communication overhead.

II. Minimize Communication:

- Localizing data and computation within agglomerated tasks decreases the need for frequent data exchanges between tasks.
- This reduction in communication can significantly enhance performance, especially in distributed computing environments.

#### Granularity

Granularity in parallel computing refers to the ratio of computation to communication in a task or set of tasks. It indicates the amount of work done between communication events.

$$
\text{Granularity} = \frac{\text{computation}}{\text{communication}}
$$

- **Fine-grained parallelism** is characterized by a high frequency of communication compared to computation, which results in low granularity.
- **Coarse-grained parallelism**, on the other hand, is characterized by a low frequency of communication compared to computation, which results in high granularity.

#### Types of Parallelism

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

#### Practical Considerations

I. Task Grouping

- Identify and group smaller tasks that frequently communicate to form larger tasks.
- Ensure that these agglomerated tasks fit well within the processor’s memory to avoid excessive swapping and paging.

II. Memory Management

- Agglomerated tasks should be designed to make efficient use of available memory.
- Proper memory management ensures that tasks run smoothly without causing memory bottlenecks.

III. Optimization Balance

- Striking a balance between fine-grained and coarse-grained parallelism is crucial.
- The ideal granularity depends on the specific application and the architecture of the computing system.

### Mapping

Mapping is the process of assigning agglomerated tasks to specific processors in a parallel computing environment. Effective mapping is crucial for balancing the computational load and minimizing communication overhead.

#### Goals

I. Balance the Computational Load Across Processors

- Distribute tasks evenly to ensure that no single processor is overloaded while others are underutilized.
- Balanced load distribution maximizes the utilization of available computational resources.

II. Minimize Communication Overhead

- Assign tasks that frequently communicate with each other to the same processor or to nearby processors.
- Reducing the distance and frequency of data exchanges can significantly decrease communication delays and overhead.

III. Minimize the Total Execution Time

- Optimize the assignment of tasks to reduce the overall time required for computation and communication.
- Efficient mapping leads to faster completion of the entire workload.

#### Strategies

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

### Example Workflow

2-D Heat Diffusion (4096×4096, 5,000 steps, 5-point stencil, FP64)

**What we’re doing**

Simulate heat diffusion on a 4096×4096 grid for 5,000 steps using a 5-point stencil in double precision. Two row-major arrays (`A[nx][ny]`, `B[nx][ny]`) “ping-pong” each step. We checkpoint to HDF5 per tile with datasets named `temp_step_<t>`.

**Why these tools**

* MPI combined with OpenMP provides a simple and portable form of hybrid parallelism, where MPI handles halo exchanges across ranks and OpenMP manages threads within a rank.
* Parallel HDF5 enables scalable checkpoints and efficient final field output.
* InfiniBand supports low-latency communication for halo exchanges.

**What “done” looks like**

* Balanced tasks with explicit halo exchanges.
* Fewer MPI ranks via agglomeration; threads handle intra-rank work.
* Topology-aware mapping and pinning (low cross-node/NUMA traffic).
* Output: converged temperature field + periodic checkpoints.

#### I. Partitioning (make work even and predictable)

**Goal:** split the domain into equal, independent tasks.

**How:** uniform 8×8 domain decomposition → 64 blocks.

* A block size of 512×512 cells results in 262,144 updates per step per block, providing predictable work and good cache behavior.
* Task IDs are assigned as T(0,0) through T(7,7) in row-major order, which makes mapping straightforward and debugging easier; for example, “T(3,5)” can be used to pinpoint issues.
* The load balance is uniform to within about ±1 percent, so no special weighting is required on homogeneous CPUs.

```
+---------------------------------------------------------------+
|                        4096 x 4096 Grid                       |
+---------------------------------------------------------------+
|   Split into 8 x 8 = 64 blocks                                |
|   Each block = 512 x 512 cells                                |
+---------------------------------------------------------------+

 Example partition view (8x8 blocks):

 +----+----+----+----+----+----+----+----+
 | T0 | T1 | T2 | T3 | T4 | T5 | T6 | T7 |
 +----+----+----+----+----+----+----+----+
 | T8 | T9 |T10 |T11 |T12 |T13 |T14 |T15 |
 +----+----+----+----+----+----+----+----+
 |T16 |T17 |T18 |T19 |T20 |T21 |T22 |T23 |
 +----+----+----+----+----+----+----+----+
 | ... repeats until T63 ...             |
 +---------------------------------------+

Each block has predictable work, ID, and size (512x512).
```

#### II. Communication (keep it correct and overlapped)

**Goal:** define and overlap all required data exchanges.

**How:** nonblocking point-to-point halos; rare collectives.

* Each step requires a one-cell halo exchange with the north, south, east, and west neighbors, while the corners are optional.
* For a 512×512 block, the per-edge message size is 512 × 1 × 8 bytes, which equals 4 KB, leading to about 16 KB per block per step when all four edges are included.
* The overlap pattern consists of four stages: first compute the interior, then issue nonblocking receives and sends to the north, south, east, and west neighbors using one contiguous buffer per edge, then perform the test or wait operations, and finally compute the borders.
* A global synchronization is performed by running an Allreduce on the residual every 100 steps using a single double, which keeps synchronization costs low.


```
 Example: One block (512x512)

      North halo
        vvvvv
   +---------------------+
   | NNNNNNNNNNNNNNNNNNN |
   |                     |
   |   Interior work     |<---> East halo
   |                     |
   | SSSSSSSSSSSSSSSSSSS |
   +---------------------+
        ^^^^^
      South halo

Legend:
- N = North halo (1 row)
- S = South halo (1 row)
- W/E = West/East halo (1 column each)

Each step:
- Send/recv 4 edges (north, south, east, west).
- Overlap: compute interior first, halos later.
- Message size per edge = 512 × 1 × 8 B = 4 KB.
```

#### III. Agglomeration (cut rank count, keep memory safe)

**Goal:** reduce messages and ranks without exceeding per-rank memory.

**How:** merge 2×2 blocks into a tile; thread inside each rank.

* The cluster consists of four hosts, each with two sockets and 16 cores per socket, giving 32 cores per host, along with 128 GB of RAM and a 100 Gbps InfiniBand interconnect.
* In the earlier configuration, there were 64 MPI ranks, each responsible for a 512×512 block, which resulted in many small halo messages.
* In the new configuration, there are 16 MPI ranks, each owning a 1024×1024 tile arranged as a 2×2 block, with OpenMP threads used inside each rank; this approach produces fewer but larger messages.
* For a 1024×1024 tile, the per-edge message size is 1024 × 1 × 8 bytes, which equals 8 KB, leading to about 32 KB per rank per step when all four edges are included.
* The memory usage per rank is approximately 8 MB for array A, 8 MB for array B, and 1–2 MB for halos and scratch space, totaling about 17–20 MB, which is far below the 512 MB budget.

```
 Before: 64 MPI ranks (each 512x512)
 After:  16 MPI ranks (each 1024x1024 = 2x2 tile)

 Illustration (agglomeration from 2x2 to 1):

 +----+----+      +--------+
 | A  | B  |  =>  |        |
 +----+----+      |   R    |   (One rank handles all 4 blocks)
 | C  | D  |      |        |
 +----+----+      +--------+

Effect:
- Fewer ranks (64 -> 16)
- Larger messages (8 KB per edge instead of 4 KB)
- Threads (OpenMP) handle work inside each bigger block
- Memory per rank ~20 MB (safe under 512 MB limit)
```

#### IV. Mapping (place work where data moves least)

**Goal:** minimize cross-node traffic and cross-NUMA access.

**How:** Cartesian rank layout + core pinning.

* The configuration uses 16 MPI ranks with 8 OpenMP threads each, giving a total of 128 threads, which matches the 4 nodes with 32 cores each.
* The MPI ranks are placed evenly across the nodes: ranks 0 through 3 on node-a, ranks 4 through 7 on node-b, ranks 8 through 11 on node-c, and ranks 12 through 15 on node-d.
* The ranks are arranged in a 4×4 Cartesian layout in row-major order, so that neighbors align with tiles; for example, the east neighbor of rank 5 is rank 6.
* Each socket hosts two ranks, and threads are pinned within the socket using `OMP_PLACES=cores` and `OMP_PROC_BIND=close` to avoid NUMA penalties.
* A topology hint is to co-locate tile rows on the same node so that east–west halo exchanges remain intra-node, while only north–south exchanges cross nodes.

```
 Cluster: 4 nodes (32 cores each)
 Layout: 16 MPI ranks × 8 threads = 128 threads

 Example placement:

   Node A (ranks 0-3)     Node B (ranks 4-7)
   +-----+-----+          +-----+-----+
   | R0  | R1  |          | R4  | R5  |
   +-----+-----+          +-----+-----+
   | R2  | R3  |          | R6  | R7  |
   +-----+-----+          +-----+-----+

   Node C (ranks 8-11)    Node D (ranks 12-15)
   +-----+-----+          +-----+-----+
   | R8  | R9  |          |R12  |R13  |
   +-----+-----+          +-----+-----+
   |R10  |R11  |          |R14  |R15  |
   +-----+-----+          +-----+-----+

Inside each node:
- 2 ranks per socket
- 8 threads per rank pinned to cores
- Cartesian layout keeps neighbor IDs aligned with space
- East-West halos mostly intra-node, only North-South cross nodes
```

#### Runbook

**Code:** `heat2d.c` (ping-pong buffers, pack/unpack halos, 5-point update)

**Build & quick check**

```bash
mpicc -O3 -march=native -fopenmp heat2d.c -o heat2d
./heat2d --nx 512 --ny 512 --steps 10 --tile 512 --verify analytic
```

**Distribute across servers (rank layout)**

```bash
mpirun -n 16 --map-by ppr:4:node --bind-to core --report-bindings \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024
```

**Full run with threads + checkpoints**

```bash
OMP_NUM_THREADS=8 OMP_PLACES=cores OMP_PROC_BIND=close \
mpirun -n 16 --map-by ppr:4:node \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024 --checkpoint 200
```

Outputs & logging:

* Checkpoints are written as files named `chkpt_t<step>_tile<r>.h5`, with one file generated per tile.
* The final field is stored in a single file named `final_temp.h5`.
* Inside the files, the datasets are named `temp_step_<t>`.
* Logs record the residual at each step using an Allreduce every 100 steps, along with timings for computation, halo exchanges, and I/O.

### Principles

* When *load balancing* is effective, processors complete their assigned work at similar times and avoid idle periods, whereas poor distribution leaves some processors waiting; for example, evenly divided tiles in a 2-D grid let all ranks progress together.
* By *minimizing communication*, tasks exchange less frequent or smaller data messages, which lowers overhead, whereas excessive communication can dominate runtime; for example, batching halo data into a single buffer reduces the number of network transfers.
* Achieving *scalability* ensures that adding more processors or increasing the problem size continues to yield useful performance gains, whereas poor scaling leads to diminishing returns; for example, a stencil solver that maintains efficiency on 128 cores can often extend to 512 cores with predictable throughput.
* Preserving *locality* keeps tasks that frequently exchange halos placed near each other in memory or on the same node, which shortens communication paths, whereas ignoring locality increases latency; for example, mapping tile rows to the same host keeps east–west messages intra-node.

### Tools and Techniques

* Using the *Message Passing Interface* (MPI) enables communication across distributed memory systems, whereas omitting it prevents processes on separate nodes from exchanging data; for example, halo values in a multi-node stencil computation require MPI sends and receives.
* Applying *OpenMP* allows shared memory parallelism within a node, whereas ignoring it forces the program to rely solely on MPI ranks even for intra-node parallelism; for example, multiple threads per rank can process tiles more efficiently than many small MPI ranks on the same socket.
* Employing *profiling* tools such as Intel VTune or GNU gprof helps identify runtime hotspots and communication bottlenecks, whereas not profiling leaves performance issues hidden; for example, profiling may reveal that boundary updates consume more time than interior computation.
