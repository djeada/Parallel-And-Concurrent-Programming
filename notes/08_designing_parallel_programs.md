## Designing Parallel Programs

Designing a parallel program means restructuring a computation so that useful work can proceed concurrently without making coordination costs dominate. A good design does more than create many tasks: it exposes enough parallelism, keeps processors busy, limits communication and synchronization, preserves data locality, and maps work onto hardware in a way that reduces waiting.

A useful design framework is **PCAM**, which has four steps.

1. **Partitioning** — split the computation and data into tasks.
2. **Communication** — identify what data tasks must exchange.
3. **Agglomeration** — combine small tasks into larger units when that improves efficiency.
4. **Mapping** — assign tasks to processors, cores, nodes, or accelerators.

PCAM is best treated as an **iterative design process**, not a one-way checklist. A mapping decision may reveal that communication is too expensive; agglomeration may then need to change, which can in turn require a different partitioning. The final design should be validated with measurements rather than assumed to be optimal from the decomposition alone.

Three questions are useful throughout the process:

- **Where is the parallelism?** Identify independent work and the critical dependencies that limit concurrency.
- **What does parallelism cost?** Account for communication, synchronization, scheduling, memory traffic, and I/O.
- **Where should the work run?** Match task size, locality, and communication patterns to the actual hardware topology.

```
+-----------------------------+
|         Big Problem         |
+--------------+--------------+
               |
               v
+-----------------------------+
| 1. Partitioning             |
| Break the problem into      |
| smaller parallel tasks      |
+--------------+--------------+
               |
               v
+-----------------------------+
| 2. Communication            |
| Identify what data must be  |
| exchanged between tasks     |
+--------------+--------------+
               |
               v
+-----------------------------+
| 3. Agglomeration            |
| Combine tasks where useful  |
| to reduce overhead          |
+--------------+--------------+
               |
               v
+-----------------------------+
| 4. Mapping                  |
| Assign tasks to processors  |
| or hardware resources       |
+-----------------------------+
```

### Partitioning

Partitioning divides the problem into smaller pieces that can run concurrently. The goal is to expose enough parallel work while keeping dependencies manageable. A useful partition should create more potential tasks than there are processors, but each task must still be large enough that scheduling and communication do not overwhelm its computation.

For irregular problems, it is often helpful to view the computation as a **dependency graph**. Nodes represent units of work, while edges represent data or ordering dependencies. The longest dependency chain forms the **critical path**, which limits the minimum possible execution time even if unlimited processors were available.

```
+-----------------------+
|      Big Problem      |
+-----------+-----------+
            |
     +--- --+------------+
     |         |         |
+----+----+ +--+--+ +----+----+
| Part 1  | |Part2| | Part 3  |
+---------+ +-----+ +---------+
```

#### Goals

**I. Balance the workload among tasks**

- Give each task roughly the same amount of work.
- Avoid situations where some processors finish early and sit idle while others continue working.
- Consider both the number of operations and the cost of each operation; equal data sizes do not always mean equal work.

**II. Minimize dependencies between tasks**

- Reduce the amount of communication and synchronization needed between tasks.
- Prefer partitions where most computation uses local data.
- Keep dependent data close together when possible.

**III. Expose enough concurrency**

- Create enough tasks to keep all available processors busy and to leave some flexibility for load balancing.
- Avoid creating so many tiny tasks that scheduling and communication overhead dominate the computation.
- Remember that available parallelism can change during execution; phases near the beginning or end of an algorithm may expose fewer runnable tasks.

### Types of Decomposition

#### I. Domain Decomposition

Domain decomposition splits the **data space** into chunks. Each worker receives a portion of the data and performs the same or similar computation on that portion.

This works well when computation is mostly local, such as image processing, simulations, matrix operations, and grid-based numerical methods. The shape of each partition matters as much as its size: compact subdomains usually reduce boundary communication because they have a smaller surface-to-volume ratio than long, thin partitions.

Consider blurring a 4000 × 4000 image.

- Split the image into four 2000 × 2000 tiles: top-left, top-right, bottom-left, and bottom-right.
- Each worker blurs its own tile.
- Because blur operations need neighboring pixels, each tile also reads a small border from adjacent tiles. These border values are often called **halo cells** or **ghost cells**.
- After processing, the tiles are stitched back together.

