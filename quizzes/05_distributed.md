# Distributed Computing and MPI Quiz

Notes: [Message Passing Interface (MPI)](../notes/05_mpi.md), [Hardware: Distributed and Cluster Computing](../notes/06_hardware.md)

Check your understanding of message passing with MPI and the basic trade-offs of distributed systems.

#### Q. Compared with shared-memory systems, distributed-memory architectures are generally considered _____.

* [ ] easier to program
* [x] more scalable
* [ ] lower latency
* [ ] less complex

> **Explanation:** Adding nodes, each with its own memory, avoids a single shared memory bottleneck. The price is explicit communication over a network, which is slower and harder to program.

#### Q. Which scenario is the best fit for point-to-point communication?

* [x] A small number of specific tasks need to exchange data with each other
* [ ] Every task must combine its value into one global sum at the end
* [ ] One task must send the same data to every other task in the job
* [ ] Every task must send distinct data to every other task

> **Explanation:** Point-to-point operations such as `MPI_Send` and `MPI_Recv` connect one sender with one receiver. Patterns involving the whole group are better expressed with collectives.

#### Q. In MPI, what is a rank?

* [ ] The operating-system PID of an MPI process
* [ ] The scheduling priority MPI gives a process
* [ ] The number of the CPU core a process runs on
* [x] A process's index within a communicator, starting at 0

> **Explanation:** Ranks run from 0 to size minus 1 and are assigned per communicator, so the same process can have different ranks in different communicators. They are not OS process IDs.

#### Q. What is `MPI_COMM_WORLD`?

* [ ] A communicator for talking to programs running outside the MPI job
* [ ] A global variable shared by every process
* [x] The predefined communicator containing all processes started in the job
* [ ] A function that creates new processes while the job is running

> **Explanation:** After `MPI_Init`, every process belongs to `MPI_COMM_WORLD`. Programs can derive smaller communicators from it for subgroups.

#### Q. What does an MPI communicator combine?

* [x] A process group and a context that isolates its messages
* [ ] A network socket and a buffer for each message
* [ ] A process and the thread that sends its messages
* [ ] A send buffer and a matching receive buffer

> **Explanation:** The context guarantees that messages sent in one communicator cannot be received in another, which is why libraries often use their own duplicated communicator.

#### Q. Eight processes call `MPI_Comm_split(MPI_COMM_WORLD, rank / 4, rank, &new_comm)`, where `rank` is the world rank. What rank and size does world rank 6 see in `new_comm`?

* [ ] Rank 6, size 8
* [ ] Rank 0, size 4
* [ ] Rank 1, size 2
* [x] Rank 2, size 4

> **Explanation:** The color `6 / 4 = 1` groups it with world ranks 4-7, and the key orders that group by world rank, so it becomes rank 2 of 4. Ranks belong to a communicator, not to a process.

#### Q. An MPI program calls `MPI_Comm_rank` and `MPI_Comm_size` on `MPI_COMM_WORLD` and is launched with `mpirun -np 4`. What values do they report?

* [ ] Ranks 1 to 4, and size 4
* [x] Ranks 0 to 3, and size 4
* [ ] Ranks 0 to 4, and size 5
* [ ] Every process reports rank 0, and size 4

> **Explanation:** `MPI_Comm_size` gives the number of processes in the communicator. Each process gets a distinct rank starting from 0.

#### Q. What does this launch command start?

```bash
mpiexec -n 1 ./manager : -n 4 ./worker
```

* [ ] Five copies of `manager`, because only the first program is used
* [ ] Two separate jobs, whose `MPI_COMM_WORLD` sizes are 1 and 4
* [ ] One `manager` process that later spawns 4 `worker` processes itself
* [x] One job of 5 ranks running two programs, sharing one `MPI_COMM_WORLD`

> **Explanation:** The colon separates the programs of one MPMD (multiple-program, multiple-data) launch. Launchers such as MPICH's and Open MPI's number ranks in the order given, so `manager` is rank 0 and the workers are ranks 1-4.

#### Q. What is the purpose of `MPI_Finalize()`?

* [x] To clean up the MPI environment after all MPI work is complete
* [ ] To combine the final results from all processes
* [ ] To send any final messages to the root process
* [ ] To act as a barrier that guarantees all output is printed

> **Explanation:** Every process calls it once at the end, after completing its communication. Most MPI calls are not allowed afterwards.

