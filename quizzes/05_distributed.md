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

* [ ] One task must send the same data to every other task in the job
* [ ] Every task must combine its value into one global sum at the end
* [x] A small number of specific tasks need to exchange data with each other
* [ ] Every task must send distinct data to every other task

> **Explanation:** Point-to-point operations such as `MPI_Send` and `MPI_Recv` connect one sender with one receiver. Patterns involving the whole group are better expressed with collectives.

#### Q. In MPI, what is a rank?

* [ ] The operating-system PID of an MPI process
* [ ] The scheduling priority MPI gives a process
* [x] A process's index within a communicator, starting at 0
* [ ] The number of the CPU core a process runs on

> **Explanation:** Ranks run from 0 to size minus 1 and are assigned per communicator, so the same process can have different ranks in different communicators. They are not OS process IDs.

#### Q. What is `MPI_COMM_WORLD`?

* [ ] A communicator for talking to programs running outside the MPI job
* [x] The predefined communicator containing all processes started in the job
* [ ] A function that creates new processes while the job is running
* [ ] A global variable shared by every process

> **Explanation:** After `MPI_Init`, every process belongs to `MPI_COMM_WORLD`. Programs can derive smaller communicators from it for subgroups.

#### Q. What does an MPI communicator combine?

* [ ] A network socket and a buffer for each message
* [x] A process group and a context that isolates its messages
* [ ] A process and the thread that sends its messages
* [ ] A send buffer and a matching receive buffer

> **Explanation:** The context guarantees that messages sent in one communicator cannot be received in another, which is why libraries often use their own duplicated communicator.

#### Q. What does `MPI_Comm_split(MPI_COMM_WORLD, rank / 4, rank, &new_comm)` do?

* [ ] Splits each message sent on it into 4-byte chunks
* [x] Creates sub-communicators of 4 consecutive ranks each
* [ ] Duplicates `MPI_COMM_WORLD` four times
* [ ] Removes every fourth rank from `MPI_COMM_WORLD`

> **Explanation:** Processes with the same color end up in the same new communicator, ordered by the key. Here ranks 0-3 form one group, 4-7 the next, and so on.

#### Q. An MPI program calls `MPI_Comm_rank` and `MPI_Comm_size` on `MPI_COMM_WORLD` and is launched with `mpirun -np 4`. What values do they report?

* [ ] Ranks 1 to 4, and size 4
* [x] Ranks 0 to 3, and size 4
* [ ] Ranks 0 to 4, and size 5
* [ ] Every process reports rank 0, and size 4

> **Explanation:** `MPI_Comm_size` gives the number of processes in the communicator. Each process gets a distinct rank starting from 0.

#### Q. What is the purpose of `MPI_Finalize()`?

* [ ] To send any final messages to the root process
* [ ] To combine the final results from all processes
* [x] To clean up the MPI environment after all MPI work is complete
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
* [x] Once `MPI_Wait` or `MPI_Test` reports completion
* [ ] After the receiving process calls `MPI_Finalize`
* [ ] Never, because the buffer belongs to MPI from then on

> **Explanation:** A nonblocking send only starts the operation. Until the request completes, MPI may still be reading the buffer, so changing it could corrupt the message.

#### Q. How does `MPI_Test` differ from `MPI_Wait`?

* [ ] `MPI_Test` sends a test message; `MPI_Wait` sends real data
* [x] `MPI_Test` checks without blocking; `MPI_Wait` blocks until done
* [ ] `MPI_Test` works only for sends; `MPI_Wait` only for receives
* [ ] `MPI_Test` cancels the operation if it is not finished

> **Explanation:** `MPI_Test` sets a flag and returns immediately, so a process can do other work and check again later. `MPI_Wait` is the blocking choice.

#### Q. When does a synchronous-mode send, `MPI_Ssend`, complete?

* [ ] As soon as the data is copied into a system buffer
* [x] Only after the matching receive has started
* [ ] Only after the receiver has finished processing the data
* [ ] Immediately, like a nonblocking send

> **Explanation:** `MPI_Ssend` never completes by buffering alone. Testing code with it reveals programs that only work because the implementation happens to buffer messages.

#### Q. Two ranks each call `MPI_Send` to the other with a large message, then `MPI_Recv`. What can happen, and what is a standard fix?

* [ ] Nothing can go wrong, because MPI always buffers sends
* [x] Both may wait forever; use `MPI_Sendrecv` instead
* [ ] The messages are swapped automatically, so the code is safe
* [ ] MPI reports an error, so the ranks must use `MPI_Bcast` instead