```
+-----------------------+-----------------------+
|       Tile 1          |        Tile 2         |
|     Worker 1          |      Worker 2         |
+-----------------------+-----------------------+
|       Tile 3          |        Tile 4         |
|     Worker 3          |      Worker 4         |
+-----------------------+-----------------------+
```

This decomposition works well because most pixels depend only on nearby pixels, so almost all computation stays inside each tile. Only the tile boundaries need data from neighboring partitions. For a two-dimensional square tile with side length \(L\), computation grows roughly with \(L^2\), while boundary data grows roughly with \(L\). Larger tiles therefore do more computation for each boundary element exchanged, although making tiles too large eventually reduces the amount of parallel work available.

#### II. Functional Decomposition

Functional decomposition splits the work by **type of operation** rather than by data region. It is often used to build pipelines.

A web request pipeline is a simple example.

- First, verify the token or cookie.
- Then query the database.
- Next, sort, filter, or format the result.
- Finally, generate HTML or JSON.

```
Request
   |
   v
+--------+     +-------+     +-----------+     +--------+
| Auth   | --> | Data  | --> | Transform | --> | Render |
+--------+     +-------+     +-----------+     +--------+
```

Each stage can be replicated or assigned a different number of workers. For example, if database access becomes the bottleneck, more workers can be allocated to the Data stage—provided the database itself can sustain the additional concurrency. Pipeline throughput is ultimately limited by the slowest stage, so queue lengths and service rates should be measured rather than assuming that adding workers will always help.

Functional decomposition is useful when:

- Different stages require different resources.
- The pipeline processes many independent inputs.
- Each stage can be tested and optimized separately.

#### III. Cyclic Decomposition

In parallel programming, cyclic decomposition usually means distributing tasks or loop iterations in a **round-robin** pattern across workers.

Instead of giving each worker one large contiguous block, cyclic decomposition assigns item `i` to worker `i mod P`, where `P` is the number of workers.

For example, 12 loop iterations can be distributed across 4 workers like this.

| Iteration | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|----------:|---|---|---|---|---|---|---|---|---|---|----|----|
| Worker    | 0 | 1 | 2 | 3 | 0 | 1 | 2 | 3 | 0 | 1 | 2  | 3  |

```
Worker 0: iterations 0, 4, 8
Worker 1: iterations 1, 5, 9
Worker 2: iterations 2, 6, 10
Worker 3: iterations 3, 7, 11
```

Cyclic decomposition is especially useful when the work is uneven or difficult to predict.

- Work per iteration is uneven or hard to predict.
- Expensive iterations are scattered throughout the input.
- A simple block split would overload one worker.

The main tradeoff is locality. Cyclic decomposition often improves load balance, but each worker may touch data spread across the whole domain. A **block-cyclic** distribution is a common compromise because nearby items stay together inside each block while the blocks themselves are distributed round-robin to smooth uneven work.

#### IV. Block Decomposition

Block decomposition divides arrays or matrices into contiguous blocks. Each worker receives one or more blocks and operates on them as units.

This is common in matrix algorithms, stencil computations, and cache-aware numerical methods.

Let

$$
A = \begin{pmatrix}
1 & 2 & 9 & 8 \\
0 & 3 & 7 & 6 \\
4 & 5 & 0 & 0 \\
4 & 5 & 0 & 0
\end{pmatrix} =
\begin{pmatrix}
A_{11} & A_{12} \\
A_{21} & A_{22}
\end{pmatrix}
$$

where

$$
A_{11}=\begin{pmatrix}1 & 2 \\ 0 & 3\end{pmatrix}, \quad
A_{12}=\begin{pmatrix}9 & 8 \\ 7 & 6\end{pmatrix}, \quad
A_{21}=\begin{pmatrix}4 & 5 \\ 4 & 5\end{pmatrix}, \quad
A_{22}=\begin{pmatrix}0 & 0 \\ 0 & 0\end{pmatrix}.
$$

Block decomposition helps in several ways.

- In block matrix multiplication, if `C = A B`, then $C_{11}=A_{11}B_{11}+A_{12}B_{21}$ and similarly for the other blocks. This lets the program reuse optimized kernels on smaller pieces.
- Contiguous blocks often have better cache locality than accesses spread across an entire matrix.
- Independent blocks can be processed by different workers.
- Block-diagonal structure can turn one large problem into several smaller independent ones.

