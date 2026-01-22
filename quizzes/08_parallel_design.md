# Parallel Design Quiz

Test your understanding of parallel algorithm design, decomposition strategies, and programming patterns.

---

#### Q. Which stage of the parallel design process focuses on combining tasks and replicating data or computation as needed to increase program efficiency?

- [ ] mapping
- [x] agglomeration
- [ ] communication
- [ ] partitioning
- [ ] synchronization

#### Q. Why does the mapping design stage not apply to applications written for common desktop operating systems?

- [x] The operating system automatically handles scheduling threads to execute on each processor core.
- [ ] Desktop computers do not have enough processor cores for mapping to be a concern.
- [ ] Most desktop computers only have one processor core.
- [ ] The mapping design stage is not necessary for modern applications.
- [ ] Mapping is handled by the application itself.

#### Q. Why does the partitioning design stage occur before the communication stage?

- [ ] You need to group the tasks together before planning the most effective communication strategy between them.
- [x] You need to know how the problem will be divided in order to assess the communication needs between individual tasks.
- [ ] You need to know where each of the tasks will physically execute before deciding on a communication strategy.
- [ ] The "order" of the four design stages is arbitrary and does not really matter.
- [ ] Communication requirements determine how tasks are grouped.

#### Q. Which stage of the parallel design process focuses on breaking the problem down into discrete pieces of work?

- [ ] mapping
- [ ] agglomeration
- [ ] communication
- [x] partitioning
- [ ] synchronization

#### Q. Which stage of the parallel design process focuses on coordinating task execution and how they share information?

- [ ] mapping
- [ ] agglomeration
- [ ] partitioning
- [x] communication
- [ ] synchronization

#### Q. Which decomposition strategy divides work based on the data each task processes?

- [ ] functional decomposition
- [x] data decomposition
- [ ] recursive decomposition
- [ ] speculative decomposition
- [ ] pipeline decomposition

#### Q. Which stage of the parallel design process focuses on specifying where each task will execute?

- [ ] communication
- [x] mapping
- [ ] agglomeration
- [ ] partitioning
- [ ] load balancing

#### Q. What does a divide-and-conquer algorithm do when it reaches the base case?

- [x] Stop subdividing the current problem and solve it.
- [ ] Recursively solve a set of smaller subproblems.
- [ ] Solve all of the subproblems that have been created.
- [ ] Divide the problem into two smaller subproblems.
- [ ] Return the initial problem unchanged.

#### Q. What is the difference between data parallelism and task parallelism?

- [ ] Data parallelism is faster
- [x] Data parallelism applies the same operation to different data; task parallelism applies different operations simultaneously
- [ ] Task parallelism uses more memory
- [ ] They are the same concept
- [ ] Data parallelism is only for databases

#### Q. What is "embarrassingly parallel" computation?

- [ ] A computation that is too simple
- [x] A computation that can be easily divided into independent tasks with little or no communication needed
- [ ] A poorly designed parallel algorithm
- [ ] A computation that embarrasses other algorithms
- [ ] A computation with many bugs

#### Q. What is "functional decomposition" in parallel design?

- [ ] Breaking functions into smaller functions
- [x] Dividing a problem based on the different operations (functions) that need to be performed
- [ ] Decomposing functional requirements
- [ ] A debugging technique
- [ ] Removing unnecessary functions

#### Q. What is "domain decomposition" in parallel design?

- [ ] Splitting a program into different domains
- [x] Dividing a problem based on the data domain, distributing data among processors
- [ ] Separating the problem domain from the solution domain
- [ ] A network partitioning technique
- [ ] Decomposing function domains

#### Q. What is the "master-worker" (or "master-slave") pattern in parallel programming?

- [ ] A debugging pattern
- [x] A pattern where one process (master) distributes work to multiple worker processes and collects results
- [ ] A pattern for handling errors
- [ ] A network communication pattern
- [ ] A memory management pattern

#### Q. What is the "pipeline" pattern in parallel programming?

- [ ] A pattern for data storage
- [x] A pattern where tasks are arranged in stages, with output of one stage becoming input of the next
- [ ] A pattern for network communication
- [ ] A debugging pattern
- [ ] A memory access pattern

#### Q. What is "geometric decomposition"?

- [ ] A graphics algorithm
- [x] Partitioning a problem's data domain into regular geometric shapes (like tiles or blocks)
- [ ] A pattern recognition technique
- [ ] A compression algorithm
- [ ] A mathematical decomposition method

#### Q. What is the "fork-join" model?

- [ ] A model for database operations
- [x] A pattern where a task forks into multiple parallel subtasks, which later join back
- [ ] A model for file operations
- [ ] A network connection pattern
- [ ] A memory allocation model

#### Q. What is "recursive decomposition"?

- [ ] A mathematical technique
- [x] A decomposition strategy where a problem is repeatedly divided into subproblems (like in divide-and-conquer)
- [ ] A pattern for recursive functions
- [ ] A debugging technique
- [ ] A memory management strategy

#### Q. What is the "SPMD" (Single Program Multiple Data) model?

