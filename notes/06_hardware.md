## Hardware in Parallel Computing

Parallel computing is the process of breaking a task into smaller parts that can be processed simultaneously by multiple processors. These notes explore the different ways of achieving parallelism in hardware and their impact on parallel computing performance.

### Ways of Achieving Parallelism

Parallelism refers to the simultaneous execution of multiple tasks or processes to improve performance and efficiency. There are several ways to achieve parallelism:

- Modern CPUs achieve Instruction-Level Parallelism (ILP) through techniques like pipelining, which breaks down the execution of instructions into several overlapping stages—fetch, decode, execute, and write-back. This means that while one instruction is being decoded, another can be fetched, and a third can be executed simultaneously, significantly increasing throughput and minimizing idle CPU time.  
- By integrating multiple independent cores into a single processor, multicore processing enables each core to execute its own thread or process. This design allows multiple tasks to run concurrently on the same chip, making it ideal for both multitasking and parallel execution in multi-threaded applications, thereby enhancing overall performance and energy efficiency.  
- Multiprocessing involves the use of two or more processors within a single computer system, where each processor can handle different tasks or threads simultaneously. This approach distributes the computational load, reduces bottlenecks, and results in substantial performance improvements, especially for complex, multi-threaded applications that require concurrent processing.  
- The setup of multiprocessing is commonly found in servers and high-performance workstations because these systems are designed to handle high volumes of data and numerous simultaneous operations. The use of multiple processors in these environments ensures that they can manage intensive workloads, provide high availability, and support rapid response times for demanding applications.  
- In distributed computing, multiple computers are connected over a network to work on a common task. Each machine processes a portion of the overall problem independently, which not only speeds up computation but also enhances fault tolerance and scalability by distributing the workload across various nodes.  
- The combined effort of multiple machines in distributed computing leads to faster computation and greater resilience. This approach is widely employed in cluster computing, grid computing, and cloud computing environments, where resources from many computers are pooled together to solve complex problems, analyze large datasets, and provide high-performance solutions for both scientific research and enterprise applications.

### Single-Core CPU

A single-core CPU can only perform one task at a time, limiting its parallel computing capabilities. Here's a detailed breakdown of how a single-core CPU processes tasks:

- The program's steps are converted into binary instructions specific to the CPU architecture during instruction fetching, and these instructions are stored in the system's memory (RAM).
- The program is loaded into system memory and sent to the CPU via a bus during instruction loading, with the CPU fetching these instructions one by one from the memory.
- The CPU processes each instruction using its components like the Arithmetic Logic Unit (ALU) during instruction execution, where the ALU performs arithmetic and logical operations.
- The clock speed of the CPU determines how quickly these instructions are processed, with higher clock speeds resulting in faster performance during instruction execution.
- Because it is a single-core CPU, it processes instructions sequentially, one at a time, which limits its ability to handle multiple tasks simultaneously during sequential processing.

### Multi-Core CPU

Multi-core CPUs contain multiple cores within a single processor chip, each capable of executing its own thread or process. This architecture significantly improves parallel computing performance. Here's how it works:

- With multiple cores each core has its own pipeline and execution engine, which allows multiple threads or processes to be handled simultaneously in a multi-core processor. This is particularly beneficial for multi-threaded applications that can distribute their workload across several cores.
- Hyper-threading is an early form of multithreading where a single physical core appears as two logical cores to the operating system, allowing it to run multiple threads simultaneously. While this improves efficiency, it is not as effective as having true physical cores, as it still relies on shared resources within the core.

### Graphics Processing Unit (GPU)

GPUs are specialized processors designed for parallel computing, particularly data parallelism, where many small processing cores work on different parts of the same problem simultaneously. Here's a deeper look into GPUs:

I. GPUs consist of thousands of smaller, efficient cores, making them ideal for tasks that can be divided into smaller, independent operations, such as rendering graphics and running complex simulations. This ability to handle multiple tasks simultaneously is due to their **parallel architecture**.