A closely related strategy is block-cyclic decomposition.

Block-cyclic decomposition combines block and cyclic ideas. It assigns blocks round-robin across workers, which helps balance load while still preserving some locality. Distributed dense-linear-algebra libraries commonly use two-dimensional block-cyclic layouts because they spread expensive matrix regions across many processes without reducing every operation to element-level cyclic access.

### Communication

Communication is the exchange of data between tasks. It is necessary when one task needs another task’s results. The best parallel designs minimize communication without sacrificing correctness and distinguish **data movement** from **synchronization**. Two tasks may need to wait for one another even when very little data is exchanged.

A simple communication cost model is:

$$
T_{\text{message}} \approx \alpha + \frac{n}{B}
$$

where $\alpha$ is startup latency, $n$ is the number of bytes, and $B$ is the effective bandwidth. This explains why many tiny messages can be expensive even when their total byte count is small, while large messages are often dominated by bandwidth.

#### Goals

**I. Minimize communication volume and frequency**

- Send less data when possible.
- Combine small messages into fewer larger messages.
- Avoid unnecessary synchronization.

**II. Preserve locality**

- Keep frequently interacting tasks on the same processor, socket, node, or nearby nodes.
- Prefer local memory access over remote communication.

**III. Overlap communication with computation**

- Start communication early.
- Perform independent computation while messages are in transit.
- Use nonblocking operations when supported by the programming model.

### Types of Communication

#### I. Local Communication

Local communication occurs between tasks on the same processor, socket, or node. Depending on the programming model, it may use shared memory, cache-coherent loads and stores, queues, or operating-system IPC. It is usually lower-latency than communication across a network, but it is not free: cache-coherence traffic, NUMA placement, memory bandwidth, and false sharing can all become bottlenecks.

For example, two cores on the same CPU socket can communicate through cache-coherent shared memory.

```
+-----------+     +-----------+
| Core 1    | <-> | Core 2    |
| Task A    |     | Task B    |
+-----------+     +-----------+
       \             /
        \           /
       +-------------+
       | Shared Mem  |
       +-------------+
```

#### II. Global Communication

Here, **global communication** refers to communication that crosses a node boundary. It usually travels over a network and is more expensive than same-node communication. In other contexts, the word *global* may instead mean that many or all participants are involved, as in a global reduction, so the intended meaning should be clear.

For example, MPI ranks running on separate servers may exchange boundary data over the network.

```
+-------------+        Network        +-------------+
| Node 1      | <-------------------> | Node 2      |
| Task A      |                       | Task B      |
+-------------+                       +-------------+
```

#### III. Point-to-Point Communication

Point-to-point communication sends data directly from one task to another.

```
+---------+     message      +----------+
| Sender  | -------------->  | Receiver |
+---------+                  +----------+
```

Common examples include:

- MPI `Send` / `Recv`
- MPI `Isend` / `Irecv`
- Producer-consumer queues
- Direct socket messages

#### IV. Collective Communication

Collective communication involves a group of tasks that participate in the same communication operation. Common collectives include broadcast, scatter, gather, reduce, and all-reduce. Efficient libraries usually implement these operations with trees, rings, recursive-doubling schemes, or topology-aware algorithms rather than routing every message through one process.

With a scatter operation, one task sends different pieces of data to different tasks.

```
Scatter:
       +-----------------------+
       |          ABC          |
       +-----------+-----------+
                   |
      +------------+------------+
      |            |            |
  +---v---+    +---v---+    +---v---+
  |   A   |    |   B   |    |   C   |
  +-------+    +-------+    +-------+
```

With a broadcast, one task sends the same data to every task in the group.

```
Broadcast:
       +-----------------------+
       |          ABC          |
       +-----------+-----------+
                   |
      +------------+------------+
      |            |            |
  +---v---+    +---v---+    +---v---+
  |  ABC  |    |  ABC  |    |  ABC  |
  +-------+    +-------+    +-------+
```

A reduction combines values contributed by many tasks into one result.

```
Worker values:  4, 7, 2, 9
Operation:      sum
Result:         22
```

