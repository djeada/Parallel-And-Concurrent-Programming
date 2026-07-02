## Designing Parallel Programs

Designing a parallel program means turning one large computation into smaller units of work that can run at the same time. A good design does more than create many tasks: it keeps processors busy, limits communication, preserves data locality, and maps work onto hardware in a way that reduces waiting.

A useful design framework is **PCAM**:

1. **Partitioning** — split the computation and data into tasks.
2. **Communication** — identify what data tasks must exchange.
3. **Agglomeration** — combine small tasks into larger units when that improves efficiency.
4. **Mapping** — assign tasks to processors, cores, nodes, or accelerators.

```
+-----------------------------+
|         Big Problem          |
+--------------+--------------+
               |
               v
+-----------------------------+
| 1. Partitioning              |
| Break the problem into       |
| smaller parallel tasks       |
+--------------+--------------+
               |
               v
+-----------------------------+
| 2. Communication             |
| Identify what data must be   |
| exchanged between tasks      |
+--------------+--------------+
               |
               v
+-----------------------------+
| 3. Agglomeration             |
| Combine tasks where useful   |
| to reduce overhead           |
+--------------+--------------+
               |
               v
+-----------------------------+
| 4. Mapping                   |
| Assign tasks to processors   |
| or hardware resources        |
+-----------------------------+
```

### Partitioning

Partitioning divides the problem into smaller pieces that can run concurrently. The goal is to expose enough parallel work while avoiding unnecessary dependencies between tasks.

```
      +-----------------------+
      |      Big Problem      |
      +-----------+-----------+
                  |
        +---------+---------+
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

- Create enough tasks to keep all available processors busy.
- Avoid creating so many tiny tasks that scheduling and communication overhead dominate the computation.

### Types of Decomposition

#### I. Domain Decomposition

Domain decomposition splits the **data space** into chunks. Each worker receives a portion of the data and performs the same or similar computation on that portion.

This works well when computation is mostly local, such as image processing, simulations, matrix operations, and grid-based numerical methods.

**Example: blurring a 4000 × 4000 image**

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

**Why it works:** most pixels depend only on nearby pixels, so most computation can run independently. Only the tile boundaries require communication.

#### II. Functional Decomposition

Functional decomposition splits the work by **type of operation** rather than by data region. It is often used to build pipelines.

**Example: web request pipeline**

- **Authentication stage:** verify the token or cookie.
- **Data stage:** query the database.
- **Transform stage:** sort, filter, or format results.
- **Render stage:** generate HTML or JSON.

```
Request
   |
   v