II. To leverage GPU parallel processing power, developers use **specialized programming** languages and libraries like CUDA, OpenCL, and OpenGL. These tools allow developers to write programs that can distribute work across the many cores of a GPU, optimizing performance.

III. Beyond graphics rendering, GPUs are utilized in various **high-performance computing applications** such as scientific simulations, data analysis, and machine learning. The parallel nature of these tasks makes GPUs a perfect fit, resulting in significant performance improvements over traditional CPU-based systems.

IV. Due to graphics **frame buffer** requirements, a GPU must be capable of moving extremely large amounts of data in and out of its main DRAM. The frame buffer is a dedicated block of memory that holds the image data currently being displayed or about to be displayed on the screen, which includes several types of data:

- The **color data** for each pixel, which contains color information.
- **Depth data**, which provides information about the distance of each pixel from the viewer, essential for 3D rendering.
- **Stencil data**, used for masking certain parts of the rendering process.
- **Accumulation data**, used for special effects like motion blur.
   
The frame buffer must be large enough to store this data for the entire display resolution and needs to be updated rapidly to maintain smooth video playback and responsive graphics rendering.

Imagine you're watching a high-definition video or playing a video game. The GPU has to keep track of every tiny dot (pixel) on your screen, including its color and depth in a 3D space. All this data is stored in the frame buffer, a special part of the GPU's memory. To ensure that what you see is smooth and realistic, the GPU must quickly move this large amount of data in and out of its memory.

V. What is Floating-Point Calculations per video frame?

- The term **floating-point calculations per video frame** refers to the number of mathematical operations involving floating-point numbers (numbers with decimals) that the GPU must perform to render a single frame of video.
- These calculations encompass tasks such as lighting, shading, physics simulations, and texture mapping.
- The complexity and quality of these calculations are dependent on the GPU's processing power and architecture.
- Due to the extensive computational requirements of rendering each frame, GPUs must balance two critical performance metrics: latency and throughput.

### Comparison: CPU vs GPU

I. CPU Architecture Diagram

```
+---------+---------+---------+
| Control |   ALU   |   ALU   |
|  CPU    +---------+---------+
|         |   ALU   |   ALU   |
+---------+---------+---------+
|                Cache        |
+-----------------------------+
|              DRAM           |
+-----------------------------+
```

II. GPU Architecture Diagram

```
+-----------------------------+
|  |  |  |  | GPU |  |  |  |  |
+-----------------------------+
|  |  |  |  |  |  |  |  |  |  |
+-----------------------------+
|  |  |  |  |  |  |  |  |  |  |
+-----------------------------+
|  |  |  |  |  |  |  |  |  |  |
+-----------------------------+
|  |  |  |  |  |  |  |  |  |  |
+-----------------------------+
|              DRAM           |
+-----------------------------+
```

Below is a table comparing CPUs and GPUs, emphasizing their unique features and common use cases:

| **Aspect**            | **CPU (Central Processing Unit)**                                                          | **GPU (Graphics Processing Unit)**                                                            |
|-----------------------|---------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------|
| **Core Architecture** | Few powerful cores designed for executing complex tasks and instructions.                   | Hundreds to thousands of simpler cores optimized for handling highly parallel tasks.           |
| **Control Unit**      | Sophisticated control units with features like branch prediction, out-of-order execution, and advanced instruction pipelining. | Simpler control units optimized for parallel processing rather than complex decision-making.    |
| **Cache System**      | Large multi-level caches (L1, L2, and sometimes L3) to minimize memory access latency.       | Smaller, specialized caches designed to manage large data throughput efficiently.              |
| **Execution Model**   | Optimized for low-latency execution, handling fewer tasks very quickly.                      | Optimized for high throughput, capable of executing many tasks concurrently.                   |
| **Memory Access**     | Complex memory hierarchy to ensure fast access to data.                                      | High-bandwidth memory systems designed to handle large volumes of data efficiently (e.g., GDDR6). |
| **Power Efficiency**  | Generally optimized for lower power consumption in typical computing tasks.                  | Often optimized for performance, resulting in higher power consumption.                         |
| **Application Focus** | Suited for general-purpose computing tasks, such as running operating systems and applications. | Specialized for tasks requiring massive parallelism, such as rendering graphics and computations in AI. |
| **Programming Model** | Uses conventional programming models and languages like C, C++, and Python.                  | Uses specialized programming models and languages like CUDA and OpenCL for parallel processing.  |
| **Cooling Requirements** | Typically requires less aggressive cooling solutions.                                      | Often requires more advanced cooling solutions due to higher power consumption and heat generation. |
| **Market**            | Commonly found in personal computers, servers, and mobile devices.                           | Commonly found in gaming consoles, workstations, and data centers for tasks like machine learning. |

