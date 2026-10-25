# Hardware and Architecture Quiz

Notes: [Hardware in Parallel Computing](../notes/06_hardware.md), [GPU Programming](../notes/09_gpu_programming.md)

Check your understanding of CPU and GPU architecture, caches, memory systems and Flynn's taxonomy.

#### Q. Flynn's taxonomy classifies computer architectures by the number of concurrent _____ streams and _____ streams.

* [x] instruction; data
* [ ] input; output
* [ ] program; memory
* [ ] cache; I/O

> **Explanation:** Combining single or multiple instruction streams with single or multiple data streams gives the four classes SISD, SIMD, MISD and MIMD.

#### Q. In Flynn's taxonomy, which class describes a modern multicore PC across its cores?

* [ ] SISD
* [ ] SIMD
* [ ] MISD
* [x] MIMD

> **Explanation:** Each core runs its own instruction stream on its own data. Inside each core, vector instructions also provide SIMD, so real systems often combine classes.

#### Q. Which is a typical example of SIMD execution?

* [ ] Two cores running unrelated programs
* [ ] A cluster of nodes exchanging MPI messages
* [ ] Redundant computers checking each other's results on the same input
* [x] One AVX instruction adding eight pairs of 32-bit floats at once

> **Explanation:** SIMD applies one instruction to multiple data elements. CPU vector extensions such as SSE, AVX and NEON, and vector processors, are classic SIMD hardware.

#### Q. Which class in Flynn's taxonomy has no widely used general-purpose machines?

* [ ] SISD
* [ ] SIMD
* [x] MISD
* [ ] MIMD

> **Explanation:** Redundant fault-tolerant computation is sometimes cited as MISD, but pure MISD hardware is rare. The other classes describe scalar CPUs, vector units and multicore systems or clusters.

#### Q. A two-socket server has many cores per socket, each with AVX-512 vector units, and memory attached to each socket. Which description fits it?

* [x] MIMD across cores, SIMD within each core, and NUMA memory
* [ ] UMA, because all of its cores share one address space
* [ ] SIMD only, because its cores execute vector instructions
* [ ] MISD, because several cores can read the same data

> **Explanation:** These terms describe different dimensions, so one machine can be all three. A shared address space does not make access times uniform: each socket reaches its own memory faster than the other socket's.

#### Q. A Symmetric Multi-Processing (SMP) system has two or more _____ processors connected to a single _____ main memory.

* [ ] identical; distributed
* [ ] dissimilar; shared
* [x] identical; shared
* [ ] dissimilar; distributed

> **Explanation:** In SMP all processors are equal peers with the same view of one shared memory, and any of them can run any thread.

#### Q. A CPU has 8 physical cores, each with 2-way simultaneous multithreading (SMT). How many logical CPUs does the OS see?

* [ ] 4
* [ ] 8
* [x] 16
* [ ] 32

> **Explanation:** Each SMT context appears to the OS as a logical CPU. The 16 contexts still share only 8 cores' execution units and caches.

#### Q. Compared with a processor that has 8 physical cores, a hyperthreaded processor with 8 logical cores (4 physical) usually provides _____ performance.

* [ ] higher
* [x] lower
* [ ] identical
* [ ] double

> **Explanation:** SMT threads share one core's execution units, caches and bandwidth. SMT improves utilisation when one thread stalls, but it does not match a second physical core.

#### Q. A program creates 64 threads on a machine with 4 logical CPUs. What does it get?

* [ ] 64 threads executing instructions at the same instant
* [ ] An error, because threads cannot outnumber logical CPUs
* [ ] A 16-fold speedup over the same program with 4 threads
* [x] 64 concurrent threads, but at most 4 running at any instant

> **Explanation:** The OS time-slices the threads over the 4 hardware contexts. Source-level concurrency does not create hardware parallelism; extra threads can hide blocking I/O but only add switching overhead to CPU-bound work.

#### Q. In modern multicore CPUs, what normally keeps the cores' cached copies of memory consistent?

* [ ] The application, by flushing its caches manually after writes
* [ ] The operating system, during every context switch
* [x] The processor hardware, through a cache-coherence protocol
* [ ] The compiler, by inserting copy instructions

> **Explanation:** Coherence is maintained transparently by hardware, typically with a MESI-style protocol. Software sees one consistent value per cache line without managing copies itself.

#### Q. Under a MESI protocol, cores 0 and 1 both hold a cache line in the Shared state. Core 0 then writes to it. What happens?

* [x] Core 1's copy is invalidated, and core 0's becomes Modified
* [ ] Core 1's copy is updated in place, and both stay Shared
* [ ] The write goes straight to DRAM, and both copies stay Shared
* [ ] Core 0 waits until core 1 evicts the line on its own

