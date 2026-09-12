## Hardware in Parallel Computing

Parallel computing uses multiple hardware execution resources to make progress on more than one operation at a time. Those resources may exist inside one CPU core, across several cores or processor sockets, on a GPU, or across multiple networked machines. The useful performance gain depends not only on how much work can run in parallel, but also on memory bandwidth, communication cost, synchronization, and how well the workload matches the hardware.

### Ways of Achieving Parallelism

Hardware and software expose parallelism at several levels:

- **Instruction-level parallelism (ILP)** allows one CPU core to overlap or execute multiple independent instructions. Pipelining, superscalar issue, out-of-order execution, branch prediction, and speculative execution are common techniques.
- **Vector or SIMD parallelism** applies one instruction to multiple data elements at once. CPU instruction sets such as AVX and NEON use this approach for vectorized workloads.
- **Thread-level parallelism (TLP)** uses multiple hardware threads or CPU cores so independent instruction streams can make progress concurrently.
- **Multicore processing** places several CPU cores on one processor package. Each core has its own execution resources, while some cache levels, memory controllers, and interconnects may be shared.
- **Multiprocessor systems** contain multiple processor packages or sockets. They provide more cores and memory capacity, but communication and memory access may become non-uniform across sockets.
- **GPU parallelism** uses many hardware execution lanes to provide very high throughput for regular, data-parallel workloads.
- **Distributed parallelism** spreads work across separate computers connected by a network. This provides scalability beyond one machine, at the cost of higher communication latency and explicit coordination.

These forms of parallelism are often combined. A cluster node, for example, may contain several CPU sockets, each CPU may contain many cores with SIMD units, and the node may also include one or more GPUs.

### Single-Core CPU

A single-core CPU has one physical processing core, but that does **not** mean it performs only one operation at a time. Modern cores exploit substantial parallelism internally through pipelining, multiple execution units, out-of-order execution, and vector instructions. The operating system can also time-slice several software threads on one core, giving them concurrent progress even though only one hardware context may be issuing instructions at a given instant.

A simplified instruction path is:

1. **Fetch** instructions from the instruction cache or memory hierarchy.
2. **Decode** them into operations understood by the processor.
3. **Rename and schedule** operations when the architecture supports out-of-order execution.
4. **Execute** independent operations on units such as integer ALUs, floating-point units, vector units, and load/store units.
5. **Retire** completed instructions in architectural program order.

Several instructions can occupy different pipeline stages at the same time. A superscalar core may also begin multiple independent instructions in one cycle.

#### Instruction-Level Parallelism Inside One Core

The amount of useful ILP depends on the instruction stream.

- Independent arithmetic operations can often overlap.
- Data dependencies force some instructions to wait for earlier results.
- Branch mispredictions can discard speculative work and refill the pipeline.
- Cache misses may stall dependent instructions for many cycles.
- SIMD instructions increase data-level parallelism by processing several values per instruction.

Clock frequency matters, but performance cannot be predicted from clock speed alone. Instructions per cycle (IPC), cache behavior, branch prediction, vector width, memory latency, and the workload itself are equally important.

### Multi-Core CPU

A multi-core CPU contains several physical cores on one processor package. Different cores can execute different software threads at the same time, so applications with independent work can achieve true parallel execution.

- Each core normally has its own execution pipeline and private architectural state.
- L1 caches are usually private to a core, while lower cache levels may be private, shared by a subset of cores, or shared across the package depending on the design.
- Cores communicate through an on-chip interconnect and ultimately share access to system memory.
- Performance scales well only when the application exposes enough parallel work and does not spend too much time synchronizing or contending for shared resources.

**Simultaneous multithreading (SMT)** allows one physical core to maintain multiple hardware thread contexts and issue instructions from more than one thread. Intel commonly calls its implementation Hyper-Threading. SMT can improve utilization when one thread is stalled, but the hardware threads share execution units, caches, and bandwidth, so two SMT threads do not provide the same throughput as two independent physical cores.