### Reducing Latency vs Increasing Throughput

- In computing, **latency** is the time taken to complete a single task from start to finish.
- **Throughput** measures the number of tasks that can be completed in a given amount of time.
- Reducing latency is advantageous for tasks needing quick response times, such as interactive applications.
- Increasing throughput benefits tasks that involve handling many operations simultaneously, like data processing.
- Reducing latency often necessitates high-speed, specialized hardware, which can consume more power and require more chip area.
- Increasing throughput can be achieved by parallel processing, which tends to be more power-efficient and make better use of chip area.
- An analogy is that **latency** is like the time it takes to get a single pizza delivered to your house using a fast, expensive car.
- **Throughput**, on the other hand, is like the total number of pizzas that can be delivered in an hour using several regular cars more efficiently.
- For GPUs, focusing on throughput is generally more beneficial because they handle many tasks at once without needing extremely fast individual components, thereby saving power and space on the chip.

#### Throughput-Oriented Design

- The aim of **throughput-oriented design** is to maximize the total execution throughput of many threads, even if individual threads take longer to execute.
- GPUs are designed with this approach to effectively manage the parallel nature of graphics and data processing tasks.
- **CPU design** is typically optimized for low latency to execute a single thread as quickly as possible.
- In contrast, **GPU design** is optimized for high throughput to execute many threads concurrently, even if each thread runs slower compared to a CPU.
- The power of GPUs with more threads lies in their design to handle many parallel tasks, optimizing their architecture.
- CPUs, on the other hand, perform better with fewer threads, as they are optimized for quick, sequential task execution with minimal latency.
- For GPUs, having more threads to work on simultaneously is beneficial, akin to having many assembly lines in a factory to produce multiple products at once.
- For CPUs, fewer threads are better because they function like a master craftsman focusing on one project at a time, ensuring it's completed as quickly and efficiently as possible.

### Parallel Computing Architectures

- Mike Flynn, *“Very High-Speed Computing Systems,”* Proc. of IEEE, 1966
- Flynn’s taxonomy classifies computer architectures based on how many instruction streams and data streams can be processed in parallel. The four main categories are SISD, SIMD, MISD, and MIMD.

**SISD (Single Instruction, Single Data)**  
A single instruction stream operates on a single data element at a time. This is the classic, straightforward model where a uniprocessor fetches instructions and data from memory and executes them in sequence.

**SIMD (Single Instruction, Multiple Data)**  
A single instruction stream operates on multiple data elements simultaneously. This category includes array processors and vector processors, where one instruction can be applied to an entire set (or array) of data elements in parallel.

**MISD (Multiple Instructions, Single Data)**  
Multiple instructions operate on a single data element. True MISD systems are rare, but systolic array processors are sometimes considered a loose example, as they involve multiple processing elements each performing different operations on data as it flows through.

**MIMD (Multiple Instructions, Multiple Data)**  
Multiple instruction streams operate on multiple data elements concurrently. This category includes multiprocessor and multithreaded systems, where each processor or thread can execute its own sequence of instructions on its own set of data.

