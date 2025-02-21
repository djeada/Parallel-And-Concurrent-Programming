## Time Sapce Duality
This diagram illustrates the idea of “time-space duality” in parallel processing by comparing **array processors** and **vector processors**:

```
====================================================
ARRAY PROCESSOR (Spatial Parallelism)
====================================================
         |   T0   |   T1   |   T2   |   T3   |
----------------------------------------------------
 PE0     |  LD0   |  ADD0  |  MUL0  |  ST0   |
 PE1     |  LD1   |  ADD1  |  MUL1  |  ST1   |
 PE2     |  LD2   |  ADD2  |  MUL2  |  ST2   |
 PE3     |  LD3   |  ADD3  |  MUL3  |  ST3   |
----------------------------------------------------
(Same operation across all PEs in each time slot)

====================================================
VECTOR PROCESSOR (Temporal Parallelism)
====================================================
Time -->    T0     T1     T2     T3     T4     T5     T6     T7
----------------------------------------------------------------
 Elem0:     LD0    ADD0   MUL0   ST0
 Elem1:            LD1    ADD1   MUL1   ST1
 Elem2:                   LD2    ADD2   MUL2   ST2
 Elem3:                          LD3    ADD3   MUL3   ST3
----------------------------------------------------------------
(Each data element "flows" through the pipeline over time)
```

1. **Array Processors**  
   - Have multiple Processing Elements (PEs) operating **in parallel** (spatial parallelism).  
   - Each PE executes the **same operation** (e.g., LD, ADD, MUL) **at the same time**, but on different data elements.  
   - Visually, you see columns labeled PE0, PE1, PE2, PE3 all doing LD in one cycle, then all doing ADD in the next cycle, and so on.  
   - Because there is hardware for each PE, the hardware cost is higher, but each operation across the data elements is completed in **one clock cycle** (they all work simultaneously).  

2. **Vector Processors**  
   - Rely on a **pipeline** to process multiple data elements **over time** (temporal parallelism).  
   - They have one (or a few) functional units that get fed with data elements **one after another** in a pipelined fashion.  
   - You see the timeline: LD0 in the first cycle, ADD0 in the second, MUL0 in the third, ST0 in the fourth; meanwhile LD1 starts in the second cycle, ADD1 in the third, and so on.  
   - This reuses the same functional units, so hardware cost can be lower than having a separate PE for every data element. However, it takes multiple cycles for all elements to be processed because each element “slides” through the pipeline stages.  

Hence the term **time-space duality**:

- **Array Processors** replicate hardware resources “in space,” performing the same operation simultaneously across multiple data elements.  
- **Vector Processors** replicate the operations “in time,” feeding each data element through a pipelined functional unit sequentially.

Both approaches can achieve high throughput, but they balance hardware complexity and performance differently.
## Vector Procssor

- A vector is a one-dimensional array of numbers, and it is **common** in many computational tasks.  
- Many scientific and commercial programs handle vectors by using a **loop** to process each element.  
- A vector processor applies instructions to vectors rather than scalar data, which can be **beneficial** for throughput.  
- These processors store multiple elements in vector registers, and those registers are often **larger** than scalar registers.  
- A vector length register manages how many elements are processed, and it is **adjusted** to match the current data size.  
- A vector stride register indicates the distance in memory between elements, and it is **useful** for non-contiguous data.  
- Using vector instructions can reduce overhead, and this method is **efficient** for repetitive computations.
- A vector instruction performs an operation on each element in consecutive cycles, which is **useful** for data-level parallelism.  
- Vector functional units are pipelined, and each pipeline stage operates on a different data element in an **efficient** manner.  
- Deeper pipelines are possible with vector instructions, which can be **helpful** for higher throughput.  
- No intra-vector dependencies exist, so hardware interlocking is **unnecessary** within a vector.  
- No control flow occurs within a vector, which is **practical** for uniform computations.  
- A known stride allows easy address calculation for all vector elements, which is **beneficial** for regular memory access.  
- Early loading or prefetching of vectors into registers or caches can be **advantageous** for reducing stalls.  