+--------+     +-------+     +-----------+     +--------+
| Auth   | --> | Data  | --> | Transform | --> | Render |
+--------+     +-------+     +-----------+     +--------+
```

Each stage can scale independently. For example, if database access becomes the bottleneck, you can allocate more workers to the Data stage.

Functional decomposition is useful when:

- Different stages require different resources.
- The pipeline processes many independent inputs.
- Each stage can be tested and optimized separately.

#### III. Cyclic Decomposition

In parallel programming, cyclic decomposition usually means distributing tasks or loop iterations in a **round-robin** pattern across workers.

Instead of giving each worker one large contiguous block, cyclic decomposition assigns item `i` to worker `i mod P`, where `P` is the number of workers.

**Example: 12 loop iterations on 4 workers**

| Iteration | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|----------:|---|---|---|---|---|---|---|---|---|---|----|----|
| Worker    | 0 | 1 | 2 | 3 | 0 | 1 | 2 | 3 | 0 | 1 | 2  | 3  |

```
Worker 0: iterations 0, 4, 8
Worker 1: iterations 1, 5, 9
Worker 2: iterations 2, 6, 10
Worker 3: iterations 3, 7, 11
```

**When cyclic decomposition helps:**

- Work per iteration is uneven or hard to predict.
- Expensive iterations are scattered throughout the input.
- A simple block split would overload one worker.

**Tradeoff:** cyclic decomposition often improves load balance, but it can reduce locality because each worker touches data spread across the whole domain.

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

**Why blocks help:**

- **Block multiplication:** if `C = A B`, then $C_{11}=A_{11}B_{11}+A_{12}B_{21}$ and similarly for the other blocks. This lets the program reuse optimized kernels on smaller pieces.
- **Cache locality:** contiguous blocks often fit better in cache than whole matrices.
- **Parallelism:** independent blocks can be processed by different workers.
- **Structure detection:** block-diagonal matrices can be solved as smaller independent problems.

**Related strategy: block-cyclic decomposition**

Block-cyclic decomposition combines block and cyclic ideas. It assigns blocks round-robin across workers, which helps balance load while still preserving some locality.

### Communication

Communication is the exchange of data between tasks. It is necessary when one task needs another task’s results. The best parallel designs minimize communication without sacrificing correctness.

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

Local communication occurs between tasks on the same processor, socket, or node. It usually uses shared memory and is faster than network communication.

**Example:** two cores on the same CPU socket share data through memory.

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

Global communication occurs between tasks on different nodes or physical machines. It usually travels over a network and is more expensive than local communication.

**Example:** MPI ranks running on separate servers exchange boundary data.

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

Collective communication involves a group of tasks. Common collective operations include broadcast, scatter, gather, reduce, and all-reduce.

**Scatter:** one task sends different pieces of data to different tasks.

```
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

**Broadcast:** one task sends the same data to all tasks.