```
#
                            Parallel Computer Architectures
                                        |
      --------------------------------------------------------------------------------------------------
      |                                 |                              |                               |
    SISD                               SIMD                           MISD                            MIMD
    (Von Neumann)                       |                              ?                               |
                                        |                                                              |
                           ---------------------------                              -----------------------------
                          |                          |                              |                           |
                       Vector                     Array                    Multiprocessors            Multicomputers
                      Processor               Processor                          |                           |
                                                                 ------------------------                 ------------------
                                                                 |            |          |                |                |
                                                                UMA         COMA        NUMA             MPP              COW
                                                                 |                       |                |                
                                                           ------------              -----------          ----------
                                                           |          |              |         |          |        |
                                                          Bus      Switched       CC-NUMA    NC-NUMA    Grid    Hypercube

```

#### Types of Parallelism: Data Parallelism vs Task Parallelism

Parallelism can be broadly classified into two types, based on how tasks are divided and executed:

I. **Data Parallelism / SIMD (Single Instruction, Multiple Data)**

- In *data parallelism*, the same operation is performed simultaneously on multiple data sets. This approach leverages the fact that many computations—such as arithmetic operations, comparisons, or logical operations—can be performed concurrently when applied to large arrays or matrices. It minimizes the overhead of instruction control and maximizes the throughput by applying one instruction across a range of data elements.
- Characteristics include efficiency for tasks that involve repetitive computations on large data sets, commonly used in applications such as image processing, numerical simulations, and machine learning. The uniformity in processing makes it ideal for applications where the same computation is repeated over extensive data, reducing redundancy and enabling high-speed processing of tasks like pixel manipulations in images or batch computations in scientific models.
- *GPUs* feature thousands of small cores that perform the same operation on different pieces of data, making them highly efficient for parallel processing tasks. This architecture is particularly effective for workloads that require simultaneous processing of numerous data points. Each core executes the same instruction independently, thus dramatically speeding up tasks such as rendering graphics or performing deep learning inference.
- *SIMD Extensions* like SSE (Streaming SIMD Extensions) and AltiVec in CPUs enable the processors to perform vectorized operations, enhancing computational performance. These specialized instruction sets allow a single CPU instruction to process multiple data elements simultaneously, reducing the number of required instructions and lowering the cycle count. This improves performance in applications that perform repetitive mathematical operations across arrays or vectors.
- *Time-space duality* is the concept that a computational task can be distributed in two different ways: across *multiple processors* simultaneously (space) or over *multiple time steps* using the same processor repeatedly (time).
- *Array Processors* use *multiple processing elements* at once to handle different pieces of data simultaneously. This approach is ideal when the task can be broken down into independent operations that can run in parallel.
- In contrast, *vector processors* apply the same operation sequentially over time using *one processing element*. They are designed to handle vector operations efficiently, providing significant speed advantages in tasks involving large datasets. Optimized for executing operations on entire data arrays rather than single elements, vector processors can accelerate workloads like linear algebra computations and other high-volume data operations, leading to substantial performance gains in scientific and engineering applications.
- An example use case is applying a filter to an image, where the same operation is applied to each pixel in parallel. When filtering an image, each pixel undergoes the same transformation (such as blurring or edge detection) simultaneously. This parallel execution significantly reduces processing time compared to sequentially applying the filter to each pixel, especially in high-resolution images.*

II. **Task Parallelism / MIMD (Multiple Instruction, Multiple Data)**

