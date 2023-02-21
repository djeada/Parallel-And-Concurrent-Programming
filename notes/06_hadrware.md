# Hardware in Parallel Computing

Parallel computing involves dividing a task into smaller parts that can be processed simultaneously by multiple processors. We will explore the different ways of achieving parallelism in hardware and how they affect the performance of parallel computing.

## Ways of Achieving Parallelism

There are different ways of achieving parallelism:

- Within the processor (instruction-level parallelism, multicore).
- Using several processors in the same machine (multiprocessing).
- Using various machines (distributed computing, multicomputer).

## Single-Core CPU

In a single-core CPU, every step of the program is converted to a binary instruction(s), which is tailored to the individual CPU architecture. 

1. The program is first loaded into system memory (RAM) from the hard drive, 
2. and the instructions are then sent from system memory to the CPU via a bus. 
3. Once a program is loaded into the CPU, it moves down the queue (pipeline) of instructions, which are executed one at a time. 
4. To execute instructions, the CPU employs many components (ALU, for example), and the clock determines the speed of the CPU.

A single-core processor is limited when it comes to parallel computing because it can only perform one task at a time. 

## Multi-Core CPU

A multi-core CPU is intended to handle multithreading easily. 

* The term "multi-core" refers to the fact that everything is replicated (pipeline and execution engine). 
* Hyper-threading was one of the early concepts for improving multithreading efficiency. The idea is to use one physical core to run more than one thread at the same time. The pipelines for the threads were being duplicated in hyper-threading, making it quicker, but not everything could be executed in parallel.

## Graphics Processing Unit (GPU)

Graphics Processing Units (GPUs) are specialized processors that are designed to execute thousands of tasks simultaneously. Unlike traditional CPUs that can only handle a limited number of tasks at once, GPUs can perform many computations in parallel.

Initially, GPUs were developed for rendering complex graphics in video games and other applications. However, their high-performance computing capabilities have since been leveraged in a broad range of scientific and engineering domains.

One of the primary advantages of GPUs is their ability to execute data-parallel computations. Data-parallelism breaks down large computational tasks into many smaller ones that can be executed concurrently. GPUs are particularly well-suited to this type of parallelism because they have many small processing cores that can work on different aspects of the same problem simultaneously.

To harness the power of GPUs, software developers typically use specialized programming languages and libraries such as CUDA, OpenCL, and OpenGL. These tools enable developers to write code that can be executed on the GPU and manage the communication between the CPU and the GPU.

By utilizing GPUs for parallel computing, developers can achieve significant performance gains over traditional CPU-based systems. However, this requires a strong understanding of parallel programming concepts and the ability to design algorithms that can take advantage of the parallelism provided by GPUs.

## Shared Memory Architectures

Shared memory architectures have all processors act independently but access the same global address space. Changes in one memory location are visible for all others. There are two types of shared memory architectures:

- Uniform Memory Access (UMA): Equal load and store access for all processors to all memory. It was the default approach for the majority of SMP systems in the past.
- Non-Uniform Memory Access (NUMA): Delay on memory access according to the accessed region. It is typically realized by a processor interconnection network and local memories. Cache-coherent NUMA (CC-NUMA) is completely implemented in hardware and has become the standard approach with recent X86 chips.

## Data Parallel / SIMD vs Task Parallel / MIMD

There are two types of parallelism:

- Data Parallel / SIMD: This involves performing the same operation on multiple data sets simultaneously. Examples of hardware that use this approach include GPUs, Cell processors, SSE, AltiVec, and vector processors.
- Task Parallel / MIMD: This involves performing different operations on different data sets simultaneously. Examples of hardware that use this approach include many-core/SMP systems, processor-array systems, systolic arrays, Hadoop, cluster systems, and MPP systems.

