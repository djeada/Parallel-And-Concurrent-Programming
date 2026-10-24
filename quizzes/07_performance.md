# Performance Evaluation Quiz

Notes: [Evaluating Performance](../notes/07_evaluating_performance.md), [Designing Parallel Programs](../notes/08_designing_parallel_programs.md)

Check your understanding of speedup, efficiency, scaling laws, work and span, and how to measure parallel programs.

#### Q. Which of these describes a program's latency?

* [ ] The ratio of sequential to parallel execution time
* [x] The time one task takes to complete
* [ ] The number of tasks completed per unit of time
* [ ] The number of processors the program uses

> **Explanation:** Latency is measured per operation, from start to finish. Percentiles such as p95 and p99 are often reported because an average can hide slow outliers.

#### Q. Which of these describes a program's throughput?

* [x] The number of tasks completed per unit of time
* [ ] The ratio of sequential to parallel execution time
* [ ] The time one task takes to complete
* [ ] The number of processors the program uses

> **Explanation:** Throughput counts completed work per second, such as requests or images. A system can raise throughput without lowering the latency of any single task.

#### Q. Which of these describes a program's speedup?

* [ ] The time one task takes to complete
* [x] One-processor time divided by p-processor time
* [ ] The number of tasks completed per unit of time
* [ ] The number of processors the program uses

> **Explanation:** Speedup is `S_p = T_1 / T_p`, based on wall-clock time for the same problem. A speedup of p is linear, or ideal, speedup.

#### Q. 85% of a program's runtime is parallelizable and runs 6 times faster on a 6-core processor. According to Amdahl's Law, what is the overall speedup?

* [x] About 3.43
* [ ] About 4.62
* [ ] About 5.10
* [ ] Exactly 6.00

> **Explanation:** `S = 1 / ((1 - 0.85) + 0.85/6) = 1 / (0.15 + 0.142) ≈ 3.43`. The 15% serial portion keeps the result well below the core count.

#### Q. Amdahl's Law gives a(n) _____ on the overall speedup from parallelizing a program.

* [ ] lower limit
* [x] upper limit
* [ ] average value
* [ ] exact prediction

> **Explanation:** The model assumes the parallel part divides perfectly and ignores communication, synchronization and imbalance. Real programs therefore do no better than it predicts, and usually worse.

#### Q. If 95% of a fixed-size program can be parallelized, what is the maximum speedup with unlimited processors, according to Amdahl's Law?

* [ ] 5
* [ ] 19
* [x] 20
* [ ] 95

> **Explanation:** As the processor count grows, the speedup approaches `1 / (1 - P) = 1 / 0.05 = 20`. The 5% serial part takes the same time no matter how many processors are added.

#### Q. How does Gustafson's Law differ from Amdahl's Law?

* [ ] It applies only to shared-memory machines
* [x] It lets the problem grow with the processor count
* [ ] It assumes the serial fraction grows with p
* [ ] It gives a lower bound instead of an upper bound

> **Explanation:** Amdahl asks how much faster a fixed problem can run. Gustafson asks how much larger a problem can be solved in about the same time, giving a more optimistic, weak-scaling view.

#### Q. A scaled workload has a serial fraction of 0.05. What speedup does Gustafson's Law predict on 32 processors?

* [ ] About 12.5
* [ ] About 20.0
* [x] About 30.45
* [ ] Exactly 32.0

> **Explanation:** Gustafson's scaled speedup is `p - α(p - 1) = 32 - 0.05 × 31 = 30.45`. Because the parallel work grows with p, the serial part matters much less than under Amdahl's Law.

#### Q. A computation takes 100 seconds on one processor and 28 seconds on four. What is its parallel efficiency?

* [ ] About 28%
* [ ] About 72%
* [x] About 89%
* [ ] About 357%