### STRIDES

This example illustrates how **vector stride** comes into play when performing matrix multiplication in a **row-major** memory layout.

Below is a **text-based (ASCII) illustration** of two \(4 \times 4\) matrices \(A\) and \(B\) stored in **row-major order**, followed by their **linear memory layouts**. 

## Matrix \(A\) (4×4)

```
+--------+--------+--------+--------+
| A(0,0) | A(0,1) | A(0,2) | A(0,3) |
+--------+--------+--------+--------+
| A(1,0) | A(1,1) | A(1,2) | A(1,3) |
+--------+--------+--------+--------+
| A(2,0) | A(2,1) | A(2,2) | A(2,3) |
+--------+--------+--------+--------+
| A(3,0) | A(3,1) | A(3,2) | A(3,3) |
+--------+--------+--------+--------+
```

### Row-Major Memory Layout of \(A\)

```
[
  A(0,0), A(0,1), A(0,2), A(0,3),
  A(1,0), A(1,1), A(1,2), A(1,3),
  A(2,0), A(2,1), A(2,2), A(2,3),
  A(3,0), A(3,1), A(3,2), A(3,3)
]
```

- **Row Access (stride = 1):**  
  Accessing row 0 → (A(0,0), A(0,1), A(0,2), A(0,3))  
  These are consecutive in memory.

- **Column Access (stride = 4):**  
  Accessing column 0 → (A(0,0), A(1,0), A(2,0), A(3,0))  
  Each element is 4 addresses apart because each row has 4 elements.

## Matrix \(B\) (4×4)

```
+--------+--------+--------+--------+
| B(0,0) | B(0,1) | B(0,2) | B(0,3) |
+--------+--------+--------+--------+
| B(1,0) | B(1,1) | B(1,2) | B(1,3) |
+--------+--------+--------+--------+
| B(2,0) | B(2,1) | B(2,2) | B(2,3) |
+--------+--------+--------+--------+
| B(3,0) | B(3,1) | B(3,2) | B(3,3) |
+--------+--------+--------+--------+
```

### Row-Major Memory Layout of \(B\)

```
[
  B(0,0), B(0,1), B(0,2), B(0,3),
  B(1,0), B(1,1), B(1,2), B(1,3),
  B(2,0), B(2,1), B(2,2), B(2,3),
  B(3,0), B(3,1), B(3,2), B(3,3)
]
```

- **Row Access (stride = 1):**  
  For row 0 → (B(0,0), B(0,1), B(0,2), B(0,3))

- **Column Access (stride = 4):**  
  For column 0 → (B(0,0), B(1,0), B(2,0), B(3,0))

- **Accessing a row** is **contiguous** (stride = 1).  
- **Accessing a column** “jumps” by the row length (stride = number of columns, which is 4 in this example).  

This concept generalizes to any matrix dimension. For instance, if matrix \(B\) had 10 columns, accessing a column would require a stride of 10.



### Context: Matrix Multiply (C = A × B)

- Each element of the result matrix C is the dot product of:
  - **A’s row** (say, row i)  
  - **B’s column** (say, column j)

- Both matrices A and B are stored in **row-major order**, meaning:
  - Consecutive memory locations store consecutive elements in each row

### Why Stride?

- When you load a **row** of A (e.g., A’s row i), the elements are laid out contiguously in memory.  
  - That implies a **stride of 1** (you move to the next memory address for the next element).

- When you load a **column** of B (e.g., B’s column j), those elements are *not* contiguous in row-major storage.  
  - Instead, each element of the column is separated by the full row width of B.  
  - So the stride equals the number of columns in B.  
  - For example, if B has 10 columns, then each element in the column is 10 addresses apart in memory, i.e., a **stride of 10**.