#### Q. When a blocking `MPI_Send` returns, what is guaranteed?

* [ ] The receiver has received and processed the message
* [x] The send buffer can be safely reused
* [ ] The matching receive has been posted
* [ ] The message has been written to disk

> **Explanation:** Standard-mode `MPI_Send` may return after copying the data into a system buffer or only after the receiver starts receiving. Either way, only buffer reuse is guaranteed.

#### Q. A process calls `MPI_Isend` on a buffer. When may it safely modify that buffer?

* [ ] Immediately after `MPI_Isend` returns
* [ ] After the receiving process calls `MPI_Finalize`
* [x] Once `MPI_Wait` or `MPI_Test` reports completion
* [ ] Never, because the buffer belongs to MPI from then on

> **Explanation:** A nonblocking send only starts the operation. Until the request completes, MPI may still be reading the buffer, so changing it could corrupt the message.

#### Q. What is wrong with this code on the receiving rank?

```c
MPI_Irecv(buf, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &req);
printf("%f\n", buf[0]);
MPI_Wait(&req, MPI_STATUS_IGNORE);
```

* [ ] Nothing, because `MPI_Irecv` fills `buf` before returning
* [ ] `MPI_Wait` must be called before `MPI_Irecv`
* [x] It reads `buf` before the receive has completed
* [ ] `MPI_Irecv` cannot receive `MPI_DOUBLE` data

> **Explanation:** Until `MPI_Wait` or a successful `MPI_Test` completes the request, the buffer must not be accessed: it may still hold old or partly written data. Read `buf` after the wait.

#### Q. How does `MPI_Test` differ from `MPI_Wait`?

* [ ] `MPI_Test` cancels the operation if it is not finished
* [ ] `MPI_Test` sends a test message; `MPI_Wait` sends real data
* [ ] `MPI_Test` works only for sends; `MPI_Wait` only for receives
* [x] `MPI_Test` checks without blocking; `MPI_Wait` blocks until done

> **Explanation:** `MPI_Test` sets a flag and returns immediately, so a process can do other work and check again later. `MPI_Wait` is the blocking choice.

#### Q. When does a synchronous-mode send, `MPI_Ssend`, complete?

* [ ] As soon as the data is copied into a system buffer
* [x] Only after the matching receive has started
* [ ] Only after the receiver has finished processing the data
* [ ] Immediately, like a nonblocking send

> **Explanation:** `MPI_Ssend` never completes by buffering alone. Testing code with it reveals programs that only work because the implementation happens to buffer messages.

#### Q. Two ranks each run the code below, with `other` set to the opposite rank. It completes with `n = 1` but hangs with `n = 1000000`. Why?

```c
MPI_Send(sendbuf, n, MPI_INT, other, 0, MPI_COMM_WORLD);
MPI_Recv(recvbuf, n, MPI_INT, other, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
```

* [x] Small sends were buffered; large ones wait for receives that never start
* [ ] `MPI_Send` rejects messages above a fixed size limit and retries forever
* [ ] Large messages are split into parts that arrive out of order
* [ ] `MPI_Recv` can receive only messages that fit in one network packet

> **Explanation:** Standard-mode `MPI_Send` may buffer or may wait for the matching receive; implementations usually buffer only small messages. Each rank then waits in `MPI_Send` for the other, so the code is unsafe for every `n`.

#### Q. Two ranks must swap large buffers. Which version cannot deadlock, however much or little MPI buffers?

* [ ] Each rank calls `MPI_Send`, then `MPI_Recv`
* [ ] Each rank calls `MPI_Recv`, then `MPI_Send`
* [x] Each rank calls `MPI_Isend`, then `MPI_Recv`, then `MPI_Wait`
* [ ] Each rank calls `MPI_Ssend`, then `MPI_Recv`

> **Explanation:** The nonblocking send lets each rank reach its receive, so both transfers complete. Send-first relies on buffering, while `MPI_Ssend`-first and receive-first always deadlock. `MPI_Sendrecv` is another safe choice.

#### Q. What does `MPI_Barrier(comm)` guarantee?

* [x] No process leaves it until every process has entered it
* [ ] Only one process at a time can send messages afterwards
* [ ] All pending messages in the communicator are delivered
* [ ] All processes print their output in rank order

> **Explanation:** A barrier is an explicit synchronization point. It says nothing about message delivery and should not be used to patch an unclear communication design.