An all-reduce performs the same reduction but returns the result to every task. This is common in simulations and distributed machine learning.

### Communication Modes

#### I. Blocking Communication

Blocking communication waits until the operation is complete before the task continues.

- Easier to reason about.
- Safer for simple programs.
- Can cause idle time if a task waits while it could have been doing useful work.

#### II. Nonblocking Communication

Nonblocking communication starts an operation and returns before the transfer is complete.

- It can enable overlap between communication and computation when independent work exists and the runtime or communication library makes progress on the transfer.
- It requires careful handling to ensure send buffers are not modified and receive buffers are not consumed before completion.
- It usually requires a later wait or test operation.
- Nonblocking calls do not guarantee overlap by themselves; actual progress depends on the implementation, transport, and how often the program re-enters the communication library.

A common pattern looks like this:

1. Post nonblocking receives.
2. Send boundary data.
3. Compute the interior region while messages travel.
4. Wait for received halo data.
5. Compute the boundary region.

### Concepts in Communication

#### I. Overhead

Overhead is work introduced by parallel execution that would not be needed in the equivalent serial computation. It includes message setup, synchronization, copying, packing and unpacking, scheduling, thread/process management, redundant work, and time lost to load imbalance.

#### II. Latency

Latency is the fixed startup delay associated with initiating and delivering communication. For small messages, this fixed cost often dominates transfer time, which is why algorithms with many tiny messages may scale poorly even when the total number of bytes is modest.

#### III. Bandwidth

Bandwidth is the sustained amount of data that can be transferred per second. It matters most when messages are large enough that transfer time dominates startup latency. Effective application bandwidth may be lower than link peak bandwidth because of protocol overhead, contention, topology, and memory-copy costs.

#### IV. Contention

Contention occurs when multiple tasks compete for the same finite resource, such as a memory controller, cache line, network link, file system, lock, or synchronization point. Contention can make communication costs nonlinear: doubling the number of communicating tasks does not necessarily preserve per-task bandwidth.

### Agglomeration

Agglomeration combines smaller logical tasks into larger execution units after the initial partitioning step. The purpose is to reduce overhead, improve locality, and create tasks that better match the target hardware.

Agglomeration is a tradeoff. Larger tasks usually communicate less frequently and reuse data better, but they also reduce the number of independently schedulable units. The design should therefore preserve enough tasks to balance load while making each task large enough to amortize runtime and communication costs.

#### Goals

**I. Improve computational granularity**

- Larger tasks perform more computation between communication events.
- This increases the computation-to-communication ratio.
- It reduces scheduling overhead and message overhead.

**II. Minimize communication**

- Merge tasks that frequently communicate.
- Keep related data within the same process, thread group, node, or GPU.
- Reduce the number of boundary exchanges.

**III. Fit the memory hierarchy**

- Agglomerated tasks should fit available memory.
- Ideally, inner working sets should fit cache or high-bandwidth memory when possible.

### Granularity

Granularity describes the amount of useful computation performed by each task relative to coordination costs. A common informal measure is the computation-to-communication ratio:

$$
\text{Granularity} \propto \frac{\text{Computation}}{\text{Communication + synchronization + scheduling overhead}}
$$

This is not a universal unit of measurement, but it is a useful design intuition: the useful work inside a task should be large enough to amortize the cost of managing that task.

#### Fine-Grained Parallelism

Fine-grained parallelism uses many small tasks.

This style exposes a lot of parallel work and can balance irregular workloads well, especially when the runtime can schedule tasks dynamically. The cost is that scheduling, synchronization, and communication happen more often, so overhead can become a large fraction of total runtime.

#### Coarse-Grained Parallelism

Coarse-grained parallelism uses fewer, larger tasks.

Larger tasks usually improve locality and reduce scheduling and communication overhead. The downside is less flexibility: if a few tasks take much longer than the others, some processors may finish early and sit idle.

#### Practical Balance

The best granularity depends on the problem, hardware, and runtime system. A good design usually starts with many logical tasks, then agglomerates them until communication and scheduling overhead are acceptable.

A practical way to tune granularity is to measure **useful work per task** against **task overhead**. If a task takes only a few microseconds but scheduling it also costs microseconds, it is too fine-grained for that runtime. Conversely, tasks that run for seconds may be too coarse if a few slow tasks leave most processors idle near the end of a phase.