### Step-by-Step:

1. **Load A’s row i**  
   - Suppose row i has elements \( A_{i,0}, A_{i,1}, \dots, A_{i,9} \).  
   - In row-major order, these are contiguous in memory.  
   - So you can load them into a vector register \( \text{VR}_1 \) with **stride = 1**.

2. **Load B’s column j**  
   - Suppose column j has elements \( B_{0,j}, B_{1,j}, \dots, B_{9,j} \).  
   - In row-major order, these are spaced apart by the row width of B.  
   - So you load them into a vector register \( \text{VR}_2 \) with **stride = \text{(number of columns in B)} \).  
   - In the diagram, that stride is 10.

3. **Perform Vector Multiply and Accumulate**  
   - Once A’s row and B’s column are both in vector registers, you can do a vector multiply (element-wise) and then accumulate the result (sum up the products) to get one element \( C_{i,j} \).

### Loading storing vectors from memory
- Loading and storing vectors from memory involves retrieving multiple elements, which is useful for data-level parallelism.  
- The stride between elements is a constant distance, which can be set to match different memory layouts.  
- Elements can be loaded in consecutive cycles if each cycle initiates the load of one new element, which is beneficial for high throughput.  
- A memory that takes more than one cycle per access can be managed by banking and interleaving elements across multiple banks, which is helpful for overlapping load operations.  
- Assuming a stride of one simplifies address calculations, which can be practical for contiguous data.

### memory banking

Each bank has its own **Memory Address Register (MAR)** and **Memory Data Register (MDR)**, and they all connect to the **CPU** via a shared **address bus** and **data bus**.

```
                 +-----------------+
                 |      CPU       |
                 |  (exec unit,   |
                 |   control)     |
                 +--------+-------+
                          |
             +------------+------------+
             |   Address Bus &        |
             |     Data Bus           |
             +------------+------------+
                          |
         -------------------------------------------------
         |          |           |       ...       |      |
         v          v           v                 v      v
+-----------+  +-----------+  +-----------+   ...   +-----------+
|  Bank 0   |  |  Bank 1   |  |  Bank 2   |         | Bank 15   |
| (MAR,MDR) |  | (MAR,MDR) |  | (MAR,MDR) |         | (MAR,MDR)  |
+-----------+  +-----------+  +-----------+   ...   +-----------+
```

**Key Points:**

1. **CPU**: Issues addresses and reads/writes data.  
2. **Address Bus**: Carries the memory address from the CPU to the appropriate memory bank’s MAR.  
3. **Data Bus**: Carries data to/from the CPU and each bank’s MDR.  
4. **Banks**: Multiple memory banks (e.g., Bank 0, Bank 1, … Bank 15). Each has its own **MAR** and **MDR** to handle address/data in parallel.
5. 
### vectoraizable loops

- A loop is considered vectorizable if each iteration is independent from the others.  
- The example loop for i = 0 to 49, C[i] = (A[i] + B[i]) / 2 meets this condition because each element can be processed separately.  
- The vectorized version of this loop sets the vector length register to 50 and the vector stride register to 1.  
- The instructions load A into V0, load B into V1, add V0 and V1 to produce V2, shift V2 right by one bit to produce V3, and store V3 into C.  
- Each instruction has a specific latency that depends on the hardware implementation and the vector length.  
- The shift right by one bit is mathematically equivalent to dividing by two, which calculates the average of the corresponding elements of A and B.  
- Vectorizing the loop can reduce loop overhead by issuing a single instruction for multiple data elements.

### GPUS are simd engines