- [ ] A database model
- [x] A programming model where all processors run the same program but on different data
- [ ] A graphics processing model
- [ ] A network protocol
- [ ] A memory model

#### Q. What is "speculative parallelism"?

- [ ] Untested parallel algorithms
- [x] Executing tasks in parallel before knowing if they will be needed, discarding results if not
- [ ] Random parallel execution
- [ ] Debugging parallel code
- [ ] Hypothetical parallelism

#### Q. What is the "bag of tasks" or "work queue" pattern?

- [ ] A storage pattern
- [x] A pattern where a pool of workers pulls tasks from a shared queue, enabling dynamic load balancing
- [ ] A debugging technique
- [ ] A network pattern
- [ ] A memory allocation pattern

#### Q. What are the four stages of Foster's parallel design methodology (PCAM)?

- [ ] Plan, Code, Analyze, Monitor
- [x] Partitioning, Communication, Agglomeration, Mapping
- [ ] Prepare, Compute, Aggregate, Merge
- [ ] Problem, Compute, Analyze, Measure
- [ ] Partition, Combine, Assign, Map

#### Q. What is "loop parallelization"?

- [ ] Making loops run faster
- [x] Converting sequential loop iterations to run in parallel across multiple processors
- [ ] Unrolling loops
- [ ] Optimizing loop conditions
- [ ] Removing loops from code

#### Q. What is a "stencil" pattern in parallel computing?

- [ ] A template for parallel programs
- [x] A pattern where each element's new value depends on a fixed pattern of neighboring elements
- [ ] A debugging tool
- [ ] A type of load balancing
- [ ] A communication protocol

#### Q. What is "reduction" in parallel computing?

- [ ] Making code smaller
- [x] Combining multiple values into a single value using an operation (like sum or max)
- [ ] Reducing memory usage
- [ ] Simplifying parallel code
- [ ] Reducing the number of processors

#### Q. What is the importance of "data locality" in parallel design?

- [ ] Keeping data in local files
- [x] Ensuring processors work on data that's close to them in memory to reduce communication overhead
- [ ] Keeping data in local variables
- [ ] A security measure
- [ ] A debugging technique

#### Q. What is a "prefix sum" (or "scan") operation?

- [ ] Checking strings for prefixes
- [x] An operation that computes all prefix sums/products of a sequence (running totals)
- [ ] A string search algorithm
- [ ] A sorting operation
- [ ] A memory allocation technique

#### Q. What is "task stealing" or "work stealing"?

- [ ] A security vulnerability
- [x] A load balancing technique where idle processors steal tasks from busy processors' queues
- [ ] Removing tasks from the queue
- [ ] A type of race condition
- [ ] A debugging technique

#### Q. What is "static vs dynamic load balancing"?

- [ ] Static is for static data; dynamic is for changing data
- [x] Static assigns work upfront; dynamic adjusts work distribution during runtime
- [ ] Static is faster; dynamic is slower
- [ ] They are the same concept
- [ ] Static uses more memory

#### Q. What is "data partitioning"?

- [ ] Dividing data for backup
- [x] Dividing data among processors so each works on a subset
- [ ] Creating data partitions on disk
- [ ] Separating data from code
- [ ] A security technique

#### Q. What is "owner computes" rule?

- [ ] The processor that owns code executes it
- [x] The processor that owns a piece of data is responsible for computing its values
- [ ] A licensing rule
- [ ] A security policy
- [ ] A debugging principle

#### Q. What is a "dependency graph" in parallel computing?

- [ ] A graph of software dependencies
- [x] A graph showing which tasks depend on others, determining what can run in parallel
- [ ] A debugging visualization
- [ ] A network topology
- [ ] A memory layout diagram

#### Q. What is "communication to computation ratio"?

- [ ] The ratio of network to CPU usage
- [x] The ratio of time spent communicating to time spent computing, indicating parallel efficiency
- [ ] The number of messages per computation
- [ ] A hardware specification
- [ ] A debugging metric

#### Q. What is "block decomposition"?

- [ ] Breaking code into blocks
- [x] Dividing data into contiguous blocks assigned to different processors
- [ ] A memory management technique
- [ ] A compilation strategy
- [ ] A debugging technique

#### Q. What is "cyclic decomposition"?

- [ ] Repeating decomposition
- [x] Distributing data in a round-robin fashion among processors
- [ ] A circular data structure
- [ ] A scheduling algorithm
- [ ] A memory recycling technique

#### Q. What is the "bulk synchronous parallel" (BSP) model?

- [ ] A model for bulk data processing
- [x] A model where computation proceeds in supersteps with computation, communication, and synchronization phases
- [ ] A synchronous communication protocol
- [ ] A hardware architecture
- [ ] A debugging framework

#### Q. What is "coarsening" in parallel algorithm design?

- [ ] Making algorithms simpler
- [x] Combining fine-grained tasks into larger tasks to reduce overhead
- [ ] Increasing granularity of data
- [ ] A debugging technique
- [ ] Reducing precision of calculations