#### Cache Hierarchy and Cache Coherence

Caches reduce the latency of accessing frequently used data, but multiple cores introduce a consistency problem: several cores may cache copies of the same memory location.

A **cache-coherence protocol** coordinates those copies so cores agree on the value of each cache line. Common protocols are based on states such as Modified, Exclusive, Shared, and Invalid (MESI and related protocols).

Coherence has a performance cost:

- Frequent writes to shared cache lines generate coherence traffic.
- A cache line may move repeatedly between cores when different cores write it.
- **False sharing** occurs when cores update different variables that happen to occupy the same cache line.
- Read-mostly data is generally easier to scale than heavily shared writable data.

Cache coherence is not the same as a programming-language memory model. Correct concurrent programs still need appropriate synchronization such as atomics, locks, barriers, or message passing.

#### Memory Bandwidth and Interconnects

Adding cores increases potential compute throughput, but all cores still depend on finite memory and interconnect bandwidth. A program can therefore stop scaling long before all cores are computationally saturated.

Important hardware limits include:

- DRAM bandwidth and latency.
- Last-level-cache capacity and bandwidth.
- On-chip interconnect contention.
- Memory-controller capacity.
- NUMA effects in multi-socket systems.
- Synchronization and cache-coherence traffic.

For memory-bound workloads, adding more cores may provide little improvement once memory bandwidth is saturated.

### Graphics Processing Unit (GPU)

GPUs are throughput-oriented processors designed to execute large numbers of similar operations across many data elements. They are especially effective when a workload exposes substantial data parallelism and relatively regular control flow.

I. A GPU contains many execution lanes grouped into larger compute units. Rather than optimizing a small number of threads for minimum latency, the design keeps many threads in flight so that ready work can execute while other threads wait for memory.

II. General-purpose GPU computing commonly uses programming models such as **CUDA**, **HIP**, **OpenCL**, and **SYCL**. Graphics APIs such as OpenGL, Vulkan, and Direct3D are primarily designed for rendering, although modern graphics pipelines can also expose programmable compute stages.

III. GPUs are widely used for graphics, scientific simulation, numerical linear algebra, machine learning, image and signal processing, and other workloads with high parallelism.

IV. GPUs also require very high memory bandwidth. Graphics workloads continuously read geometry, textures, depth information, intermediate render targets, and final image data. Modern GPUs therefore use high-bandwidth memory systems such as GDDR or HBM and rely heavily on caches and locality.

A **framebuffer** is memory that stores image data associated with rendering or display. Depending on the rendering pipeline, related buffers can include:

- **Color buffers** containing rendered pixel values.
- **Depth buffers** used for depth testing in 3D scenes.
- **Stencil buffers** used for masking and other rendering operations.
- Additional render targets or intermediate buffers used by effects such as deferred shading, antialiasing, or post-processing.

The exact memory layout is implementation-dependent; modern GPUs do not simply move every rendered value directly between compute units and DRAM for each operation. Caches, compression, tiling, and on-chip storage reduce unnecessary traffic.

V. **Floating-point operations per frame**

The computational cost of rendering a frame can be estimated in part by the number of arithmetic operations required for geometry processing, shading, lighting, image effects, and other stages. However, floating-point operation count alone does not determine performance. A frame can also be limited by:

- Memory bandwidth.
- Texture sampling.
- Rasterization and pixel fill rate.
- Branch divergence.
- Synchronization.
- Cache behavior.
- CPU submission overhead.

GPUs are therefore designed to balance compute throughput with memory bandwidth and specialized graphics or matrix-processing hardware.

### Comparison: CPU vs GPU

I. CPU Architecture Diagram

```text
+---------+---------+---------+
| Control |   ALU   |   ALU   |
|  CPU    +---------+---------+
|         |   ALU   | Vector  |
+---------+---------+---------+
|          Cache Hierarchy    |
+-----------------------------+
|              DRAM           |
+-----------------------------+
```

II. GPU Architecture Diagram