#### Q. Can a collective such as `MPI_Bcast` be relied on as a substitute for `MPI_Barrier`?

* [ ] Yes, every collective synchronizes all processes like a barrier
* [ ] No, because collectives can be called by only some processes
* [ ] Yes, as long as the root process is rank 0
* [x] No, it may return before other processes have entered it

> **Explanation:** Every process in the communicator must make the matching call, but only `MPI_Barrier` guarantees that all have arrived. A broadcast root, for example, may return as soon as its buffer can be reused.

#### Q. What is collective communication in MPI?

* [ ] A message exchanged between exactly two processes
* [x] An operation every process in the communicator must call
* [ ] Communication that uses shared memory instead of messages
* [ ] Messages exchanged between two separate MPI jobs

> **Explanation:** Collectives express group patterns directly, which makes intent clear and lets the implementation use efficient tree or ring algorithms.

#### Q. What does `MPI_Bcast()` do?

* [x] Copies the root's data to every process in the communicator
* [ ] Broadcasts a message to every computer on the network
* [ ] Sends a different piece of data to each process
* [ ] Collects one value from every process at the root

> **Explanation:** After the call, every process has a copy of the root's buffer. Scattering different pieces is `MPI_Scatter`.

#### Q. Four ranks hold the values 4, 7, 2 and 9 and call `MPI_Reduce` with `MPI_SUM` and root 0. What is the result?

* [ ] Every rank receives 22
* [ ] Each rank receives the sum of the values before it
* [ ] Rank 0 receives 4, its own value
* [x] Rank 0 receives 22

> **Explanation:** `MPI_Reduce` combines the values with the given operation and stores the result only at the root. `MPI_Allreduce` would give 22 to every rank.

#### Q. How does `MPI_Allreduce` differ from `MPI_Reduce`?

* [ ] It works on all MPI data types, not only on integers
* [ ] It is nonblocking, while `MPI_Reduce` is blocking
* [x] Every process receives the reduced result, not just the root
* [ ] It reduces the data from every communicator in the job at once

> **Explanation:** All-reduce is common when every process needs the global value, such as a convergence norm in a simulation or summed gradients in distributed training.

#### Q. Every rank of an iterative solver runs the loop below. Why does it use `MPI_Allreduce` rather than `MPI_Reduce` to rank 0?

```c
while (globalerr > tol) {
    exchange_halos();
    compute();
    localerr = max_local_error();
    MPI_Allreduce(&localerr, &globalerr, 1, MPI_FLOAT, MPI_MAX, comm);
}
```

* [ ] `MPI_Reduce` cannot compute a maximum, only sums
* [x] Every rank needs `globalerr` to decide whether to continue
* [ ] `MPI_Allreduce` needs no network communication
* [ ] `MPI_Reduce` works only with integer data types

> **Explanation:** With `MPI_Reduce`, only the root would see the new error. The other ranks would test a stale value and could leave the loop at a different iteration, mismatching later halo exchanges and collectives.

#### Q. What is the difference between `MPI_Scatter` and `MPI_Gather`?

* [ ] Scatter is meant for small messages, while Gather is for large ones
* [ ] Scatter is synchronous; Gather is asynchronous and returns early
* [ ] Scatter sends the same data everywhere; Gather sums the data
* [x] Scatter splits root data among processes; Gather collects pieces at the root

> **Explanation:** They are inverse patterns. A typical program scatters chunks of an array, lets each process work on its chunk, then gathers the results.

#### Q. The root must distribute 1,000 array elements as evenly as possible among 3 processes. Which collective fits?

* [ ] `MPI_Scatter`, with a count of 333 for every process
* [ ] `MPI_Alltoall`, with the root as the only sender
* [x] `MPI_Scatterv`, with per-process counts and displacements
* [ ] `MPI_Gather`, with counts of 334, 333 and 333

> **Explanation:** `MPI_Scatter` sends the same count to every process, and 1,000 is not divisible by 3. `MPI_Scatterv` accepts counts such as 334, 333 and 333 with matching displacements.

#### Q. What does `MPI_Alltoall` do?

* [ ] Broadcasts one buffer from every process to the root
* [x] Every process sends a distinct block of data to every other process
* [ ] Reduces data from all processes and broadcasts the result
* [ ] Synchronizes all processes without exchanging data