### Mapping

Mapping assigns agglomerated tasks to hardware resources such as cores, sockets, nodes, or accelerators. Good mapping improves load balance and reduces communication distance. On hierarchical machines, *where* a task runs affects not only network traffic but also cache sharing, NUMA locality, accelerator access, and memory bandwidth.

Mapping also interacts with the operating system and runtime. Thread affinity, process pinning, GPU selection, and first-touch memory placement can materially change performance even when the algorithm itself is unchanged.

#### Goals

**I. Balance computational load across processors**

- Keep all processors busy.
- Avoid placing too much work on one node or socket.
- Account for heterogeneous hardware if processors differ in speed.

**II. Minimize communication overhead**

- Place frequently communicating tasks close together.
- Keep memory accesses local when possible.
- Avoid unnecessary cross-node or cross-socket traffic.

**III. Minimize total execution time**

- Optimize both computation and communication.
- Consider I/O, memory bandwidth, synchronization, and network topology.

### Mapping Strategies

#### I. Static Mapping

Tasks are assigned before execution and remain fixed.

Static mapping works best when the workload is predictable and task sizes are fairly uniform.

#### II. Dynamic Mapping

Tasks are assigned during execution based on current load, often through work queues, work stealing, or runtime scheduling.

Dynamic mapping is a better fit when task costs are irregular or difficult to predict in advance.

Dynamic mapping improves balance but adds scheduling overhead and may reduce locality because work can migrate away from the data it uses. The runtime must therefore balance responsiveness against data placement.

#### III. Hierarchical Mapping

Tasks are mapped in levels: for example, first to nodes, then sockets or NUMA domains, then cores or accelerators.

Hierarchical mapping is especially useful on multi-node systems with NUMA domains, GPUs, or a complex network topology.

Hierarchical mapping mirrors the machine: expensive communication should cross the fewest high-cost boundaries possible. For example, tightly coupled threads may share a NUMA domain, while only coarse-grained halo exchanges cross nodes.

#### IV. Task Clustering

Tasks that communicate frequently are grouped and placed near each other.

Task clustering is useful for graph algorithms, stencil computations, and other workloads with known communication patterns.

#### V. Graph Partitioning

Represent tasks as graph nodes and communication as graph edges. Then partition the graph so that heavily connected tasks stay together and edge cuts are minimized.

Graph partitioning is particularly useful for irregular meshes, sparse matrix computations, and graph analytics.

## Example Workflow

### 2-D Heat Diffusion

The worked example simulates heat diffusion on a 4096 × 4096 grid for 5,000 time steps using a 5-point stencil in double precision.

The program uses two row-major arrays, `A[nx][ny]` and `B[nx][ny]`, which alternate roles each step. This is often called a **ping-pong buffer** pattern.

A typical Jacobi-style update derived from a 5-point stencil is:

$$
B[i,j] = 0.25 \times (A[i-1,j] + A[i+1,j] + A[i,j-1] + A[i,j+1])
$$

The underlying 5-point stencil consists of the center cell plus its four axial neighbors. In this simple averaging update, the new center value is computed from the four neighbors, so the old center value does not appear explicitly in the formula.

Periodic checkpoints are written to HDF5 files.

### What We Are Designing

- A parallel 2-D heat diffusion solver.
- A domain decomposition that divides the grid into tiles.
- Halo exchanges between neighboring tiles.
- Hybrid parallelism using MPI between ranks and OpenMP within each rank.
- Checkpoint output for long-running simulations.
- A design whose computation, communication, synchronization, and I/O costs can be measured separately.

### Why These Tools

- **MPI** handles communication between distributed-memory processes.
- **OpenMP** uses multiple cores inside each node or socket.
- **Parallel HDF5** supports scalable checkpoint and output files.
- **InfiniBand** or another low-latency network supports frequent halo exchanges.

### What “Done” Looks Like

- Work is evenly divided across ranks.
- Halo exchanges are explicit and correct.
- Communication is overlapped with interior computation.
- Agglomeration reduces the number of MPI ranks without exceeding memory limits.
- Mapping keeps neighboring tiles close in the hardware topology.
- Output includes the final temperature field and periodic checkpoints.