```
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

**Reduce:** many tasks contribute values, and one result is produced.

```
Worker values:  4, 7, 2, 9
Operation:      sum
Result:         22
```

**All-reduce:** every task receives the reduced result. This is common in simulations and machine learning training.

---

### Communication Modes

#### I. Blocking Communication

Blocking communication waits until the operation is complete before the task continues.

- Easier to reason about.
- Safer for simple programs.
- Can cause idle time if a task waits while it could have been doing useful work.

#### II. Nonblocking Communication

Nonblocking communication starts an operation and allows the task to continue while the message is in progress.

- Enables overlap between communication and computation.
- Requires careful handling to ensure buffers are not modified too early.
- Usually requires a later wait or test operation.

**Example pattern:**

1. Post nonblocking receives.
2. Send boundary data.
3. Compute the interior region while messages travel.
4. Wait for received halo data.
5. Compute the boundary region.

### Concepts in Communication

#### I. Overhead

Overhead is time spent managing communication rather than doing the main computation. It includes message setup, synchronization, copying, packing, unpacking, and scheduling.

#### II. Latency

Latency is the time required for a message to begin reaching its destination. It matters most when messages are small and frequent.

#### III. Bandwidth

Bandwidth is the amount of data that can be transferred per second. It matters most when messages are large.

#### IV. Contention

Contention occurs when multiple tasks compete for the same memory bus, network link, file system, or synchronization point.

### Agglomeration

Agglomeration combines smaller tasks into larger tasks after the initial partitioning step. The purpose is to reduce overhead, improve locality, and create tasks that better match the target hardware.

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

Granularity describes how much computation occurs per communication event.

$$
\text{Granularity} = \frac{\text{Computation}}{\text{Communication}}
$$

#### Fine-Grained Parallelism

Fine-grained parallelism uses many small tasks.

**Advantages:**

- Excellent load balancing.
- Many opportunities for parallel execution.
- Useful when work is irregular and dynamic scheduling is available.

**Disadvantages:**

- High scheduling overhead.
- Frequent communication.
- Lower computation-to-communication ratio.

#### Coarse-Grained Parallelism

Coarse-grained parallelism uses fewer, larger tasks.

**Advantages:**

- Lower communication overhead.
- Better locality.
- Higher computation-to-communication ratio.

**Disadvantages:**

- Potential load imbalance.
- Less flexibility for dynamic scheduling.
- Some processors may become idle if tasks are uneven.

#### Practical Balance

The best granularity depends on the problem, hardware, and runtime system. A good design usually starts with many logical tasks, then agglomerates them until communication and scheduling overhead are acceptable.

### Mapping

Mapping assigns agglomerated tasks to hardware resources such as cores, sockets, nodes, GPUs, or clusters. Good mapping improves load balance and reduces communication distance.

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

**Best for:** predictable workloads with uniform task sizes.

#### II. Dynamic Mapping

Tasks are assigned during execution based on current load.

**Best for:** irregular workloads where task cost is hard to predict.

#### III. Hierarchical Mapping

Tasks are mapped in levels: for example, first to nodes, then sockets, then cores.

**Best for:** multi-node systems with NUMA domains, GPUs, or complex network topology.

#### IV. Task Clustering

Tasks that communicate frequently are grouped and placed near each other.

**Best for:** graph algorithms, stencil computations, and workflows with known communication patterns.

#### V. Graph Partitioning

Represent tasks as graph nodes and communication as graph edges. Then partition the graph so that heavily connected tasks stay together and edge cuts are minimized.

**Best for:** irregular meshes, sparse matrix computations, and graph analytics.

## Example Workflow

### 2-D Heat Diffusion

**Problem:** simulate heat diffusion on a 4096 × 4096 grid for 5,000 time steps using a 5-point stencil in double precision.

The program uses two row-major arrays, `A[nx][ny]` and `B[nx][ny]`, which alternate roles each step. This is often called a **ping-pong buffer** pattern.

A typical 5-point stencil update is:

$$
B[i,j] = 0.25 \times (A[i-1,j] + A[i+1,j] + A[i,j-1] + A[i,j+1])
$$

Periodic checkpoints are written to HDF5 files.

### What We Are Designing

- A parallel 2-D heat diffusion solver.
- A domain decomposition that divides the grid into tiles.
- Halo exchanges between neighboring tiles.
- Hybrid parallelism using MPI between ranks and OpenMP within each rank.
- Checkpoint output for long-running simulations.

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

**Goal:** split the domain into equal, independent tasks.

**Method:** use an 8 × 8 domain decomposition, giving 64 blocks.

- The full grid is 4096 × 4096.
- Each block is 512 × 512 cells.
- Each block performs 512 × 512 = 262,144 cell updates per step.
- Blocks are named `T(0,0)` through `T(7,7)` or numbered `T0` through `T63` in row-major order.
- Because every block has the same size, load balance is nearly uniform on homogeneous CPUs.

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

**Goal:** exchange only the boundary data needed for each stencil step.

**Method:** use nonblocking point-to-point halo exchanges and occasional collectives.

For each time step:

1. Each tile exchanges one row or column with its north, south, east, and west neighbors.
2. A 512 × 512 block sends one row of 512 double-precision values to the north and south.
3. It also sends one column of 512 double-precision values to the east and west.
4. Each edge message is approximately:

$$
512 \times 1 \times 8\text{ bytes} = 4096\text{ bytes} = 4\text{ KB}
$$

5. With four edges, each block exchanges about 16 KB per step.
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

**Recommended overlap pattern:**

```text
1. Post Irecv for north/south/east/west halos.
2. Pack and Isend local boundary rows/columns.
3. Compute the interior region that does not need new halos.
4. Wait/Test for halo receives to complete.
5. Unpack halos and compute border cells.
```

### III. Agglomeration: Reduce Rank Count but Keep Memory Safe

**Goal:** reduce communication overhead and MPI rank count without exceeding memory limits.

**Method:** merge 2 × 2 blocks into one larger tile and use OpenMP threads inside each MPI rank.

Assume the cluster has:

- 4 nodes.
- 2 sockets per node.
- 16 cores per socket.
- 32 cores per node.
- 128 total cores.
- 128 GB RAM per node.
- 100 Gbps InfiniBand.

**Before agglomeration:**

- 64 MPI ranks.
- Each rank owns one 512 × 512 block.
- Many small halo messages.

**After agglomeration:**

- 16 MPI ranks.
- Each rank owns one 1024 × 1024 tile made from a 2 × 2 group of blocks.
- Each rank uses 8 OpenMP threads.
- Fewer ranks and fewer messages.

For a 1024 × 1024 tile:

$$
1024 \times 1 \times 8\text{ bytes} = 8192\text{ bytes} = 8\text{ KB per edge}
$$

Four edges produce about 32 KB of halo data per rank per step.

Memory per rank:

- Array `A`: 1024 × 1024 × 8 bytes ≈ 8 MB.
- Array `B`: 1024 × 1024 × 8 bytes ≈ 8 MB.
- Halos and scratch space: about 1–4 MB.
- Total: roughly 17–20 MB per rank, well below a 512 MB per-rank budget.

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

**Goal:** minimize cross-node traffic and NUMA penalties.

**Method:** use a Cartesian rank layout and core pinning.

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

This keeps threads close to the rank’s memory allocation and reduces NUMA overhead.

### Runbook

**Code:** `heat2d.c`

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
./heat2d --nx 512 --ny 512 --steps 10 --tile 512 --verify analytic
```