> **Explanation:** Speedup is `100 / 28 ≈ 3.57`, and efficiency is speedup divided by processor count: `3.57 / 4 ≈ 0.89`. An efficiency near 100% means near-linear speedup.

#### Q. As more processors are applied to a fixed-size problem, what typically happens to parallel efficiency?

* [ ] It stays constant at 100%
* [ ] It rises steadily along with speedup
* [x] It falls, even while speedup may still increase
* [ ] It becomes exactly zero beyond 8 processors

> **Explanation:** Serial work, communication and synchronization stay roughly constant or grow while each processor's share of useful work shrinks, so each added processor contributes less.

#### Q. What is superlinear speedup, and what can cause it?

* [ ] Speedup below 1, caused by too much communication
* [x] Speedup above p, often because each part now fits in cache
* [ ] Speedup exactly equal to p, from perfect load balance
* [ ] An impossible result that always indicates a timing bug

> **Explanation:** Splitting the problem shrinks each worker's working set, which can cut cache misses or paging. The parallel run then does less memory work per element than the one-processor baseline.

#### Q. What is an advantage of fine-grained parallelism with many small tasks?

* [ ] A high computation-to-communication ratio
* [ ] Lower scheduling overhead
* [x] Good load balancing
* [ ] Better cache locality

> **Explanation:** Many small tasks can be spread evenly and rebalanced dynamically. The cost is more frequent scheduling, synchronization and communication.

#### Q. What is an advantage of coarse-grained parallelism with a few large tasks?

* [x] A high computation-to-communication ratio
* [ ] Good load balancing for irregular work
* [ ] More tasks available than processors
* [ ] Easier recovery from slow tasks

> **Explanation:** Large tasks do more computation between coordination events, so overhead is a smaller fraction of runtime. If a few tasks run long, though, other processors sit idle.

#### Q. Granularity can be described as the ratio of _____ to _____.

* [ ] speedup; number of processors
* [ ] tasks; time
* [ ] tasks; number of processors
* [x] computation; communication

> **Explanation:** Granularity compares useful work in a task with its coordination cost. A task must do enough work to amortise the cost of scheduling and communicating it.

#### Q. In a computational graph, which of these describes the critical path?

* [x] The longest chain of dependent operations through the program
* [ ] The total time of all task nodes in the graph
* [ ] The shortest path from the start node to the end node
* [ ] The set of tasks that access shared memory

> **Explanation:** Operations on the critical path must run one after another, so its length bounds the execution time no matter how many processors are available.

#### Q. In a computational graph, which of these describes a program's work?

* [ ] The time of the task nodes along the critical path
* [ ] The longest chain of dependent operations
* [x] The total time of all task nodes in the graph
* [ ] The number of edges between task nodes

> **Explanation:** Work, `T_1`, is the time a single processor needs to execute every task. It is the total amount of computation, regardless of how it is scheduled.

#### Q. In a computational graph, which of these describes a program's span?

* [ ] The total time of all task nodes in the graph
* [x] The total time of the task nodes along the critical path
* [ ] The number of processors needed to run the graph
* [ ] The time between the first and second task nodes

> **Explanation:** Span, `T_∞`, is the execution time with unlimited processors. It equals the length of the critical path.

#### Q. What does the ratio of work to span represent?

* [ ] The communication overhead paid per processor
* [x] The parallelism, an upper bound on achievable speedup
* [ ] The fraction of the program that must run serially
* [ ] The number of tasks that are on the critical path

> **Explanation:** Because span is part of the work, the ratio is always at least 1. A ratio of 1 means no parallelism; larger values mean more processors can be used profitably.

#### Q. Why are computational graphs useful when designing a parallel program?

* [ ] They automatically eliminate data races between tasks
* [x] They show which tasks depend on which, exposing parallelism
* [ ] They replace the need to measure performance later
* [ ] They pinpoint exactly which lines of code need a mutex

> **Explanation:** Tasks with no path between them in the graph are independent and can run concurrently. The graph also exposes the critical path that limits speedup.