### I. Partitioning: Make Work Even and Predictable

We start by splitting the domain into equal-sized tasks while keeping dependencies local. An 8 × 8 decomposition gives 64 blocks.

- The full grid is 4096 × 4096.
- Each block is 512 × 512 cells.
- Each block performs 512 × 512 = 262,144 cell updates per step.
- Blocks are named `T(0,0)` through `T(7,7)` or numbered `T0` through `T63` in row-major order.
- Because every block has the same size and performs the same stencil, computational load is nearly uniform on homogeneous CPUs.
- Equal block sizes do not guarantee perfect balance once communication, operating-system noise, NUMA placement, or checkpoint I/O are included, so per-rank timings should still be measured.

```
+---------------------------------------------------------------+
|                       4096 x 4096 Grid                        |
+---------------------------------------------------------------+
| Split into 8 x 8 = 64 blocks                                  |
| Each block = 512 x 512 cells                                  |
+---------------------------------------------------------------+

Example partition view:

+----+----+----+----+----+----+----+----+
| T0 | T1 | T2 | T3 | T4 | T5 | T6 | T7 |
+----+----+----+----+----+----+----+----+
| T8 | T9 |T10 |T11 |T12 |T13 |T14 |T15 |
+----+----+----+----+----+----+----+----+
|T16 |T17 |T18 |T19 |T20 |T21 |T22 |T23 |
+----+----+----+----+----+----+----+----+
| ... continues until T63 ...           |
+---------------------------------------+
```

### II. Communication: Keep It Correct and Overlapped

Next, the ranks exchange only the boundary data needed for each stencil step. Nonblocking point-to-point communication handles the halo traffic, while occasional collectives handle global quantities such as the residual.

For each time step:

1. Each tile exchanges one row or column with its north, south, east, and west neighbors.
2. A 512 × 512 block sends one row of 512 double-precision values to the north and south.
3. It also sends one column of 512 double-precision values to the east and west.
4. Each edge message is approximately:

$$
512 \times 1 \times 8\text{ bytes} = 4096\text{ bytes} = 4\text{ KB}
$$

5. With four edges, each block **sends** about 16 KB per step and receives roughly the same amount, for about 32 KB of one-way application payload counted in both directions.
6. Every 100 steps, an `Allreduce` computes a global residual using one double value.

```
Example: one 512 x 512 block

          North halo
       vvvvvvvvvvvvvvv
    +---------------------+
    | N N N N N N N N N N |
    |                     |
 W  |   Interior work     |  E
 e  |                     |  a
 s  |                     |  s
 t  | S S S S S S S S S S |  t
    +---------------------+
       ^^^^^^^^^^^^^^^
          South halo

Each step:
- Exchange north, south, east, and west halos.
- Compute interior cells while halo messages are in flight.
- Compute border cells after halo data arrives.
```

A practical overlap sequence is:

```text
1. Post Irecv for north/south/east/west halos.
2. Pack and Isend local boundary rows/columns.
3. Compute the interior region that does not need new halos.
4. Wait/Test for halo receives to complete.
5. Unpack halos and compute border cells.
6. Wait for sends to complete before reusing send buffers.
```

Rows are contiguous in row-major storage, but columns are strided. Columns can therefore be packed into contiguous buffers or described with an MPI derived datatype. Packing adds CPU and memory-copy work; derived datatypes reduce explicit code but still need to be benchmarked because implementation quality varies.

The interior region must be large enough to hide a useful fraction of communication time. If halo transfer completes only after all interior work is already finished, nonblocking communication improves structure but not runtime.

### III. Agglomeration: Reduce Rank Count but Keep Memory Safe

The next step is to reduce MPI rank count and communication overhead without creating tiles that are too large for memory. Here, four neighboring blocks are merged into one 1024 × 1024 tile, and OpenMP threads handle the work inside each MPI rank.

Assume the cluster has:

- 4 nodes.
- 2 sockets per node.
- 16 cores per socket.
- 32 cores per node.
- 128 total cores.
- 128 GB RAM per node.
- 100 Gbps InfiniBand.

Before agglomeration, the configuration is:

- 64 MPI ranks.
- Each rank owns one 512 × 512 block.
- Many small halo messages.

After agglomeration, it becomes:

- 16 MPI ranks.
- Each rank owns one 1024 × 1024 tile made from a 2 × 2 group of blocks.
- Each rank uses 8 OpenMP threads.
- Fewer ranks and fewer messages.
- A larger interior-to-boundary ratio, so more stencil work is performed per halo element.

For a 1024 × 1024 tile:

$$
1024 \times 1 \times 8\text{ bytes} = 8192\text{ bytes} = 8\text{ KB per edge}
$$

Four edges produce about 32 KB of outbound halo payload per rank per step and roughly 32 KB inbound, so counting both directions gives about 64 KB of application payload per rank per step.

Memory per rank:

- Array `A`: 1024 × 1024 × 8 bytes ≈ 8 MB.
- Array `B`: 1024 × 1024 × 8 bytes ≈ 8 MB.
- Halos and scratch space: about 1–4 MB.
- Total: roughly 17–20 MB per rank for the core arrays and small working buffers, well below a 512 MB per-rank budget.

The application may still use substantially more memory once MPI buffers, HDF5 buffers, runtime metadata, and additional diagnostic arrays are included, so production sizing should measure resident memory rather than relying only on the two main arrays.

```
Before: 64 MPI ranks, each 512 x 512
After:  16 MPI ranks, each 1024 x 1024

Agglomerating 2 x 2 blocks:

+----+----+        +--------+
| A  | B  |   ->   |        |
+----+----+        |   R    |  One rank owns the merged tile
| C  | D  |        |        |
+----+----+        +--------+

Effect:
- Fewer ranks: 64 -> 16
- Fewer messages
- Larger messages: 8 KB per edge instead of 4 KB
- OpenMP threads handle work inside each tile
```

### IV. Mapping: Place Work Where Data Moves Least

Finally, map the ranks so that communication stays as local as possible and OpenMP threads remain close to their memory. A Cartesian rank layout together with explicit core binding provides a straightforward starting point.

Final configuration:

- 16 MPI ranks.
- 8 OpenMP threads per rank.
- 128 total threads.
- 4 nodes × 32 cores per node = 128 cores.

A simple 4 × 4 Cartesian rank layout is:

```text
R0   R1   R2   R3
R4   R5   R6   R7
R8   R9   R10  R11
R12  R13  R14  R15
```

One possible node placement is:

```
Node A: R0   R1   R2   R3
Node B: R4   R5   R6   R7
Node C: R8   R9   R10  R11
Node D: R12  R13  R14  R15
```

With this placement:

- East-west neighbors within the same tile row are usually on the same node.
- North-south communication crosses nodes.
- Each node runs 4 ranks.
- Each rank uses 8 threads.
- Each socket can host 2 ranks × 8 threads.

Recommended OpenMP placement:

```bash
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close
```

This asks OpenMP to bind each team to cores close to the thread that created the team. MPI rank binding must also reserve the same eight cores per rank; otherwise all eight OpenMP threads may compete inside a smaller CPU set.

NUMA placement matters as well. With **first-touch allocation**, each rank initializes its large arrays in parallel using the same thread placement that will later update them. This encourages memory pages to be allocated near the threads that use them.

### Runbook

The example implementation is assumed to live in `heat2d.c`.

Expected implementation features:

- Ping-pong buffers `A` and `B`.
- 5-point stencil update.
- Halo pack/unpack routines.
- Nonblocking MPI halo exchange.
- OpenMP parallel loops for interior and border updates.
- Residual calculation with `MPI_Allreduce` every 100 steps.
- Optional HDF5 checkpoint output.

#### Build and Quick Check

```bash
mpicc -O3 -march=native -fopenmp heat2d.c -o heat2d
mpirun -n 1 ./heat2d --nx 512 --ny 512 --steps 10 --tile 512 --verify analytic
```

#### Distributed Run

```bash
OMP_NUM_THREADS=8 OMP_PLACES=cores OMP_PROC_BIND=close \
mpirun -n 16 --map-by ppr:4:node:PE=8 --bind-to core --report-bindings \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024
```

#### Full Run with Threads and Checkpoints

```bash
OMP_NUM_THREADS=8 OMP_PLACES=cores OMP_PROC_BIND=close \
mpirun -n 16 --map-by ppr:4:node:PE=8 --bind-to core \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024 --checkpoint 200
```