```text
+------------------------------------------------+
|       Many Compute Units / Execution Lanes     |
|  [CU] [CU] [CU] [CU] [CU] [CU] [CU] [CU]     |
+------------------------------------------------+
|      Registers / Shared Memory / GPU Caches    |
+------------------------------------------------+
|                 L2 Cache                       |
+------------------------------------------------+
|          High-Bandwidth Device Memory          |
+------------------------------------------------+
```

The diagrams are intentionally simplified. Both CPUs and GPUs contain multiple cache levels, schedulers, execution units, and specialized hardware.

| **Aspect** | **CPU (Central Processing Unit)** | **GPU (Graphics Processing Unit)** |
|---|---|---|
| **Core architecture** | Fewer, complex cores optimized for strong single-thread performance and general-purpose control flow. | Many execution lanes organized for high-throughput parallel work. |
| **Control logic** | Aggressive branch prediction, speculation, and out-of-order execution are common. | More throughput-oriented scheduling; groups of threads often execute in SIMD/SIMT fashion. |
| **Cache system** | Large cache hierarchy designed to reduce latency for diverse workloads. | Cache and explicitly managed on-chip memories are tuned for high bandwidth and data reuse. |
| **Execution model** | Strong at latency-sensitive, branch-heavy, irregular, and sequential work. | Strong at regular, massively parallel workloads with many independent elements. |
| **Memory system** | Optimized for low latency and general-purpose access patterns. | Optimized for high bandwidth; discrete GPUs often have separate device memory. |
| **Parallelism** | ILP, SIMD/vector operations, SMT, and multiple CPU cores. | Large-scale thread/data parallelism, usually executed in SIMD/SIMT groups. |
| **Programming model** | General-purpose languages plus threading, vectorization, and process libraries. | CUDA, HIP, OpenCL, SYCL, shader languages, and higher-level accelerator libraries. |
| **Energy use** | Varies from low-power mobile CPUs to high-power server processors. | Varies widely; high-end accelerators can consume substantial power but offer high throughput per watt for suitable workloads. |
| **Best fit** | Operating systems, control-heavy applications, databases, compilers, interactive workloads, and mixed computation. | Graphics, machine learning, simulation, dense numerical work, image processing, and other highly parallel workloads. |

A GPU is not automatically faster than a CPU. Data-transfer cost, problem size, branch behavior, synchronization, arithmetic intensity, and available parallelism determine whether acceleration is worthwhile.

### Reducing Latency vs Increasing Throughput

- **Latency** is the time required for one operation or request to complete.
- **Throughput** is the amount of work completed per unit time.
- Low latency matters for interactive or dependency-heavy work where later computation cannot begin until an earlier result is available.
- High throughput matters when many independent items can be processed concurrently.
- Hardware optimized for latency often spends more chip area and power on large caches, branch prediction, speculative execution, and sophisticated scheduling.
- Throughput-oriented hardware spends more resources on parallel execution lanes and memory bandwidth.
- A useful analogy is transportation: latency is the travel time of one passenger, while throughput is the number of passengers moved per hour.

CPUs and GPUs occupy different points on this design spectrum. CPUs generally invest more hardware in reducing the latency of a small number of instruction streams; GPUs generally invest more hardware in sustaining many concurrent operations.

#### Throughput-Oriented Design

A throughput-oriented design tries to maximize completed work across many threads rather than minimize the execution time of one individual thread.

- GPUs keep many warps or wavefronts ready so the scheduler can issue from another group when one stalls.
- Large register files and high-bandwidth memory systems support many active threads.
- Simpler per-thread control allows more execution resources to fit on the chip.
- Occupancy alone is not a performance goal; enough active work is needed to hide latency, but excessive resource pressure can reduce efficiency.

CPUs also benefit from parallel threads, but their cores devote more area to features that improve the latency of general-purpose code. GPU threads are typically much lighter-weight and are most useful in large numbers.

#### Hardware Factors That Limit Scaling

