# Parallel Design Quiz

Notes: [Designing Parallel Programs](../notes/08_designing_parallel_programs.md), [Message Passing Interface (MPI)](../notes/05_mpi.md), [Hardware in Parallel Computing](../notes/06_hardware.md)

Check your understanding of the PCAM design method, decomposition strategies, communication costs and common parallel patterns.

#### Q. What are the four stages of Foster's PCAM parallel design methodology?

* [ ] Planning, Coding, Analysis, Monitoring
* [ ] Partitioning, Combination, Assignment, Measurement
* [ ] Preparation, Computation, Aggregation, Merging
* [x] Partitioning, Communication, Agglomeration, Mapping

> **Explanation:** Partitioning splits the work into tasks, communication identifies the data they exchange, agglomeration groups tasks, and mapping assigns them to processors.

#### Q. Which stage of the parallel design process focuses on breaking the problem down into discrete pieces of work?

* [x] Partitioning
* [ ] Agglomeration
* [ ] Communication
* [ ] Mapping

> **Explanation:** Partitioning exposes the available parallelism, ideally producing many more potential tasks than processors so later stages have flexibility.

#### Q. Which stage of the parallel design process focuses on what data tasks must exchange and how they coordinate?

* [x] Communication
* [ ] Agglomeration
* [ ] Partitioning
* [ ] Mapping

> **Explanation:** The communication stage identifies which tasks need each other's results and how they exchange them, distinguishing data movement from synchronization.

#### Q. A heat solver first gives one 512×512 block to each of 64 ranks, then is redesigned so that each of 16 ranks owns a 2×2 group of those blocks. Which PCAM stage is this change?

* [ ] Partitioning
* [ ] Communication
* [x] Agglomeration
* [ ] Mapping

> **Explanation:** Merging neighboring tasks into larger units cuts the number of ranks and messages and raises the interior-to-boundary ratio. Sixteen tiles still leave enough units to spread across the machine.

#### Q. You decide which node and which cores each of 16 MPI ranks runs on, placing ranks that exchange halos on the same node. Which PCAM stage is this?

* [ ] Communication
* [ ] Partitioning
* [x] Mapping
* [ ] Agglomeration

> **Explanation:** Mapping assigns tasks to cores, sockets, nodes or accelerators to balance load and keep frequently communicating tasks close together. The tasks themselves are already fixed by the earlier stages.

#### Q. Why does the partitioning stage come before the communication stage?

* [ ] Tasks must be grouped into larger units first
* [ ] The order of the four stages is arbitrary
* [ ] Each task's physical location must be known first
* [x] The division of work determines what data must be exchanged

> **Explanation:** Communication needs depend on the dependencies between the tasks produced by partitioning. Grouping happens later, in agglomeration, and placement in mapping.

#### Q. How should the PCAM stages be applied in practice?

* [ ] Strictly once and in order, never revisiting an earlier stage
* [x] Iteratively, revisiting earlier stages when later ones expose problems
* [ ] In any order, since the stages do not affect each other
* [ ] Only the mapping stage is still needed on modern hardware

> **Explanation:** For example, a mapping may reveal that communication is too costly, prompting different agglomeration or partitioning. The final design should be validated by measurement.

#### Q. Why do programmers often skip explicit mapping for threads on a typical desktop operating system?

* [x] The OS scheduler assigns threads to cores automatically
* [ ] Desktop computers have too few cores for mapping to matter
* [ ] Desktop operating systems forbid choosing a thread's core
* [ ] Mapping only applies to GPU programs

> **Explanation:** The scheduler places and migrates threads dynamically. Affinity and pinning can still matter for performance, especially on NUMA or HPC systems.

#### Q. What is domain decomposition?

* [ ] Dividing a problem by the different operations it performs
* [ ] Splitting a program into modules by business domain
* [x] Dividing the data into chunks that workers process alike
* [ ] Separating the problem domain from the solution domain

> **Explanation:** Each worker owns a region of the data. It works well when computation is mostly local, as in image filters, simulations and matrix operations.