> **Explanation:** All-to-all is a complete exchange, used for example in distributed matrix transposes and FFTs. Its cost grows quickly with process count.

#### Q. Ranks 0-3 contribute the values 1, 2, 3 and 4 to `MPI_Scan` with `MPI_SUM`. What does rank 2 receive?

* [ ] 3
* [ ] 5
* [x] 6
* [ ] 10

> **Explanation:** `MPI_Scan` is an inclusive prefix reduction: rank $i$ receives the combination of values from ranks 0 to $i$. Rank 2 gets $1 + 2 + 3 = 6$; the exclusive `MPI_Exscan` would give 3.

#### Q. What are message tags used for in MPI?

* [x] Distinguishing kinds of messages between the same processes
* [ ] Labelling messages so debuggers can display them
* [ ] Selecting how the message's header is compressed in transit
* [ ] Choosing the key used to encrypt the message

> **Explanation:** A receive matches on communicator, source and tag. Distinct tags let a process receive, for example, data messages and control messages separately.

#### Q. An application and a library it calls both send messages on `MPI_COMM_WORLD` with tag 0. What can go wrong, and what is the usual fix?

* [x] A receive may match the other's message; the library should use `MPI_Comm_dup`
* [ ] Tag 0 is reserved for libraries; the application must use tag 1
* [ ] The library's messages are dropped; call `MPI_Barrier` before each library call
* [ ] Nothing, because MPI matches messages by the calling function

> **Explanation:** Matching uses only communicator, source and tag. A duplicated communicator has the same group but its own context, so library traffic can never match the application's receives.

#### Q. Rank 0 receives from ranks 1 and 2 with `MPI_ANY_SOURCE`. Why might the output differ between runs?

* [ ] `MPI_ANY_SOURCE` drops one of the messages at random
* [ ] MPI reorders the messages from each sender
* [ ] `MPI_ANY_SOURCE` only works with blocking sends
* [x] Messages from different senders can arrive in either order

> **Explanation:** Arrival order across senders depends on timing. Naming the expected source rank, and a specific tag, removes the ambiguity when the program needs a fixed order.

#### Q. In a ring, each rank sends to `(rank + 1) % np`. To receive deterministically instead of using `MPI_ANY_SOURCE`, which source should each rank name in C?

* [ ] `(rank + 1) % np`
* [ ] `(rank - 1) % np`
* [x] `(rank - 1 + np) % np`
* [ ] `rank`

> **Explanation:** Each rank's message comes from its left neighbor. Adding `np` first matters because in C, `-1 % np` is `-1`, which is not a valid rank for rank 0.

#### Q. Single-threaded rank 1 sends two messages to rank 0 with the same tag and communicator, and rank 0 posts two receives that match either. In what order does rank 0 receive them?

* [ ] In any order, depending on network timing
* [x] In the order they were sent
* [ ] Only the second one, which overwrites the first
* [ ] In order of message size, smallest first

> **Explanation:** MPI's non-overtaking rule: messages from one sender to one receiver that match the same receive arrive in send order. There is no ordering guarantee across different senders, or between sends made concurrently by several threads.

#### Q. Why might a process call `MPI_Probe` before `MPI_Recv`?

* [ ] To check whether the network link is working
* [x] To learn an incoming message's size before allocating a buffer
* [ ] To test whether the sending process is still alive
* [ ] To measure the round-trip network latency to the sender

> **Explanation:** `MPI_Probe` returns a status describing a matching message without receiving it. `MPI_Get_count` on that status gives its size, so the buffer can be allocated exactly.

#### Q. A receive specifies a count of 100 elements, but the matching message has 60. What happens?

* [ ] MPI reports an error because the counts differ
* [ ] The receive waits for 40 more elements
* [ ] The remaining 40 elements are filled with zeros
* [x] It succeeds, and `MPI_Get_count` reports 60

> **Explanation:** The receive count is a capacity, so it can exceed the message size. Receiving a message larger than the buffer is the error case, `MPI_ERR_TRUNCATE`.

#### Q. What is a Cartesian topology in MPI?

* [x] A virtual grid of processes, in 1D, 2D or 3D
* [ ] The physical layout of cables in a cluster
* [ ] A row-major memory layout for storing matrices
* [ ] A visualisation produced by MPI debugging tools

> **Explanation:** `MPI_Cart_create` builds the grid and `MPI_Cart_shift` finds neighbor ranks, which suits grid-based simulations; dimensions can be periodic. Virtual topologies do not move processes to other hardware.