More execution units do not guarantee proportional speedup. Common limits include:

- **Serial work:** parts of the program that cannot be parallelized.
- **Synchronization:** barriers, locks, and communication can leave hardware idle.
- **Memory bandwidth:** additional cores may compete for the same DRAM channels.
- **Cache capacity and coherence:** working sets may stop fitting in cache, or writable sharing may generate coherence traffic.
- **Load imbalance:** some workers finish earlier than others.
- **Communication latency:** especially important across sockets, accelerators, or networked nodes.
- **Power and thermal limits:** processors may reduce clock frequency when many units are active.

This is why parallel speedup usually becomes sublinear as more hardware is added.

### Parallel Computing Architectures

- Mike Flynn, *“Very High-Speed Computing Systems,”* Proc. of IEEE, 1966.
- Flynn’s taxonomy classifies architectures by the number of instruction streams and data streams they can process concurrently. The four categories are SISD, SIMD, MISD, and MIMD.

**SISD (Single Instruction, Single Data)**
A single instruction stream operates on one data stream. A simple scalar processor is the classic example, although modern scalar CPUs may still exploit internal ILP.

**SIMD (Single Instruction, Multiple Data)**
One instruction stream operates on multiple data elements. Vector processors and CPU SIMD/vector instructions are common examples. GPUs also use SIMD-like hardware execution internally, although their programming model is usually described as SIMT.

**MISD (Multiple Instructions, Single Data)**
Multiple instruction streams operate on the same data stream. Pure MISD machines are rare, and real systems are seldom classified this way. Fault-tolerant redundant computation and some specialized pipelines are sometimes used as illustrative examples, but the category has no widely used general-purpose counterpart.

**MIMD (Multiple Instructions, Multiple Data)**
Multiple instruction streams operate on multiple data streams. Multicore CPUs, multiprocessor servers, and distributed-memory clusters are common MIMD systems.

```text
                            Parallel Computer Architectures
                                       |
       --------------------------------------------------------------------
       |                    |                    |                         |
     SISD                 SIMD                 MISD                     MIMD
       |                    |                    |                         |
 scalar processors   vector / array      rare / specialized      ----------------
                         processors                              |              |
                                                        shared-memory    distributed-memory
                                                           systems          systems
                                                        /        \        /        \
                                                      UMA       NUMA   clusters     MPP
```

Flynn’s taxonomy is useful as a high-level classification, but modern systems often combine categories. A multicore CPU is MIMD across cores while each core may also execute SIMD instructions. A GPU runs many thread groups independently, while each group is executed on SIMD-like hardware.

#### Programming model vs hardware execution model

- A **programming model** describes how programmers express parallel work: threads, tasks, vector operations, message passing, SPMD kernels, dataflow, and so on.
- A **hardware execution model** describes how the processor actually issues and executes that work: superscalar out-of-order execution, SIMD lanes, SIMT warps, hardware threads, or multiple independent cores.
- These layers do not have to match one-to-one. Sequential source code may execute on a deeply out-of-order CPU, and an SPMD GPU kernel may execute on SIMD hardware.
- Libraries and runtimes often hide additional layers. A high-level task runtime, for example, may schedule tasks onto an MIMD multicore CPU whose cores each use SIMD internally.

Keeping the distinction clear helps explain why source-level concurrency does not automatically imply hardware parallelism, and why hardware can exploit some parallelism even when the source code appears sequential.

#### Types of Parallelism: Data Parallelism vs Task Parallelism

Parallelism is often described in terms of how work is divided.

I. **Data Parallelism**

In data parallelism, the same or similar operation is applied to many data elements.

- Examples include image filters, matrix operations, vector arithmetic, particle updates, and neural-network tensor operations.
- CPU SIMD/vector instructions are one hardware mechanism for implementing data parallelism.
- GPUs expose data parallelism through many lightweight threads that are usually executed in SIMT groups.
- Array processors replicate processing elements so multiple elements can be processed spatially at once.
- Vector processors use pipelined vector functional units to operate on streams of elements. They may process multiple elements per cycle and should not be thought of simply as one scalar processor repeating an operation sequentially.
- Regular memory access and independent elements usually make data-parallel workloads easier to accelerate.