#### Outputs and Logging

- If using **collective Parallel HDF5**, use one logical checkpoint file such as `chkpt_t<step>.h5`, with each rank writing its hyperslab collectively.
- If using independent per-rank files instead, a pattern such as `chkpt_t<step>_rank<rank>.h5` is simpler but can create metadata pressure on large file systems.
- Final output file: `final_temp.h5` when using collective output.
- Dataset names: `temp_step_<t>`.
- Logs should include:
  - residual every 100 steps,
  - total runtime,
  - computation time,
  - halo exchange time,
  - I/O time,
  - achieved updates per second.

### Design Checklist

| Question | Why It Matters |
|---|---|
| Are tasks balanced? | Prevents idle processors. |
| Is there enough parallel work? | Keeps all cores or nodes busy. |
| Are dependencies minimized? | Reduces synchronization and communication. |
| Are messages batched? | Avoids excessive latency overhead. |
| Is communication overlapped with computation? | Hides some communication cost. |
| Are frequently communicating tasks placed close together? | Improves locality and reduces network traffic. |
| Does each task fit memory/cache constraints? | Prevents paging and improves performance. |
| Has the program been profiled? | Confirms bottlenecks instead of guessing. |
| Does strong scaling still improve runtime? | Reveals when communication and synchronization dominate added compute resources. |
| Are rank and thread affinities verified? | Prevents accidental oversubscription or poor NUMA placement. |
| Is I/O architecture consistent with the file system? | Avoids turning checkpoints into the dominant scalability bottleneck. |

### Principles

- Aim for processors to finish their assigned work at roughly the same time; otherwise, faster workers spend time waiting for the slowest ones.
- Keep communication deliberate and compact. For example, one halo buffer per edge is usually better than many tiny messages.
- Add processors only while they still produce useful speedup. Poor scaling usually points to communication, synchronization, load imbalance, serial work, or too little computation per worker.
- Place tasks that communicate frequently close together in memory and in the network topology.
- Make tasks large enough to amortize overhead, but not so large that load balancing becomes difficult.

### Tools and Techniques

- Use MPI for communication across distributed-memory systems, including halo exchanges, reductions, and multi-node execution.
- Use OpenMP for shared-memory parallelism inside a node, especially for threading loops within each MPI rank.
- A hybrid MPI + OpenMP design can reduce rank count, inter-rank communication, and replicated MPI state, but it also makes thread affinity and NUMA placement more important.
- Parallel HDF5 provides scalable shared-file output for checkpoints and final simulation results.
- Profilers such as Intel VTune, Linux `perf`, HPCToolkit, TAU, and MPI-specific tools help separate computation hotspots from communication bottlenecks.
- Use pinning and affinity controls such as `OMP_PLACES`, `OMP_PROC_BIND`, and MPI binding options to keep threads and ranks close to their data.

### Common Mistakes

| Mistake | Why It Hurts | Better Approach |
|---|---|---|
| Creating too many tiny tasks | Scheduling and communication overhead dominate | Agglomerate tasks until granularity is reasonable |
| Ignoring halo costs | Boundary communication becomes the bottleneck | Batch halos and overlap communication with interior work |
| Using blocking communication everywhere | Ranks may wait instead of computing | Use nonblocking communication when independent work exists, then verify that overlap actually occurs |
| Mapping ranks without topology awareness | Increases cross-node and NUMA traffic | Use Cartesian layouts and binding options |
| Assuming equal data size means equal work | Some data regions may be more expensive | Measure work or use dynamic scheduling |
| Optimizing before profiling | Time is spent on the wrong bottleneck | Profile computation, communication, and I/O |
| Assuming nonblocking MPI guarantees overlap | Transfers may make little progress during computation | Measure overlap; use appropriate MPI progress settings or communication structure |
| Binding one hybrid rank to one core | OpenMP threads oversubscribe the same CPU set | Reserve the full core set for each rank, e.g. `PE=8` for eight threads |
| Treating per-rank HDF5 files as Parallel HDF5 | File-count and metadata overhead can grow rapidly | Use collective Parallel HDF5 for shared datasets when appropriate |
| Ignoring surface-to-volume ratio | Communication grows too large relative to local stencil work | Use compact subdomains and avoid over-partitioning |