> **Explanation:** MESI is a write-invalidate protocol: a core must hold the only valid copy before it writes. Core 1's next access misses and fetches the new value, which is the traffic that false sharing multiplies.

#### Q. Which of these applications would benefit most from parallel execution on multiple cores?

* [ ] A tool that downloads several files from the Internet
* [x] A math library that processes large matrices
* [ ] A logging service that frequently writes to a database
* [ ] A GUI form for an accounting application

> **Explanation:** Large matrix operations are CPU-bound and split naturally into independent pieces. The downloader and logger are mostly I/O-bound, so concurrency helps them more than parallel computation.

#### Q. What is a cache line?

* [x] The fixed-size block, often 64 bytes, that caches transfer
* [ ] The bus that connects the L1 cache to main memory
* [ ] A queue of pending requests waiting for the cache
* [ ] A register that counts cache hits and misses

> **Explanation:** Caches load and track whole lines, not individual bytes. This is why nearby data is cheap to access and why unrelated variables can share a line.

#### Q. With `N = 4096`, the column-order loop below ran about 10 times slower than the row-order loop at `gcc -O2`. Why?

```c
static double a[N][N];
for (i = 0; i < N; i++) for (j = 0; j < N; j++) s += a[i][j];   /* row order */
for (j = 0; j < N; j++) for (i = 0; i < N; i++) s += a[i][j];   /* column order */
```

* [ ] The column-order loop performs more additions in total
* [ ] The compiler runs only the row-order loop on several cores
* [ ] C stores arrays column by column, so the names are swapped
* [x] Each column-order access touches a different cache line

> **Explanation:** C arrays are row-major, so `a[i][j]` and `a[i + 1][j]` are 32 KiB apart. The row loop uses all eight `double`s of each 64-byte line and streams well; the column loop uses one per line fetched.

#### Q. What is false sharing?

* [ ] One thread reading memory that belongs to another process
* [ ] Two threads reading invalid data because of a race
* [x] Threads writing separate variables on the same cache line
* [ ] A security flaw that leaks cache contents across cores

> **Explanation:** The variables are logically independent, but the cache line keeps moving between cores. False sharing can slow a program even when it has no data race.

#### Q. Each worker thread increments its own counter, but the counters sit next to each other in one array and the program scales poorly. What is a likely fix?

* [ ] Protect all the counters with one mutex
* [x] Give each worker's counter its own cache line
* [ ] Declare every counter as `volatile`
* [ ] Add more worker threads to spread the load

> **Explanation:** Adjacent counters share cache lines, so every increment invalidates other cores' copies. Padding them apart, or keeping local counts and combining them at the end, removes that traffic.

#### Q. Does hardware cache coherence make unsynchronized concurrent code correct?

* [ ] Yes, coherence guarantees every thread sees writes in program order
* [x] No, ordering still needs atomics, locks or other synchronization
* [ ] Yes, but only on x86 processors with strong ordering
* [ ] No, because coherence is disabled when several threads run

> **Explanation:** Coherence keeps each cache line consistent, but compilers and CPUs still reorder operations. The language memory model, through synchronization, is what defines correct ordering.

#### Q. How do UMA and NUMA architectures differ?

* [x] UMA access time is uniform; in NUMA it depends on location
* [ ] NUMA systems cannot offer a shared address space
* [ ] UMA is found only in single-core processors
* [ ] NUMA uses faster memory chips than UMA systems

> **Explanation:** In NUMA, memory is attached to particular sockets or nodes, and local accesses are faster than remote ones. Modern multi-socket servers are typically cache-coherent NUMA.

#### Q. On a Linux NUMA server, one thread initialises a large array and then 32 threads on two sockets process it. Why might the threads on one socket run slower?

* [ ] NUMA prevents two sockets from reading the same array
* [ ] The other socket's cores have a lower clock speed
* [x] First-touch placed every page on the initialising thread's node
* [ ] Only one socket at a time can use its L1 caches

> **Explanation:** By default a page lives on the node of the thread that first touches it, so the other socket's accesses are remote. Initialising data in parallel, with the same threads that will use it, keeps accesses local.

#### Q. What is the purpose of a memory barrier (fence)?

* [ ] To protect memory from unauthorised access
* [ ] To flush the entire CPU cache to disk
* [ ] To allocate a new block of aligned memory
* [x] To limit how memory operations may be reordered

> **Explanation:** A fence stops the compiler and CPU from reordering certain loads and stores across it. Atomics with acquire or release ordering provide the same kind of guarantee in a structured way.

#### Q. What is branch prediction?