Data parallelism and SIMD are related but not identical: **data parallelism is a software/workload property**, while SIMD is one possible hardware execution mechanism.

II. **Task Parallelism**

In task parallelism, different tasks or functions execute concurrently, often on different data.

- Different CPU cores may execute unrelated threads or processes at the same time.
- Pipeline stages may perform distinct operations concurrently.
- Servers can process independent requests as separate tasks.
- Task runtimes can dynamically schedule heterogeneous work across a multicore system.
- Distributed systems can assign different tasks to different machines.

Task and data parallelism are often combined. A scientific application may run different simulation tasks across nodes while using SIMD instructions or GPUs inside each task.

#### Shared Memory Architectures

Shared-memory systems give multiple processors or cores access to one shared address space. Communication can therefore occur through loads and stores rather than explicit network messages.

A shared address space does **not** mean that every write becomes instantaneously visible everywhere. Modern processors use private caches, store buffers, speculative execution, and relaxed memory ordering. Hardware cache coherence keeps cached copies of memory consistent, while the programming model still requires synchronization to establish safe ordering between threads.

There are two common organizations:

I. **Uniform Memory Access (UMA)**

- In an ideal UMA system, processors observe approximately uniform latency to main memory.
- Small and medium symmetric multiprocessing systems often approximate this model.
- Even in UMA systems, cache hits and misses still produce different access times, so “uniform” mainly describes the relationship between processors and main-memory regions.

II. **Non-Uniform Memory Access (NUMA)**

- In NUMA systems, memory is physically attached to different processor sockets or nodes.
- Access to local memory is faster and often provides more bandwidth than access to remote memory.
- **Cache-coherent NUMA (CC-NUMA)** maintains a coherent shared address space while preserving non-uniform physical memory placement.
- Operating systems and runtimes may try to keep threads and their data on the same NUMA node.
- First-touch allocation, CPU affinity, and NUMA-aware data partitioning can have a large effect on performance.

Modern multi-socket x86 servers are common examples of CC-NUMA systems.

#### Comparison of UMA, NUMA, SIMD, and MIMD

These terms describe different dimensions of a system, so they should not be treated as mutually exclusive categories.

| Concept | CPUs | GPUs |
|---|---|---|
| **SIMD / vector execution** | CPU cores use vector instruction sets such as SSE, AVX, SVE, or NEON. | Execution lanes typically operate in SIMD/SIMT groups such as warps or wavefronts. |
| **MIMD** | Multiple CPU cores can execute independent instruction streams. | Different compute units or thread groups can make progress independently, even though execution within a warp/wave is SIMD-like. |
| **Shared address space** | Threads in a process normally share virtual memory; cache coherence coordinates copies across cores. | Threads can access device-global memory, while faster explicitly shared on-chip memory often has block/workgroup scope. Integrated or unified-memory systems may provide broader shared-address-space mechanisms. |
| **UMA** | Common as an abstraction on smaller shared-memory systems. | Not a defining GPU property. Some integrated systems share physical memory, but access cost can still vary by location, cache state, or processor. |
| **NUMA** | Common in multi-socket servers and some many-core systems. | Multi-GPU and chiplet-based accelerator systems can have non-uniform access to local versus remote memory, even when software exposes a unified address space. |

A single platform can therefore be MIMD across processors, SIMD within each processor, and NUMA in its memory organization at the same time.

### Distributed Computing and Cluster Computing Architectures

Distributed and cluster computing both use multiple computers, but they emphasize different operating assumptions. A cluster is a type of distributed system whose machines are usually managed together and connected by a relatively fast network.

#### Distributed Computing

Distributed computing uses independent computers that coordinate over a network to provide a service or solve a problem.

I. Architecture