> **Explanation:** Large messages often are not buffered, so each `MPI_Send` waits for the other's receive, creating a cycle. `MPI_Sendrecv` lets MPI handle both directions without deadlock; ordering the calls oppositely on the two ranks also works.

#### Q. What does `MPI_Barrier(comm)` guarantee?

* [ ] All pending messages in the communicator are delivered
* [x] No process leaves it until every process has entered it
* [ ] Only one process at a time can send messages afterwards
* [ ] All processes print their output in rank order

> **Explanation:** A barrier is an explicit synchronization point. It says nothing about message delivery and should not be used to patch an unclear communication design.

#### Q. Can a collective such as `MPI_Bcast` be relied on as a substitute for `MPI_Barrier`?

* [ ] Yes, every collective synchronizes all processes like a barrier
* [x] No, it may return before other processes have entered it
* [ ] Yes, as long as the root process is rank 0
* [ ] No, because collectives can be called by only some processes

> **Explanation:** Every process in the communicator must make the matching call, but only `MPI_Barrier` guarantees that all have arrived. A broadcast root, for example, may return as soon as its buffer can be reused.

#### Q. What is collective communication in MPI?

* [ ] A message exchanged between exactly two processes
* [x] An operation every process in the communicator must call
* [ ] Communication that uses shared memory instead of messages
* [ ] Messages exchanged between two separate MPI jobs

> **Explanation:** Collectives express group patterns directly, which makes intent clear and lets the implementation use efficient tree or ring algorithms.

#### Q. What does `MPI_Bcast()` do?

* [ ] Broadcasts a message to every computer on the network
* [x] Copies the root's data to every process in the communicator
* [ ] Sends a different piece of data to each process
* [ ] Collects one value from every process at the root

> **Explanation:** After the call, every process has a copy of the root's buffer. Scattering different pieces is `MPI_Scatter`.

#### Q. Four ranks hold the values 4, 7, 2 and 9 and call `MPI_Reduce` with `MPI_SUM` and root 0. What is the result?

* [x] Rank 0 receives 22
* [ ] Every rank receives 22
* [ ] Rank 0 receives 4, its own value
* [ ] Each rank receives the sum of the values before it

> **Explanation:** `MPI_Reduce` combines the values with the given operation and stores the result only at the root. `MPI_Allreduce` would give 22 to every rank.

#### Q. How does `MPI_Allreduce` differ from `MPI_Reduce`?

* [ ] It works on all MPI data types, not only on integers
* [x] Every process receives the reduced result, not just the root
* [ ] It is nonblocking, while `MPI_Reduce` is blocking
* [ ] It reduces the data from every communicator in the job at once

> **Explanation:** All-reduce is common when every process needs the global value, such as a convergence norm in a simulation or summed gradients in distributed training.

#### Q. What is the difference between `MPI_Scatter` and `MPI_Gather`?

* [ ] Scatter is meant for small messages, while Gather is for large ones
* [x] Scatter splits root data among processes; Gather collects pieces at the root
* [ ] Scatter is synchronous; Gather is asynchronous and returns early
* [ ] Scatter sends the same data everywhere; Gather sums the data

> **Explanation:** They are inverse patterns. A typical program scatters chunks of an array, lets each process work on its chunk, then gathers the results.

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

> **Explanation:** `MPI_Scan` is an inclusive prefix reduction: rank i receives the combination of values from ranks 0 to i. Rank 2 gets 1 + 2 + 3 = 6.

#### Q. What are message tags used for in MPI?

* [ ] Labelling messages so debuggers can display them
* [x] Distinguishing kinds of messages between the same processes
* [ ] Selecting how the message's header is compressed in transit
* [ ] Choosing the key used to encrypt the message

> **Explanation:** A receive matches on communicator, source and tag. Distinct tags let a process receive, for example, data messages and control messages separately.

#### Q. Rank 0 receives from ranks 1 and 2 with `MPI_ANY_SOURCE`. Why might the output differ between runs?

* [ ] `MPI_ANY_SOURCE` drops one of the messages at random
* [x] Messages from different senders can arrive in either order
* [ ] MPI reorders the messages from each sender
* [ ] `MPI_ANY_SOURCE` only works with blocking sends

> **Explanation:** Arrival order across senders depends on timing. Naming the expected source rank, and a specific tag, removes the ambiguity when the program needs a fixed order.