- In *task parallelism*, different operations are performed simultaneously on different data sets. This approach allows each processor or core to execute distinct instructions on separate pieces of data, making it ideal for workloads where tasks are heterogeneous and can run independently, thus optimizing resource usage.
- Characteristics include suitability for applications where tasks can be divided into independent or semi-independent sub-tasks, allowing for more complex and flexible parallel execution. By breaking down a larger problem into smaller, distinct tasks, systems can process these sub-tasks concurrently. This increases flexibility, as each task can be optimized and executed according to its own computational requirements, leading to overall efficiency improvements in complex applications.
- *Many-Core/SMP Systems* consist of multiple processors that each execute different tasks on different data, optimizing overall system performance. In these systems, each core or processor handles its own thread or process, which allows for simultaneous execution of diverse operations. This architecture is well-suited for multi-threaded applications, reducing the likelihood of bottlenecks and improving the system's ability to handle parallel workloads.
- *Processor Arrays and Systolic Arrays* are types of specialized hardware designed specifically for the parallel processing of tasks, improving computational efficiency. These arrays consist of multiple processing elements arranged in a grid or pipeline structure. They facilitate rapid data movement and coordinated processing, making them highly efficient for repetitive and structured computations such as matrix multiplications or signal processing tasks.
- *Distributed Systems* such as Hadoop, cluster systems, and MPP (Massively Parallel Processing) systems distribute tasks across multiple machines, enabling the handling of large-scale data processing. By partitioning tasks among many interconnected computers, distributed systems can tackle massive datasets that would overwhelm a single machine. This method enhances scalability and fault tolerance, ensuring that large computational tasks are managed efficiently across a network.
- An example use case is a web server handling multiple client requests simultaneously, where each request is an independent task. In this scenario, a web server can assign each incoming client request to a different processor or thread. This allows the server to process requests concurrently, ensuring fast response times and efficient handling of multiple users without any single request delaying others.
  
#### Shared Memory Architectures

Shared memory architectures enable multiple processors to access a common global address space, facilitating communication and data sharing among processors. This means that any changes made to a memory location by one processor are immediately visible to all other processors. There are two primary types of shared memory architectures:

I. Uniform Memory Access (UMA)

- In **UMA**, all processors have equal access time to all memory locations.
- **Characteristics** include uniform memory access latency, regardless of which processor accesses which memory module.
- This architecture is typically used in Symmetric Multiprocessing (SMP) systems, where multiple processors share a single, uniform memory.
- An **example** of UMA is traditional SMP systems, where processors are connected to a single memory controller, ensuring equal memory access times.

II. Non-Uniform Memory Access (NUMA)

- In **NUMA**, the memory access time varies depending on the memory location relative to a processor.
- **Characteristics** include memory being physically divided into regions, with each region being closer to some processors than others. Processors have faster access to their local memory compared to remote memory, leading to non-uniform memory access times. This architecture is typically implemented using a processor interconnection network and local memories.
- **Cache-Coherent NUMA (CC-NUMA)** ensures consistency between the caches of different processors, so that all processors have a coherent view of memory. This is fully implemented in hardware and is commonly found in recent x86 chips.
- An **example** of NUMA is modern server architectures and high-performance computing systems, where memory is divided across multiple processors, each with its local memory.

#### Comparison of UMA, NUMA, SIMD, and MIMD

Below is a table that outlines the relevance of Shared Memory, UMA, NUMA, SIMD, and MIMD for both CPUs and GPUs:

| Concept              | CPUs                                                                                                        | GPUs                                                                                                                 |
|----------------------|-------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------|
| **SIMD (Single Instruction, Multiple Data)** | Use SIMD instructions (e.g., SSE, AVX, or NEON) to process multiple data elements in parallel within each core.           | Inherently rely on SIMD- or SIMT-based execution, where a single instruction is applied to multiple data elements across many threads. |
| **MIMD (Multiple Instruction, Multiple Data)** | Modern multi-core CPUs exhibit MIMD by running different instruction streams on separate cores simultaneously.            | Mostly use SIMD at the hardware level, but can show MIMD-like behavior across different thread blocks or warps that may execute divergent instructions. |
| **Shared Memory**    | All cores share a common physical memory space accessible to each processor, coordinated by caches and interconnects. | Provide on-chip shared memory (e.g., per-block or per-wavefront) that allows fast data exchange among threads within the same block.  |
| **UMA (Uniform Memory Access)** | In a UMA system, all processors access the shared memory with uniform latency, simplifying memory management.                | In integrated CPU-GPU systems with a unified memory architecture, GPUs share the same memory pool with uniform access (though performance can vary). |
| **NUMA (Non-Uniform Memory Access)** | Memory access times depend on physical proximity to specific memory regions (NUMA nodes), improving scalability for large systems. | Less common in typical GPU deployments, but can appear in high-end or multi-GPU systems for large-scale parallel processing with distributed memory. |