- GPUs use an instruction pipeline that operates like a SIMD pipeline, which is **beneficial** for parallel processing.  
- User programming is done with threads rather than explicit SIMD instructions, but the hardware still runs threads in a **SIMD** fashion under the hood.  
- Each GPU core executes many threads in lockstep, which can be **helpful** for hiding memory latency.  
- The hardware groups threads into warps or wavefronts that share the same instruction stream, and this is **relevant** for achieving data-level parallelism.  
- Parallelizable code examples map naturally onto the thread-based model, although each thread often executes the same instructions in a **SIMD** manner.  
- Recognizing the difference between thread-based and SIMD-based approaches can be **useful** for optimizing GPU applications.

### SIMD VS SIMT

- SIMD is a single sequential instruction stream that operates on multiple data elements in parallel.  
- SIMT uses multiple instruction streams of scalar instructions, and threads are grouped dynamically into warps.  
- Traditional SIMD hardware employs instructions like VLD, VADD, and VST, with a vector length register to manage data elements.  
- SIMT hardware relies on thread-based instructions, such as CUDA kernels, and defines parameters like NumThreads for parallel execution.  
- One advantage of SIMT is that it can treat each thread independently on any type of scalar pipeline, which allows flexible MIMD-style processing.  
- Another advantage of SIMT is that it can group threads into warps dynamically, which provides a way to exploit data-parallel execution while maintaining flexibility.

### Fine Grained Multihtreading
- Fine-grained multithreading uses multiple thread contexts, which allows each cycle to fetch instructions from a different thread.  
- The pipeline fetches from a new thread while previous instructions are still resolving, which can reduce idle cycles.  
- There is no additional hardware logic needed for handling data dependencies within a thread, because threads proceed independently.  
- This approach tolerates control and data dependence latencies by overlapping them with useful work from other threads.  
- It helps utilize pipeline stages more fully by having multiple threads in flight.  
- Single-thread performance is reduced, because the pipeline switches among threads rather than focusing on one thread.  
- Extra hardware is required to support multiple thread contexts, which increases design complexity.  
- If there are not enough threads ready to run, the pipeline can still stall.

### high-level GPU architecture

- A **front-end** (Program Counter, mask registers, instruction cache, decode logic, etc.).  
- Multiple **scalar pipelines** and a **SIMD execution** block.  
- An **interconnection network** connecting multiple **shader cores**.  
- Multiple **memory controllers** connected to **GDDR** memory.

```
                              +--------------------------------+
                              |           PC, Mask             |
                              |        +--------------+        |
                              |        |   I-Cache    |        |
                              |        +--------------+        |
                              |             Decode             |
                              |                                |
                              |  +--------------+  +--------+  |
                              |  | Scalar Pipe  |  |  ...   |  |  <-- Possibly multiple
                              |  +--------------+  +--------+  |      scalar pipelines
                              |  | Scalar Pipe  |               |
                              |  +--------------+               |
                              |         ...                     |
                              |        SIMD Exec                |
                              +---------------+-----------------+
                                              |
                                              |
            +----------------------+-----------+----------------------+
            |                Interconnection Network                |
            +------------+------------+------------+----------------+
                         |            |            |
                         v            v            v
                  +-----------+  +-----------+  +-----------+  ...
                  |ShaderCore |  |ShaderCore |  |ShaderCore |
                  +-----------+  +-----------+  +-----------+
                         |            |            |
                         +-----+------+------+-----+
                               |      |      |
         +---------------------+------+------+---------------------+
         |                 Memory Controller(s)                   |
         |                 +--------------+                       |
         |                 |    GDDRx     |  (e.g., GDDR3, GDDR5) |
         +-----------------+--------------+------------------------+
```

1. **Front-End**  
   - **PC (Program Counter), Mask**: Tracks instruction addresses and active lanes (threads).  
   - **I-Cache**: Holds instructions to be decoded.  
   - **Decode**: Decodes instructions into micro-ops for the pipelines.  
   - **Scalar Pipelines**: Execute scalar operations (e.g., address calculations, control flow).  
   - **SIMD Execution**: Executes vector (wide) operations for parallel data processing (shaders).