* [ ] The compiler choosing which branch of an `if` to compile
* [x] The CPU guessing a branch's outcome to keep fetching instructions
* [ ] Predicting which memory addresses a program will access next
* [ ] Running both sides of every branch on different cores

> **Explanation:** A correct guess keeps the pipeline full. A misprediction forces the CPU to discard the speculative work and refill the pipeline, costing many cycles.

#### Q. What is speculative execution?

* [x] Executing instructions before knowing they are needed, undoing wrong guesses
* [ ] A compiler optimisation that deletes code it predicts will be unused
* [ ] A debugging mode that executes every instruction twice to check it
* [ ] Running only the code the programmer marked as optional

> **Explanation:** Combined with branch prediction, speculation lets the CPU work ahead. Only correctly predicted results are committed to the architectural state.

#### Q. What does out-of-order execution do?

* [ ] Executes the program's statements in a random order each run
* [ ] Executes instructions backwards to undo earlier mistakes
* [ ] Reorders the source code once at compile time
* [x] Runs independent instructions as soon as their inputs are ready

> **Explanation:** While one instruction waits, for example on a cache miss, later independent instructions can run. Retiring in order preserves the program's visible behavior on that core.

#### Q. What is instruction-level parallelism (ILP)?

* [ ] Running several programs at once on several cores
* [ ] Splitting one instruction across several cores
* [x] One core executing several independent instructions at once
* [ ] Parallelism between different programming languages

> **Explanation:** Pipelining, superscalar issue and out-of-order execution exploit ILP. Data dependencies between instructions limit how much is available.

#### Q. On a single-threaded workload, CPU A runs at 4 GHz and averages 1 instruction per cycle; CPU B runs at 3 GHz and averages 2. Both execute the same instructions. Which finishes first?

* [x] CPU B, at about 6 billion instructions per second
* [ ] CPU A, at about 4 billion instructions per second
* [ ] Neither, since clock speed and IPC cancel each other out
* [ ] CPU A, since IPC does not affect how long a program runs

> **Explanation:** Instruction throughput is clock rate times IPC: $4 	imes 1 = 4$ versus $3 	imes 2 = 6$ billion per second. Clock speed alone does not predict performance.

#### Q. Compiled with `gcc -O2` (no `-ffast-math`), the second loop below summed the same `double` array about 3.5 times faster than the first on a modern x86 CPU. Why?

```c
for (i = 0; i < n; i++)
    s += a[i];

for (i = 0; i < n; i += 4) {   /* n is a multiple of 4 */
    s0 += a[i]; s1 += a[i + 1]; s2 += a[i + 2]; s3 += a[i + 3];
}
```

* [ ] The second loop uses integer additions instead of floating point
* [ ] The second loop is automatically split across four CPU cores
* [ ] The second loop reads a quarter as much memory as the first
* [x] Four independent dependency chains let the core overlap the additions

> **Explanation:** Each `s += a[i]` waits for the previous addition's result, so the first loop is limited by floating-point add latency. The compiler may not reorder it itself, because floating-point addition is not associative.

#### Q. How do the L1, L2 and L3 caches typically compare?

* [ ] They are the same size but store different kinds of data
* [ ] L1 is shared by all cores, while L3 is private to each core
* [x] L1 is smallest and fastest; L3 is largest and slowest
* [ ] L3 is fastest because it is closest to memory

> **Explanation:** Each level trades speed for capacity, with L2 in between. L1 is usually private to a core, while L3 is often shared by several or all cores.

#### Q. What is a GPU's architecture optimised for?

* [ ] Running operating-system code with low latency
* [x] High throughput on many similar, independent operations
* [ ] Fast execution of single-threaded, branch-heavy programs
* [ ] Managing file systems and storage devices

> **Explanation:** A GPU keeps many lightweight threads in flight and switches between them to hide memory latency. CPUs invest more hardware in making a few threads fast.

#### Q. When can offloading a computation to a GPU make it slower than running it on the CPU?

* [ ] Whenever the CPU has more than eight cores
* [ ] Whenever the data is stored in floating point
* [x] When the problem is small and data transfers dominate
* [ ] When the algorithm is perfectly data-parallel

> **Explanation:** A GPU is not automatically faster. Transfer cost, small problem size, branch divergence and low arithmetic intensity can all remove the benefit.

#### Q. What does GPGPU computing mean?

* [ ] Using a GPU only for rendering games and video
* [x] Using GPUs for general-purpose, non-graphics computation
* [ ] A shading language for writing graphics effects
* [ ] A tool for debugging graphics drivers on Linux

> **Explanation:** Programming models such as CUDA, HIP, OpenCL and SYCL let GPUs run simulations, linear algebra and machine learning workloads.