#### Q. A 2D grid is split among four workers. Why do $2 \times 2$ square tiles usually communicate less than four thin strips?

* [x] Tiles have less boundary per area, so less halo exchange
* [ ] Square tiles contain fewer grid points in total
* [ ] Thin strips cannot be processed in parallel at all
* [ ] Square tiles never need any ghost cells

> **Explanation:** Computation grows with a tile's area, while communication grows with its perimeter. For an $N \times N$ grid, the cuts total $2N$ for tiles but $3N$ for strips.

#### Q. In a 2-D stencil, the side of a square tile doubles from $L$ to $2L$. How do its per-step computation and halo volume change?

* [ ] Both double
* [ ] Both grow 4 times
* [x] Computation grows 4 times, halo volume 2 times
* [ ] Computation grows 2 times, halo volume 4 times

> **Explanation:** Work grows with the area, $L^2$, and boundary data with the perimeter, $L$. Larger tiles do more computation per halo element, but too few tiles leave too little parallel work.

#### Q. Each rank owns a $1024 \times 1024$ tile of `double` values and, every step, sends one boundary row or column to each of its 4 neighbors. How much halo data does it send per step?

* [ ] 4,096 bytes
* [ ] 8,192 bytes
* [x] 32,768 bytes
* [ ] 8,388,608 bytes

> **Explanation:** One edge is $1024 \times 8 = 8192$ bytes, and four edges give 32,768 bytes, about 32 KiB. The tile itself is $1024^2 \times 8$ bytes, 8 MiB, so the halos are a small fraction of the data.

#### Q. What is functional decomposition?

* [ ] Splitting functions into smaller helper functions
* [ ] Dividing a dataset into equal contiguous blocks
* [ ] Removing unused functions to shrink the program
* [x] Dividing a problem by the different operations it performs

> **Explanation:** For example, a web request can pass through authentication, database, formatting and rendering stages, each handled by its own workers. This naturally forms pipelines.

#### Q. A three-stage pipeline takes 2 ms, 5 ms and 3 ms per item, with one worker per stage. What is its steady-state throughput?

* [ ] 100 items per second
* [x] 200 items per second
* [ ] 333 items per second
* [ ] 500 items per second

> **Explanation:** Once the pipeline is full, the slowest stage completes one item every 5 ms, so throughput is 200 items per second. Each item's latency is still 10 ms.

#### Q. A three-stage pipeline takes 2 ms, 5 ms and 3 ms per item, with one worker per stage. A second worker is added to the 5 ms stage. What is the new steady-state throughput?

* [ ] 200 items per second
* [x] About 333 items per second
* [ ] 400 items per second
* [ ] 500 items per second

> **Explanation:** Two workers finish middle-stage items every 2.5 ms on average, so the 3 ms stage becomes the bottleneck: $\frac{1000}{3} \approx 333$ items per second. Doubling the slowest stage did not double throughput.

#### Q. What is the difference between data parallelism and task parallelism?

* [ ] Data parallelism is always faster than task parallelism on CPUs
* [x] Data: same operation on different data; task: different operations at once
* [ ] Task parallelism always uses more memory than data parallelism
* [ ] Data parallelism applies only to database queries

> **Explanation:** An image filter applied to every pixel is data-parallel; serving unrelated requests or running pipeline stages at once is task-parallel. Real programs often combine both.

#### Q. What is an embarrassingly parallel computation?

* [ ] A computation too simple to be worth parallelizing
* [ ] A computation that always contains hidden race conditions
* [x] One made of independent tasks needing little or no communication
* [ ] A poorly designed algorithm that parallelizes badly in practice

> **Explanation:** Rendering separate frames or running independent Monte Carlo trials are examples. Such workloads usually scale well, limited mainly by load balance and I/O.

#### Q. What does a divide-and-conquer algorithm do when it reaches the base case?

* [ ] Merges all of the subproblems created so far
* [x] Stops subdividing and solves the subproblem directly
* [ ] Recursively solves a set of smaller subproblems
* [ ] Divides the problem into two smaller subproblems