- **Nodes** are independent computers with their own processors, memory, and operating-system state.
- **Network communication** may use TCP/IP, RDMA-capable transports, message queues, RPC, or specialized interconnects.
- **Control can be centralized or decentralized.** Some systems use a coordinator or leader; others distribute decision-making across many nodes.
- Nodes do not share ordinary memory, so communication normally requires explicit messages or distributed storage abstractions.

II. Characteristics

- **Scalability** comes from adding machines, although coordination and communication overhead usually grow as well.
- **Fault tolerance** may use replication, retry, checkpointing, consensus, or redundant tasks, depending on the system.
- **Resource aggregation** combines CPU, accelerator, memory, and storage capacity across machines.
- **Partial failure** is fundamental: one node or network link can fail while the rest of the system remains operational.

III. Examples

- Volunteer-computing projects such as **SETI@home** divide independent work across internet-connected machines.
- **Blockchain** networks are distributed systems in which nodes exchange and validate replicated ledger state.
- Large cloud services, distributed databases, and content-delivery systems are other common examples.

IV. Challenges

- **Communication latency and bandwidth** can dominate fine-grained workloads.
- **Synchronization and consistency** become more expensive when state is replicated across machines.
- **Fault handling** must account for node crashes, lost messages, retries, and network partitions.
- **Security** must protect communication and authenticate participating machines.

#### Cluster Computing

Cluster computing uses a group of machines that are usually colocated, administratively coordinated, and connected by a fast network. HPC clusters are designed to run tightly coupled parallel jobs, while data-processing clusters may emphasize storage and throughput.

I. Architecture

- **Compute nodes** provide CPU and/or GPU resources.
- A cluster often has **login, management, or controller nodes**, but not every cluster architecture depends on one permanent “head node” for all computation.
- **High-speed interconnects** such as Ethernet, InfiniBand, or other HPC fabrics connect nodes.
- **Schedulers and resource managers** such as Slurm or Kubernetes assign work according to the cluster’s purpose.

II. Characteristics

- **High aggregate performance** comes from combining many nodes.
- Nodes are often similar for operational simplicity, but heterogeneous clusters with different CPUs, GPUs, memory capacities, or accelerator types are common.
- **Resource management** schedules jobs, allocates processors and memory, and may enforce priorities or quotas.
- Tight coupling is practical when the network offers low latency and high bandwidth.

III. Examples

- **Beowulf-style clusters** use commodity machines for scientific and technical computing.
- **HPC clusters** run applications based on MPI, distributed solvers, and accelerator-aware libraries.
- **Hadoop or Spark clusters** distribute data-processing tasks across machines, although their execution model differs from tightly coupled HPC workloads.

IV. Challenges

- **Scalability** requires attention to network topology, storage, schedulers, and application communication patterns.
- **Cost and power** grow with node count, networking, cooling, and storage.
- **Reliability and maintenance** require monitoring, replacement, software deployment, and failure recovery.
- **Load imbalance** can leave expensive nodes idle while other nodes remain busy.

#### Comparison of Distributed and Cluster Computing

| **Aspect** | **Distributed Computing** | **Cluster Computing** |
|---|---|---|
| **Scope** | Broad category covering independent networked machines that cooperate. | A more tightly managed form of distributed computing, usually within one organization or facility. |
| **Network** | May span datacenters or the public internet; latency can be high or variable. | Usually uses a local, relatively high-speed network. |
| **Management** | Can be centralized, decentralized, or hierarchical. | Usually coordinated by shared management and scheduling infrastructure. |
| **Hardware** | Often heterogeneous and geographically dispersed. | Often more uniform, though heterogeneous CPU/GPU clusters are common. |
| **Failure model** | Partial failures and network partitions are normal design concerns. | Node failures still occur, but networking and administration are usually more controlled. |
| **Typical use cases** | Cloud services, distributed databases, global systems, volunteer computing. | HPC simulations, batch analytics, AI training, and tightly coupled parallel jobs. |
