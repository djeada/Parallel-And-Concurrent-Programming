# Hardware and Architecture Quiz

Test your understanding of parallel computing hardware, CPU architecture, and memory systems.

---

#### Q. A Symmetric Multi-Processing (SMP) system has two or more _____ processors connected to a single _____ main memory.

- [x] identical; shared
- [ ] dissimilar; shared
- [ ] dissimilar; distributed
- [ ] identical; distributed
- [ ] asynchronous; shared

#### Q. A hyperthreaded processor with 8 logical cores will usually provide _____ performance compared to a regular processor with 8 physical cores.

- [x] lower
- [ ] higher
- [ ] equivalent
- [ ] variable

#### Q. In most modern multi-core CPUs, cache coherency is usually handled by the _____.

- [ ] user
- [ ] application software
- [ ] operating system
- [x] processor hardware
- [ ] compiler

#### Q. Which of these applications would benefit the most from parallel execution?

- [x] tool for downloading multiple files from the Internet at the same time
- [ ] math library for processing large matrices
- [ ] system logging application that frequently writes to a database
- [ ] graphical user interface (GUI) for an accounting application
- [ ] real-time data streaming service

#### Q. What is cache coherency?

- [ ] A measure of cache speed
- [x] Ensuring that all caches in a multiprocessor system have consistent copies of shared data
- [ ] The size of the cache
- [ ] The organization of cache memory
- [ ] A cache optimization technique

#### Q. What is the purpose of a cache line?

- [ ] To connect cache to main memory
- [ ] To debug cache operations
- [x] The basic unit of data transfer between cache and main memory
- [ ] To store cache metadata
- [ ] To synchronize cache access

#### Q. What is "false sharing" in multi-core systems?

- [ ] Sharing invalid data between cores
- [x] Performance degradation when threads modify different variables that happen to be on the same cache line
- [ ] Incorrect data sharing due to bugs
- [ ] Sharing memory that shouldn't be shared
- [ ] A security vulnerability

#### Q. What is the difference between NUMA and UMA architectures?

- [ ] NUMA is newer than UMA
- [x] In UMA, memory access time is uniform; in NUMA, it varies depending on memory location relative to processor
- [ ] NUMA uses less memory
- [ ] UMA is for single processors only
- [ ] There is no significant difference

#### Q. What is the purpose of a memory barrier (memory fence)?

- [ ] To protect memory from unauthorized access
- [ ] To allocate memory
- [x] To ensure memory operations complete in a specific order
- [ ] To compress memory
- [ ] To debug memory issues

#### Q. What is "branch prediction" in modern CPUs?

- [ ] Predicting which branch of code to compile
- [x] CPU technique to guess which way a branch will go to continue executing without waiting
- [ ] A debugging technique
- [ ] Predicting memory access patterns
- [ ] A parallel execution technique

#### Q. What is SIMD (Single Instruction, Multiple Data)?

- [ ] A programming language
- [x] A parallel processing technique where one instruction operates on multiple data elements simultaneously
- [ ] A type of memory architecture
- [ ] A network protocol
- [ ] A debugging tool

#### Q. What is the difference between L1, L2, and L3 caches?

- [ ] They store different types of data
- [x] L1 is smallest and fastest; L2 is larger and slower; L3 is largest and slowest among the three
- [ ] Only L1 is used for data; others are for instructions
- [ ] They are in different physical locations
- [ ] L3 is private; L1 and L2 are shared

#### Q. What is hyperthreading (simultaneous multithreading)?

- [ ] Running threads at hyper speed
- [x] A technique that allows a single CPU core to execute multiple threads by sharing core resources
- [ ] A type of multi-core processor
- [ ] Threading across multiple computers
- [ ] A debugging technique

#### Q. What is the memory hierarchy in modern computers?

- [ ] A management structure for memory allocation
- [x] The organization of memory from fastest/smallest (registers, cache) to slowest/largest (RAM, disk)
- [ ] A debugging tool for memory
- [ ] A way to organize data in memory
- [ ] A security feature

#### Q. What is "speculative execution" in modern CPUs?

- [ ] Executing code that might not be needed
- [x] CPU technique of executing instructions before knowing if they're needed, rolling back if wrong
- [ ] A debugging mode
- [ ] Parallel execution of speculative code paths
- [ ] A compiler optimization

#### Q. What is the purpose of a TLB (Translation Lookaside Buffer)?

- [ ] To translate code between languages
- [ ] To buffer network translations
- [x] To cache virtual-to-physical address translations for faster memory access
- [ ] To translate data formats
- [ ] To store translation logs

#### Q. What is a GPU (Graphics Processing Unit) architecture optimized for?