#### Q. What best describes the difference between latency and throughput?

* [x] Latency is time per operation; throughput is work per unit time
* [ ] Latency matters only for GPUs; throughput only for CPUs
* [ ] Latency is work per second; throughput is the time for one operation
* [ ] Both measure the same thing in different units

> **Explanation:** A system can process many items per second while each item still takes a long time. CPUs lean towards low latency, GPUs towards high throughput.

#### Q. How are data parallelism and SIMD related?

* [ ] They are two names for the same hardware feature
* [ ] Data parallelism requires a GPU, while SIMD requires a cluster
* [ ] SIMD is a workload property; data parallelism is a CPU instruction set
* [x] Data parallelism is a workload property; SIMD is one way to exploit it

> **Explanation:** Applying the same operation to many elements can be executed with SIMD instructions, GPU threads in SIMT groups or multiple cores.

#### Q. What is a vector processor?

* [x] A processor whose instructions operate on whole vectors of elements
* [ ] A processor with many independent, identical cores
* [ ] A processor dedicated to drawing vector graphics on screen
* [ ] A processor that only performs integer arithmetic

> **Explanation:** One vector instruction streams many elements through deep pipelines, so a new element can enter every cycle after start-up. Long vectors use the hardware best.

#### Q. What is cache thrashing?

* [ ] Deliberately clearing the cache to improve security
* [x] Repeated misses as data is evicted before it can be reused
* [ ] Two caches exchanging lines faster than memory can
* [ ] A cache optimisation that prefetches data in bulk

> **Explanation:** Thrashing happens when the working set does not fit, or when access patterns map many addresses to the same cache sets. Performance then approaches that of main memory.

#### Q. How do shared-memory and distributed-memory systems differ?

* [ ] Shared memory is used for small data; distributed memory for large data
* [ ] Shared memory is always faster; distributed memory is always slower
* [x] Shared: one address space; distributed: per-node memory and messages
* [ ] There is no practical difference for programmers

> **Explanation:** Shared memory lets threads communicate through loads and stores. Distributed memory scales further but requires explicit message passing, for example with MPI.

#### Q. What is memory bandwidth?

* [ ] The total amount of installed memory
* [ ] The number of memory addresses a program may use
* [ ] The delay before the first byte of a request arrives
* [x] The rate at which data can be read from or written to memory

> **Explanation:** Bandwidth is measured in bytes per second and limits data-heavy workloads. The delay for one access is latency, a separate property.

#### Q. A memory-bound loop already saturates DRAM bandwidth with 8 cores. What usually happens if you run it on 16 cores?

* [x] It barely speeds up, since bandwidth is already saturated
* [ ] It becomes about twice as fast, like compute-bound code
* [ ] It always slows down to exactly half its speed
* [ ] It becomes compute-bound and scales linearly

> **Explanation:** All cores share finite memory bandwidth. Once it is saturated, extra cores add compute capacity that the loop cannot use.

#### Q. A compute-bound job whose threads share no data speeds up only about 6.5 times going from 1 to 8 cores of a desktop CPU. Which hardware effect is a plausible cause?

* [ ] The cache line size shrinks as more cores become active
* [ ] SIMD instructions are disabled when more than one core is active
* [x] The cores run at a lower clock frequency when all of them are busy
* [ ] The CPU allows only one core at a time to execute arithmetic

> **Explanation:** Turbo frequencies are highest with few active cores; power and thermal limits lower clocks when all are busy. Shared caches, memory bandwidth and SMT siblings are other common causes of sublinear scaling.

#### Q. What is hardware prefetching?

* [ ] Loading a program into memory before it is started
* [ ] Compiling frequently used functions ahead of time
* [ ] Reordering data in memory so it is sorted
* [x] Loading data into cache before the program requests it

> **Explanation:** Prefetchers recognise patterns such as sequential or strided access. Predictable access patterns therefore run much faster than random ones.

#### Q. What is the "memory wall"?

* [ ] A physical limit on how much RAM a single machine can hold
* [x] The growing gap between processor speed and memory access speed
* [ ] A hardware barrier that isolates each process's memory from others
* [ ] A cache that blocks writes from reaching memory

> **Explanation:** Processors improved faster than DRAM latency for decades, so many programs spend much of their time waiting for memory. Caches, prefetching and locality-aware code help.

#### Q. With a write-back cache policy, when is modified data written to main memory?

* [ ] Immediately, as part of every store
* [x] When the dirty line is evicted or flushed
* [ ] Only when the owning program exits
* [ ] Never, because the cache replaces main memory

> **Explanation:** Write-back avoids writing to memory on every store, saving bandwidth. A write-through cache, by contrast, updates memory on every write.