#### Q. Increasing the number of processors while keeping the problem size per processor fixed (weak scaling) aims to accomplish _____ in _____.

* [ ] the same work; less time
* [x] more work; the same time
* [ ] more work; less time
* [ ] less work; the same time

> **Explanation:** In weak scaling the total problem grows with the machine. Ideally the runtime stays constant while the total amount of work increases.

#### Q. Increasing the number of processors while keeping the total problem size fixed (strong scaling) aims to accomplish _____ in _____.

* [ ] more work; less time
* [x] the same work; less time
* [ ] more work; the same time
* [ ] less work; more time

> **Explanation:** Strong scaling divides a fixed problem among more processors. It eventually saturates when each processor's share becomes too small relative to overhead.

#### Q. Why should you repeat a timing measurement several times and report the variation?

* [ ] The system clock drifts randomly between runs
* [ ] The first run always crashes on parallel machines
* [x] Scheduling, caches and background load vary between runs
* [ ] Averaging makes the program itself run faster

> **Explanation:** Scheduling, cache state, frequency scaling and background activity all change runtimes. Repeated runs, with warm-up handled consistently, show typical behavior and how much it varies.

#### Q. A parallel run on 4 processors takes 30 seconds, and the one-processor baseline takes 100 seconds. What is the total parallel overhead, `T_o = p·T_p − T_1`?

* [ ] 0 seconds
* [x] 20 seconds
* [ ] 70 seconds
* [ ] 120 seconds

> **Explanation:** The parallel run consumes `4 × 30 = 120` processor-seconds, while the useful work is 100 seconds. The extra 20 seconds are communication, synchronization, idle time and other overhead.

#### Q. What does the scalability of a parallel program describe?

* [ ] How large its source code can grow over time
* [x] How its performance changes as resources are added
* [ ] The maximum number of threads its language allows
* [ ] How many users can download and install it at once

> **Explanation:** A scalable program keeps gaining useful performance as resources grow. Scalability is measured experimentally, for example with strong- and weak-scaling studies.

#### Q. What is load imbalance?

* [ ] A power supply that cannot sustain the processors' load
* [x] Uneven work, so some processors wait while others are busy
* [ ] Too much total work for the machine's memory to hold
* [ ] A network link carrying more traffic than another

> **Explanation:** In synchronized programs the slowest worker sets the runtime. Dynamic scheduling, work stealing or finer-grained tasks can improve balance.

#### Q. Four workers finish a phase in 10, 10, 10 and 30 seconds. What is the imbalance ratio, `T_max / T_avg`?

* [ ] 1.0
* [ ] 1.5
* [x] 2.0
* [ ] 3.0

> **Explanation:** The average is `60 / 4 = 15` seconds, and `30 / 15 = 2.0`. A ratio of 1.0 would mean perfect balance; here three workers sit idle for 20 seconds each.

#### Q. What does isoefficiency describe?

* [ ] Every processor in a system having equal efficiency
* [x] How fast problem size must grow with p to keep efficiency fixed
* [ ] The energy used per operation by a parallel computer
* [ ] The efficiency of a program's reads and writes to disk

> **Explanation:** A slowly growing isoefficiency function means the algorithm scales well: modest increases in problem size keep added processors productive.

#### Q. A program uses 4 threads and finishes in 10 seconds of elapsed time, consuming about 40 seconds of CPU time. Which time should be used to compute its speedup?

* [x] The 10 seconds of wall-clock time
* [ ] The 40 seconds of CPU time
* [ ] The CPU time divided by the number of threads
* [ ] The CPU time of the slowest thread only

> **Explanation:** Speedup is about how long the user waits, which is elapsed wall-clock time. CPU time adds up across threads and can increase even as the program gets faster.

#### Q. Why do larger problems often achieve better parallel speedup than small ones?