#### Q. What is an intercommunicator, created for example with `MPI_Intercomm_create`?

* [ ] A communicator limited to the processes on one node
* [ ] A copy of a communicator that has a new context
* [x] A communicator connecting two disjoint groups of processes
* [ ] A communicator whose ranks are arranged as a grid

> **Explanation:** In point-to-point calls on an intercommunicator, ranks refer to the remote group, so, for example, a simulation group can send to an analysis group. A copy with a new context comes from `MPI_Comm_dup`.

#### Q. Why use an MPI derived datatype such as `MPI_Type_vector` to send a matrix column?

* [ ] It compresses the column's data before sending it
* [x] It describes the strided layout, so no manual packing is needed
* [ ] It transposes the column into a row on the receiving side
* [ ] It is the only way MPI can send floating-point data

> **Explanation:** In a row-major matrix, a column's elements are not contiguous. A vector type with the right stride lets MPI read them directly.

#### Q. What are ghost (halo) cells in a domain-decomposed simulation?

* [ ] Cells that store data deleted during the simulation
* [ ] Unused cells added to pad arrays to a power of two
* [x] Local copies of neighbors' boundary values
* [ ] Cells that record errors detected during communication

> **Explanation:** A stencil near the edge of a subdomain needs neighbor values. Each step, processes exchange their boundary data to refresh these halo copies.

#### Q. What is latency hiding in distributed computing?

* [x] Overlapping communication with independent computation
* [ ] Hiding network delays from users with a progress bar
* [ ] Compressing messages so they arrive sooner
* [ ] Caching every remote value locally in advance

> **Explanation:** A process can post nonblocking sends and receives, compute work that does not depend on them, and only then wait. Actual overlap depends on the MPI implementation making progress.

#### Q. What does the CAP theorem say about distributed data stores?

* [ ] They can guarantee consistency, availability and partition tolerance at all times
* [ ] Replication always reduces both consistency and availability
* [ ] Caching, availability and performance cannot be optimised together
* [x] During a network partition, a system must choose between consistency and availability

> **Explanation:** Partitions cannot be ruled out in real networks. While one lasts, a node must either refuse requests (keeping consistency) or answer with possibly stale data (keeping availability).

#### Q. What is eventual consistency?

* [ ] Every read always returns the latest write
* [ ] Data becomes consistent only after the whole system restarts
* [x] If updates stop, all replicas eventually converge to the same value
* [ ] Writes are applied to all replicas in one fixed global order

> **Explanation:** Replicas may temporarily disagree, so a read can return stale data. Many highly available systems accept this in exchange for lower latency and partition tolerance.

#### Q. What is a primary trade-off of replicating data across nodes?

* [x] It improves availability and reads but makes consistency costlier
* [ ] It always reduces the latency of every single write
* [ ] It removes any need to handle node or link failures
* [ ] It only benefits write-heavy workloads

> **Explanation:** More copies mean more places to read from and survive failures, but every write must be propagated and conflicts resolved.

#### Q. What is sharding?

* [ ] Copying the full dataset to every node
* [ ] Caching data only at the network edge
* [ ] Compressing data before it is stored on disk
* [x] Partitioning data so each node holds only a subset

> **Explanation:** Each shard owns a range or hash of keys, so storage and writes scale out. Queries that span many shards become more expensive, so the shard key is chosen to keep common queries local.

#### Q. What makes failure handling fundamentally different in a distributed system than on a single machine?

* [ ] Distributed systems never experience hardware failures
* [x] Partial failure: one node or link can fail while the rest keeps running
* [ ] A failure on any node always stops the whole system immediately
* [ ] Failures can only occur in the network, never in nodes

> **Explanation:** A slow or silent node is hard to distinguish from a crashed one. Systems therefore use timeouts, retries, replication and checkpointing to keep working.

#### Q. How does a typical cluster differ from a general distributed system?

* [x] Its nodes are usually colocated and centrally managed
* [ ] Its machines all share one physical memory
* [ ] It cannot run tightly coupled MPI programs
* [ ] It must consist of identical hardware nodes

> **Explanation:** A cluster is a kind of distributed system. Its low-latency interconnect and shared scheduling make tightly coupled jobs such as MPI simulations practical, and heterogeneous CPU/GPU clusters are common.