#### Distributed Run

```bash
mpirun -n 16 --map-by ppr:4:node --bind-to core --report-bindings \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024
```

#### Full Run with Threads and Checkpoints

```bash
OMP_NUM_THREADS=8 OMP_PLACES=cores OMP_PROC_BIND=close \
mpirun -n 16 --map-by ppr:4:node --bind-to core \
  ./heat2d --nx 4096 --ny 4096 --steps 5000 --tile 1024 --checkpoint 200
```

#### Outputs and Logging

- Checkpoint files: `chkpt_t<step>_tile<rank>.h5`.
- Final output file: `final_temp.h5`.
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

### Principles

- **Load balancing:** processors should finish their assigned work at similar times. Poor load balance leaves some processors waiting.
- **Minimizing communication:** fewer and better-organized messages reduce overhead. Batching halo data into one buffer per edge is usually better than sending many tiny messages.
- **Scalability:** adding processors should produce useful speedup. Poor scaling often indicates too much communication, synchronization, load imbalance, or serial work.
- **Locality:** tasks that frequently exchange data should be placed close together in memory or network topology.
- **Granularity:** tasks should be large enough to do meaningful work between communication events but small enough to balance load.

### Tools and Techniques

- **MPI:** communication across distributed-memory systems. MPI is commonly used for halo exchanges, reductions, and multi-node execution.
- **OpenMP:** shared-memory parallelism inside a node. It is useful for threading loops within each MPI rank.
- **Hybrid MPI + OpenMP:** combines distributed-memory and shared-memory parallelism. It often reduces rank count and improves memory use on modern clusters.
- **Parallel HDF5:** scalable file output for checkpoints and final simulation results.
- **Profiling tools:** Intel VTune, GNU gprof, Linux `perf`, HPCToolkit, TAU, and MPI profilers can reveal computation hotspots and communication bottlenecks.
- **Pinning and affinity:** environment variables such as `OMP_PLACES`, `OMP_PROC_BIND`, and MPI binding options help keep threads and ranks close to their data.

### Common Mistakes

| Mistake | Why It Hurts | Better Approach |
|---|---|---|
| Creating too many tiny tasks | Scheduling and communication overhead dominate | Agglomerate tasks until granularity is reasonable |
| Ignoring halo costs | Boundary communication becomes the bottleneck | Batch halos and overlap communication with interior work |
| Using blocking communication everywhere | Ranks wait instead of computing | Use nonblocking communication for predictable exchanges |
| Mapping ranks without topology awareness | Increases cross-node and NUMA traffic | Use Cartesian layouts and binding options |
| Assuming equal data size means equal work | Some data regions may be more expensive | Measure work or use dynamic scheduling |
| Optimizing before profiling | Time is spent on the wrong bottleneck | Profile computation, communication, and I/O |