> **Explanation:** The base case ends the recursion. In parallel versions, a cutoff on problem size also stops creating tasks too small to be worth scheduling.

#### Q. What is the master-worker (manager-worker) pattern?

* [x] One process hands out tasks to workers and collects results
* [ ] Every process does identical work with no coordination
* [ ] Workers elect a new master every time a task finishes
* [ ] A pattern used only for handling errors in workers

> **Explanation:** The master hands out tasks and gathers results, which balances load naturally. With many workers, the master itself can become a bottleneck.

#### Q. What is the bag-of-tasks (work queue) pattern?

* [ ] Running every task on every worker for redundancy
* [ ] Giving every worker a fixed list of tasks up front
* [ ] Storing completed results in an unordered collection
* [x] Workers repeatedly take the next task from a shared pool

> **Explanation:** Load balances dynamically: fast workers simply take more tasks, so uneven task costs balance out. Contention on the shared queue can limit scaling with many workers.

#### Q. What is work stealing?

* [x] Idle workers take tasks from the queues of busy workers
* [ ] A scheduler that cancels slow tasks and restarts them elsewhere
* [ ] A security attack in which a process takes another's CPU time
* [ ] A type of race condition in task queues

> **Explanation:** Each worker mostly uses its own deque, so there is no central bottleneck. Only idle workers steal, which keeps contention low while balancing load.

#### Q. How does static load balancing differ from dynamic load balancing?

* [ ] Static is meant for unchanging data; dynamic is for data that changes
* [ ] Static is always faster, while dynamic is always more accurate
* [x] Static assigns work up front; dynamic reassigns it during execution
* [ ] Static uses threads; dynamic uses processes

> **Explanation:** Static mapping has no run-time overhead and suits uniform, predictable work. Dynamic mapping handles irregular task costs but adds scheduling overhead and may reduce locality.

#### Q. What is the fork-join model?

* [ ] A model for committing database transactions
* [x] A task splits into parallel subtasks, then waits for all of them
* [ ] A pattern in which child processes duplicate the parent's open files
* [ ] A network protocol for joining new nodes to a running cluster

> **Explanation:** Parallel regions open and close around independent work, as in OpenMP parallel loops or recursive parallel merge sort.

#### Q. What is the SPMD (Single Program, Multiple Data) model?

* [ ] Each processor runs a different program on the same data
* [ ] A hardware design in which one instruction controls all cores
* [ ] One processor runs the program while others store data
* [x] Every processor runs the same program on different data

> **Explanation:** Most MPI programs are SPMD: every rank runs the same executable and uses its rank to select its portion of the data or role.

#### Q. What is speculative parallelism?

* [ ] Running parallel algorithms that have not been tested
* [x] Doing work in parallel before knowing whether it is needed
* [ ] Assigning tasks to processors at random to save time
* [ ] Estimating a program's speedup without running it

> **Explanation:** For example, both branches of a costly decision can be computed while the condition is evaluated. Unneeded results are discarded, trading extra work for lower latency.

#### Q. What characterises the bulk synchronous parallel (BSP) model?

* [ ] Processes communicate continuously without synchronizing
* [ ] All data is processed in one large batch on a single processor
* [x] Supersteps of local computation, communication and a barrier
* [ ] A hardware architecture built around one shared bus

> **Explanation:** Within a superstep, processes compute on local data and send messages. The barrier at the end makes the messages available for the next superstep.

#### Q. Which loop cannot be parallelized by simply splitting its iterations among threads?

* [ ] `for i in range(n): c[i] = a[i] + b[i]`
* [ ] `for i in range(n): c[i] = max(a[i], b[i])`
* [ ] `for i in range(n): b[i] = 2 * a[i]`
* [x] `for i in range(1, n): a[i] = a[i - 1] + b[i]`

> **Explanation:** Each iteration needs the result of the previous one, a loop-carried dependency. The others are independent per index; this one needs a restructured algorithm such as a parallel scan.

#### Q. What is a stencil computation?