### Distributed Computing and Cluster Computing Architectures

Distributed computing and cluster computing are approaches to parallelism that leverage multiple machines to work on a common task. They offer significant benefits in terms of scalability, fault tolerance, and performance.

#### Distributed Computing

Distributed computing involves a network of independent computers that work together to perform a task. These computers communicate and coordinate their actions by passing messages to one another.

I. Architecture

- **Nodes** in a distributed system are independent computers, often referred to as nodes or hosts, connected via a network.
- **Network Communication** between nodes is achieved through network protocols such as TCP/IP, making communication overhead an important consideration in these systems.
- **Decentralized Control** means there is no single point of control, with each node operating independently and cooperatively to complete the task.

II. Characteristics

- **Scalability** allows for easily adding more nodes to handle larger tasks, enhancing the system's capacity.
- **Fault Tolerance** is achieved through redundancy and replication of data across nodes, improving system reliability.
- **Resource Sharing** utilizes the combined resources (CPU, memory, storage) of all nodes, maximizing efficiency.

III. Examples

- **SETI@home** is a project that uses internet-connected computers to analyze radio signals for signs of extraterrestrial intelligence.
- **Blockchain** technology, such as that used in cryptocurrencies like Bitcoin, involves each node maintaining a copy of the ledger and validating transactions.

IV. Challenges

- **Communication Latency** can affect performance due to delays in data transfer between nodes.
- **Synchronization** ensures that all nodes work cohesively and data remains consistent across the system.
- **Security** involves protecting data as it travels across potentially insecure networks, ensuring the integrity and confidentiality of information.

#### Cluster Computing

Cluster computing involves a group of closely connected computers that work together as a single system. Clusters are typically used for high-performance computing tasks.

I. Architecture

- **Nodes** in a cluster computing system consist of multiple computers, usually identical or similar, connected via a high-speed local network.
- The **Head Node** is a special node that manages the cluster, distributing tasks among the worker nodes.
- **Worker Nodes** perform the actual computation, receiving tasks from the head node and returning results upon completion.

II. Characteristics

- **High Performance** clusters are designed for intensive computational tasks requiring significant processing power.
- A **Homogeneous Environment** ensures uniform performance as nodes are often identical in hardware and software.
- **Resource Management** is handled by cluster management software that takes care of task scheduling, resource allocation, and load balancing.

III. Examples

- A **Hadoop Cluster** is used for big data processing, splitting large datasets into smaller chunks and processing them in parallel across the nodes.
- The **Beowulf Cluster** is a type of cluster architecture used for scientific computations, built from standard, off-the-shelf hardware.

IV. Challenges

- **Scalability** can be complex, often requiring reconfiguration of the cluster when adding more nodes.
- **Cost** is a consideration, as high-speed networks and homogeneous hardware can be expensive.
- Regular **Maintenance** is required to ensure optimal performance and reliability of the cluster system.

#### Comparison of Distributed and Cluster Computing

| **Aspect**           | **Distributed Computing**                                                                 | **Cluster Computing**                                                                           |
|----------------------|--------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------|
| **Infrastructure**   | Nodes can be geographically dispersed and connected over the internet.                      | Nodes are usually in close physical proximity, connected via a high-speed local network.        |
| **Management**       | Decentralized management with no single point of control.                                   | Centralized management with a head node overseeing the cluster operations.                      |
| **Use Cases**        | Suitable for tasks that require diverse resources and can tolerate higher latency (e.g., collaborative research projects, global computing networks). | Ideal for tasks needing intensive computation and low-latency communication (e.g., scientific simulations, data analysis). |