- [ ] Running operating system code
- [ ] Managing file systems
- [x] Parallel processing of many similar operations (high throughput)
- [ ] Single-threaded performance
- [ ] Memory management

#### Q. What is "cache thrashing"?

- [ ] Deliberately destroying cache data
- [x] Repeated cache misses when data is constantly evicted and reloaded
- [ ] A cache optimization technique
- [ ] An attack on cache memory
- [ ] A debugging process

#### Q. What is the difference between shared and distributed memory systems?

- [ ] Shared is faster; distributed is slower
- [x] In shared memory, all processors access the same memory; in distributed, each has its own memory
- [ ] Shared is for small systems; distributed is for large
- [ ] They are the same concept
- [ ] Shared uses more power

#### Q. What is "memory bandwidth"?

- [ ] The width of memory chips
- [x] The rate at which data can be read from or written to memory
- [ ] The range of memory addresses
- [ ] The number of memory channels
- [ ] The size of memory

#### Q. What is a "memory-bound" workload?

- [ ] A workload limited by memory size
- [x] A workload whose performance is limited by memory bandwidth or latency, not CPU speed
- [ ] A workload that uses all memory
- [ ] A workload that cannot use cache
- [ ] A workload that is restricted to certain memory regions

#### Q. What is the purpose of prefetching in modern CPUs?

- [ ] To fetch data before the program starts
- [x] To load data into cache before it's actually needed to reduce wait time
- [ ] To prefetch instructions for compilation
- [ ] To organize data in memory
- [ ] To debug memory access

#### Q. What is "Amdahl's number" or the sequential fraction in parallel computing?

- [ ] A random number used in algorithms
- [x] The fraction of a program that cannot be parallelized
- [ ] The number of processors needed
- [ ] The maximum speedup possible
- [ ] The efficiency rating

#### Q. What is the difference between physical and logical cores?

- [ ] Physical cores are real hardware; logical are software simulations
- [x] Physical cores are actual hardware units; logical cores are the threads a physical core can run (e.g., with hyperthreading)
- [ ] Logical cores are faster
- [ ] Physical cores handle data; logical handle instructions
- [ ] There is no difference

#### Q. What is a "compute-bound" workload?

- [ ] A workload bound to a specific computer
- [x] A workload whose performance is limited by CPU speed, not memory or I/O
- [ ] A workload that cannot be parallelized
- [ ] A workload that uses extensive computation
- [ ] A workload limited by network speed

#### Q. What is "instruction-level parallelism" (ILP)?

- [ ] Parallel execution of programs
- [x] CPU technique of executing multiple instructions simultaneously within a single core
- [ ] Parallelism between different programming languages
- [ ] A compiler optimization
- [ ] Parallel instruction decoding

#### Q. What is a "vector processor"?

- [ ] A processor that handles graphics
- [x] A processor designed to perform the same operation on multiple data elements simultaneously
- [ ] A processor for mathematical calculations only
- [ ] A processor with multiple cores
- [ ] A processor for vector graphics

#### Q. What is "out-of-order execution"?

- [ ] Executing code in random order
- [x] CPU technique of executing instructions in an order different from the program order to maximize efficiency
- [ ] Executing code without compilation
- [ ] A bug in program execution
- [ ] Executing instructions backwards

#### Q. What is the "memory wall" problem?

- [ ] A physical limit on memory size
- [x] The growing disparity between CPU speed and memory access speed
- [ ] A security barrier for memory
- [ ] A debugging challenge
- [ ] A memory corruption issue

#### Q. What is "cache associativity"?

- [ ] Linking caches together
- [x] The number of cache locations where a memory block can be placed
- [ ] The speed of cache access
- [ ] The association between cache levels
- [ ] Cache sharing between cores

#### Q. What is a "write-back" cache policy?

- [ ] Writing data back to the program
- [x] Writing modified data to main memory only when the cache line is evicted
- [ ] Undoing write operations
- [ ] A backup mechanism
- [ ] Writing data immediately to disk

#### Q. What is "write-through" cache policy?

- [ ] Writing data through multiple caches
- [x] Writing data to both cache and main memory simultaneously
- [ ] A faster writing technique
- [ ] Writing through a buffer
- [ ] A debugging mode

#### Q. What is the "von Neumann bottleneck"?

- [ ] A bottleneck in the CPU
- [x] The limitation of having a single path between CPU and memory for both instructions and data
- [ ] A software design problem
- [ ] A network bottleneck
- [ ] A memory allocation issue

#### Q. What is "GPGPU" computing?

- [ ] Graphics processing for games
- [x] Using graphics processing units for general-purpose computing tasks
- [ ] A graphics programming language
- [ ] A type of GPU architecture
- [ ] Graphics debugging