2. **Interconnection Network**  
   - Connects the front-end and memory subsystem to multiple **shader cores** (compute units).  

3. **Shader Cores**  
   - Each core has ALUs, registers, and scheduling hardware for parallel threads (shaders).

4. **Memory Controllers + GDDR**  
   - Multiple controllers handle high-bandwidth GDDR memory accesses in parallel.
  
### General purpose programming on GPU
- General-purpose processing on GPUs can be **helpful** for workloads that exhibit data-level parallelism.  
- The SPMD programming model is frequently used because it is **versatile** for many different tasks.  
- Matrices, image processing, and deep neural networks can exploit **parallel** execution on GPU hardware.  
- Using a GPU for these tasks requires a new programming model that can be **useful** for large-scale data operations.  
- The trade-off for this flexibility involves **additional** resource usage and increased complexity in coding.
- CPU few out of order cores
- GPU many in order FGMT cores

Three steps:
- GPU loads from cpu
- gpu kernel
- gpu to cpu

### Wraps and blocks
- CPU threads often handle sequential or modestly parallel tasks, while GPU kernels handle massively parallel workloads.  
- GPU warps are hardware-level groupings of threads that are not directly exposed to the programmer.  
- The programmer launches parallel kernels on the GPU from the host code using syntax like KernelA<<<nBlk, nThr>>>(args).  
- Each GPU kernel is split into many threads, but the hardware manages how these threads are grouped into warps.  
- The programmer can focus on writing thread-level code without managing or interacting with warp formation.  
- The host code typically runs serially between kernel launches, and the GPU runs many threads in parallel within each kernel.  
- Warps schedule threads that execute the same instruction stream, but this scheduling is hidden from the user.  
- This separation allows the GPU hardware to optimize performance internally while the user writes simpler thread-based code.

- GPU cores rely on **SIMD** pipelines, which are known as streaming multiprocessors and streaming processors.  
- A block of threads is divided into warps, and each warp usually contains 32 **threads**.  
- Warps execute instructions in lockstep, which can be **helpful** for data-level parallelism.  
- The hardware manages warp formation automatically, and programmers do not interact with **warps** directly.  
- This design can be **useful** for balancing the workload across GPU resources.

- Traditional SIMD processes a single thread with multiple data lanes in **lockstep**, which can be beneficial for uniform computations.  
- The programming model for traditional SIMD relies on explicit vector instructions, which can be **useful** for data-level parallelism.  
- Warp-based SIMD (SIMT) runs multiple scalar threads in a **SIMD** manner, which means each warp executes the same instruction on different data.  
- The hardware automatically groups threads into warps, which can be **helpful** for managing large numbers of threads.  
- Traditional SIMD requires the software to specify vector lengths, which is **practical** for controlling how many data elements are processed.  
- Warp-based SIMD conceals vector-length details from the programmer, which is **convenient** for thread-based coding.  
- Traditional SIMD ISAs often include vector instructions, while warp-based SIMD relies on **scalar** instructions repeated across many threads.

### Program Structure CUDA

- A CUDA program typically includes function prototypes marked with __global__ to indicate that these functions will run on the GPU.  
- The main function usually starts by allocating memory on the GPU using cudaMalloc, which reserves device memory.  
- After allocation, data is transferred from the host to the device with cudaMemcpy, which moves the input data to GPU memory.  
- The kernel function is then launched using the triple angle bracket notation (kernel<<<blocks, threads>>>(args)), which specifies the execution configuration.  
- Once the kernel finishes execution, the results are transferred back to the host using cudaMemcpy to retrieve output data from the device.  
- A kernel function can use automatic variables that the compiler places in registers, which is efficient for frequently accessed data.  
- Shared memory is declared using __shared__ inside the kernel, which can be helpful for reducing global memory accesses.  
- Intra-block synchronization is managed by calling __syncthreads, which ensures that all threads within a block reach the same point before proceeding.
- 