* [x] Updating each grid element from a fixed pattern of its neighbors
* [ ] A protocol for broadcasting data from one root
* [ ] A tool for drawing a program's task dependency graph
* [ ] A code template that new parallel programs are written from

> **Explanation:** Heat diffusion and image blurs are stencils. In parallel versions, each worker needs halo data from neighbors at its boundaries every step.

#### Q. What is a reduction in parallel computing?

* [ ] Reducing the number of processors to save power
* [x] Combining many values into one, using an operation like sum
* [ ] Simplifying parallel code by removing all its locks
* [ ] Reducing a program's peak memory usage by compression

> **Explanation:** The operation, such as sum or max, must be associative. Workers compute partial results locally, then combine them, typically in a tree, which takes logarithmic steps instead of a serial loop.

#### Q. What does a prefix sum (scan) compute for the input `[3, 1, 4, 2]`?

* [ ] `[10, 10, 10, 10]`
* [ ] `[2, 4, 1, 3]`
* [ ] `[3, 1, 4, 2]`
* [x] `[3, 4, 8, 10]`

> **Explanation:** An inclusive scan gives the running total at every position. It can be computed in parallel and is a building block for many parallel algorithms.

#### Q. Why does data locality matter in parallel design?

* [ ] It keeps data stored in local files rather than databases
* [ ] It is a security measure that hides data from other processes
* [x] Using nearby data cuts communication and memory traffic
* [ ] It guarantees that no synchronization is needed

> **Explanation:** Accessing nearby data is far cheaper than fetching it from another core's cache, another NUMA node or another machine. Good partitioning and mapping keep most accesses local.

#### Q. What does the "owner computes" rule state?

* [x] The owner of each piece of data computes its updates
* [ ] The processor that compiled the code must execute it
* [ ] Only the master process may write the final results
* [ ] Every processor computes every value and compares results

> **Explanation:** Assigning computation by data ownership keeps writes local and makes clear which process must send or receive boundary values.

#### Q. In a task dependency graph, what do the edges represent?

* [ ] The physical links between processors
* [ ] The memory each task allocates
* [x] Data or ordering dependencies between tasks
* [ ] The order in which the programmer wrote the tasks

> **Explanation:** Tasks without a path between them can run concurrently. The longest path, the critical path, limits the minimum execution time.

#### Q. What is block decomposition of an array?

* [ ] Splitting source code into separate compilation blocks
* [x] Giving each worker one or more contiguous chunks of the array
* [ ] Assigning individual elements to workers in round-robin order
* [ ] Blocking all workers until the array is ready

> **Explanation:** Contiguous blocks give good cache locality and simple index arithmetic. They balance poorly when some regions are much more expensive to process than others.

#### Q. With cyclic decomposition, item $i$ goes to worker $i \bmod P$. With 4 workers, which worker processes item 10?

* [ ] Worker 0
* [ ] Worker 1
* [x] Worker 2
* [ ] Worker 3

> **Explanation:** $10 \bmod 4 = 2$. Round-robin assignment spreads expensive items across workers, improving balance at the cost of locality.

#### Q. $C = AB$, with each matrix split into $2 \times 2$ blocks. Which formula gives the block $C_{11}$?

* [ ] $A_{11}B_{12} + A_{12}B_{22}$
* [ ] $A_{11}B_{11}$
* [ ] $A_{11}B_{11} + A_{21}B_{12}$
* [x] $A_{11}B_{11} + A_{12}B_{21}$

> **Explanation:** Blocks multiply like scalar entries: block row 1 of $A$ times block column 1 of $B$. Each block product can reuse an optimized kernel on a cache-sized piece, while $A_{11}B_{12} + A_{12}B_{22}$ is $C_{12}$.

#### Q. Why is block-cyclic decomposition often used in distributed dense linear algebra?

* [ ] It eliminates all communication between processes
* [ ] It assigns the whole matrix to the fastest process
* [ ] It needs no knowledge of the matrix dimensions
* [x] It balances load while keeping locality within blocks