* [ ] Larger problems always contain less serial code
* [x] Fixed overheads become a smaller share of the total work
* [ ] Larger problems run at a higher clock frequency
* [ ] Small problems cannot be split across processors at all

> **Explanation:** Communication and start-up costs matter relatively less as useful work per processor grows. This is the effect Gustafson's Law and weak scaling describe.

#### Q. What is the main purpose of profiling a parallel program?

* [ ] To create performance profiles for different users
* [x] To find where time goes, including waiting, before optimising
* [ ] To test the program for security vulnerabilities
* [ ] To list every hardware platform the program supports

> **Explanation:** In parallel programs, time is often lost in locks, barriers, communication or imbalance rather than in the hottest function. Measure first, then optimise the measured bottleneck.

#### Q. How does sampling profiling differ from instrumentation?

* [x] Sampling records the location periodically; instrumentation adds measurement code
* [ ] Sampling requires recompiling the whole program; instrumentation never does
* [ ] Sampling can measure only GPU code; instrumentation only CPU code
* [ ] They are two names for the same profiling technique

> **Explanation:** Tools such as `perf` sample, which barely disturbs timing. Instrumentation, as with `gprof` or Callgrind, records every call but can change the behavior it measures.

#### Q. What does the roofline model show?

* [ ] The physical height limit of racks in a data centre
* [x] Performance bounds set by peak compute and memory bandwidth
* [ ] The network topology connecting the nodes of a cluster
* [ ] The cost of a machine relative to its performance

> **Explanation:** It plots attainable performance against arithmetic intensity. Kernels with low intensity sit under the sloped memory-bandwidth roof; those with high intensity hit the flat compute roof. It shows which limit to attack.

#### Q. What is arithmetic intensity?

* [ ] How complicated a program's mathematical formulas are
* [x] Operations performed per byte moved from memory
* [ ] The clock speed of the arithmetic units
* [ ] The number of arithmetic units in a processor

> **Explanation:** High arithmetic intensity means each byte loaded is reused for much computation, so the code is more likely to be compute-bound than memory-bound.

#### Q. When timing an MPI phase, why is the maximum elapsed time across ranks reported, for example with `MPI_Reduce` and `MPI_MAX`?

* [ ] The maximum is always the most accurate single measurement
* [x] The phase is not complete until the slowest rank finishes
* [ ] Rank 0 cannot measure its own time
* [ ] MPI timers only work on the slowest rank

> **Explanation:** Averaging per-rank times would hide stragglers. The parallel phase ends when every rank has finished, so the slowest one determines its duration.

#### Q. A program reports 100% CPU utilisation on every core but shows poor speedup. How is this possible?

* [ ] It is not possible, since full utilisation guarantees linear speedup
* [x] The cores may be busy spinning on locks, missing in cache or doing redundant work
* [ ] Utilisation counters are always inaccurate on multicore CPUs
* [ ] Speedup cannot be measured when utilisation is 100%

> **Explanation:** Busy is not the same as productive. Interpret utilisation together with throughput, efficiency and hardware counters to see whether the cycles are doing useful work.

#### Q. Why can timing only the host call that launches a GPU kernel be misleading?

* [ ] Kernel launches always take exactly the same time as the kernel
* [x] The launch returns before the kernel finishes running
* [ ] GPU kernels cannot be timed from the host at all
* [ ] The host clock is paused while a kernel runs

> **Explanation:** The timer captures only submission. Use GPU events for device time, or synchronize before stopping a host timer. Also state whether data transfers and allocation are included.

#### Q. In a strong-scaling study, what is the "scaling knee"?

* [ ] The processor count at which the program first produces correct output
* [x] The point beyond which adding processors gives little further improvement
* [ ] The processor count at which power consumption is highest
* [ ] The point where efficiency first exceeds 100%

> **Explanation:** Plotting runtime, speedup and efficiency against processor count shows where gains flatten out. Running past the knee wastes resources for that workload and configuration.