#### Q. Rank 1 sends two messages to rank 0 with the same tag and communicator. In what order can rank 0 receive them?

* [x] In the order they were sent
* [ ] In any order, depending on network timing
* [ ] Only the second one, which overwrites the first
* [ ] In order of message size, smallest first

> **Explanation:** MPI guarantees that matching messages between the same sender and receiver do not overtake each other. It gives no ordering guarantee across different senders.

#### Q. Why might a process call `MPI_Probe` before `MPI_Recv`?

* [ ] To check whether the network link is working
* [x] To learn an incoming message's size before allocating a buffer
* [ ] To test whether the sending process is still alive
* [ ] To measure the round-trip network latency to the sender

> **Explanation:** `MPI_Probe` returns a status describing a matching message without receiving it. `MPI_Get_count` on that status gives its size, so the buffer can be allocated exactly.

#### Q. A receive specifies a count of 100 elements, but the matching message has 60. What happens?

* [ ] MPI reports an error because the counts differ
* [x] It succeeds, and `MPI_Get_count` reports 60
* [ ] The remaining 40 elements are filled with zeros
* [ ] The receive waits for 40 more elements

> **Explanation:** The receive count is a capacity, so it can exceed the message size. Receiving a message larger than the buffer is the error case.

#### Q. What is a Cartesian topology in MPI?

* [ ] The physical layout of cables in a cluster
* [x] A virtual grid of processes, in 1D, 2D or 3D
* [ ] A row-major memory layout for storing matrices
* [ ] A visualisation produced by MPI debugging tools

> **Explanation:** `MPI_Cart_create` builds the grid and `MPI_Cart_shift` finds neighbor ranks, which suits grid-based simulations; dimensions can be periodic. Virtual topologies do not change where processes physically run.

#### Q. Why use an MPI derived datatype such as `MPI_Type_vector` to send a matrix column?

* [ ] It compresses the column's data before sending it
* [x] It describes the strided layout, so no manual packing is needed
* [ ] It transposes the column into a row on the receiving side
* [ ] It is the only way MPI can send floating-point data

> **Explanation:** In a row-major matrix, a column's elements are not contiguous. A vector type with the right stride lets MPI read them directly.

#### Q. What are ghost (halo) cells in a domain-decomposed simulation?

* [ ] Cells that store data deleted during the simulation
* [x] Local copies of neighbors' boundary values
* [ ] Unused cells added to pad arrays to a power of two
* [ ] Cells that record errors detected during communication

> **Explanation:** A stencil near the edge of a subdomain needs neighbor values. Each step, processes exchange their boundary data to refresh these halo copies.

#### Q. What is latency hiding in distributed computing?

* [ ] Hiding network delays from users with a progress bar
* [x] Overlapping communication with independent computation
* [ ] Compressing messages so they arrive sooner
* [ ] Caching every remote value locally in advance

> **Explanation:** A process can post nonblocking sends and receives, compute work that does not depend on them, and only then wait. Actual overlap depends on the MPI implementation making progress.

#### Q. What does the CAP theorem say about distributed data stores?

* [ ] They can guarantee consistency, availability and partition tolerance at all times
* [x] During a network partition, a system must choose between consistency and availability
* [ ] Caching, availability and performance cannot be optimised together
* [ ] Replication always reduces both consistency and availability

> **Explanation:** Partitions cannot be ruled out in real networks. While one lasts, a node must either refuse requests (keeping consistency) or answer with possibly stale data (keeping availability).

#### Q. What is eventual consistency?

* [ ] Every read always returns the latest write
* [x] If updates stop, all replicas eventually converge to the same value
* [ ] Data becomes consistent only after the whole system restarts
* [ ] Writes are applied to all replicas in one fixed global order

> **Explanation:** Replicas may temporarily disagree, so a read can return stale data. Many highly available systems accept this in exchange for lower latency and partition tolerance.

#### Q. What is a primary trade-off of replicating data across nodes?

* [ ] It always reduces the latency of every single write
* [x] It improves availability and reads but makes consistency costlier
* [ ] It removes any need to handle node or link failures
* [ ] It only benefits write-heavy workloads

> **Explanation:** More copies mean more places to read from and survive failures, but every write must be propagated and conflicts resolved.

#### Q. What is sharding?

* [ ] Copying the full dataset to every node
* [x] Partitioning data so each node holds only a subset
* [ ] Compressing data before it is stored on disk
* [ ] Caching data only at the network edge

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