> **Explanation:** Blocks are dealt round-robin. Pure block layouts can leave some processes idle as the active region shrinks, and pure cyclic layouts lose locality. Block-cyclic is a compromise between the two.

#### Q. Message time is often modelled as $\alpha + n/B$ (startup latency plus size over bandwidth). What does this suggest about sending 1,000 tiny messages?

* [ ] It costs the same as one message with the same total bytes
* [x] Latency is paid 1,000 times, so batching them is usually faster
* [ ] Small messages are always faster because they use less bandwidth
* [ ] Only the bandwidth term matters for small messages

> **Explanation:** For small messages the fixed latency term dominates. Batching data reduces the number of startups paid, which is why agglomeration reduces communication cost.

#### Q. What is the goal of graph partitioning when mapping irregular tasks?

* [x] To balance work while cutting few communication edges
* [ ] To give every partition the same number of edges
* [ ] To place each task on its own separate processor
* [ ] To remove all dependencies from the task graph

> **Explanation:** Each cut edge represents communication between processors. Partitioners for meshes and sparse matrices balance the work per part while keeping cuts small.

#### Q. A stencil code uses nonblocking communication to overlap halo exchange with work. Which order of steps achieves this?

* [ ] Wait for the halos, then post receives, then compute every point
* [ ] Post receives, wait on them at once, then send the boundary data out
* [x] Post receives and sends, compute interior, wait, compute boundary
* [ ] Compute the boundary, then the interior, and finally post receives

> **Explanation:** Interior points do not depend on halo data, so they can be computed while messages are in flight. Only the boundary update must wait for the received values.

#### Q. A stencil code switches to `MPI_Isend`/`MPI_Irecv` and computes the interior before `MPI_Waitall`, but its runtime does not improve. A trace shows the data moving only inside `MPI_Waitall`. What is a likely cause?

* [ ] Computing the interior corrupted the pending send buffers
* [ ] Nonblocking calls are always slower than the matching blocking calls
* [x] MPI made little progress on the transfers during the computation
* [ ] `MPI_Waitall` sends every message a second time to check it

> **Explanation:** Nonblocking calls make overlap possible but do not guarantee it; many implementations advance large transfers only inside MPI calls. Occasional `MPI_Test` calls or asynchronous-progress settings can help, and the gain must be measured.

#### Q. A Jacobi solver keeps two arrays, `A` and `B`: it computes new values into `B` from `A`, then swaps them. Why not update `A` in place?

* [ ] A single array cannot be shared by several OpenMP threads
* [x] Some neighbors would already hold new-step values when they are read
* [ ] Two arrays halve the memory traffic of each time step
* [ ] MPI cannot exchange halos from an array that is being updated

> **Explanation:** A Jacobi step computes every value from the previous step only. In place, the result mixes old and new values, which changes the method, and in parallel it would depend on thread timing.

#### Q. A $4 \times 4$ grid of ranks R0-R15, numbered row by row, is placed four per node: R0-R3 on node A, R4-R7 on node B, and so on. With non-periodic boundaries, which halo exchanges cross between nodes?

* [x] North-south exchanges only
* [ ] East-west exchanges only
* [ ] Both directions, equally often
* [ ] None, because each node holds whole rows

> **Explanation:** East-west neighbors such as R1 and R2 share a node, while north-south neighbors such as R1 and R5 do not. Mapping decides which exchanges pay network cost.

#### Q. A hybrid job starts 16 MPI ranks with `OMP_NUM_THREADS=8`, but the launcher binds each rank to a single core. What is the likely result?

* [ ] The MPI launcher rejects the job as invalid
* [ ] OpenMP moves 7 of the threads to idle cores
* [ ] Each rank still gets 8 cores, since OpenMP ignores binding
* [x] Each rank's 8 threads time-share one core

> **Explanation:** Threads inherit their rank's CPU set, so all 8 compete for one core while others sit idle. Reserve 8 cores per rank, for example with Open MPI's `--map-by ppr:4:node:PE=8`, and check with `--report-bindings`.
