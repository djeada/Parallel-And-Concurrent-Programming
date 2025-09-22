## GPU Programming
GPUs (Graphics Processing Units) excel at performing the same operation on many data elements in parallel. Originally, GPUs were used just for rendering images, but their highly parallel nature also makes them ideal for general-purpose tasks with large data sets, such as scientific simulations, machine learning, and other compute-intensive workloads.

**What’s special about GPU programming?**

- GPUs have many cores organized to handle massive parallelism.  
- They often use specialized memory hierarchies (shared memory, constant memory, etc.) to achieve high bandwidth.  
- The programming model (e.g., CUDA, OpenCL) provides ways to map computations onto thousands of threads, which then run in parallel.  
- Achieving good performance can require careful attention to memory accesses, thread organization, and instruction throughput.

### Time-Space Duality

GPU architectures (and more broadly, parallel processors) often exploit a concept called **time-space duality**, which compares two main ways of doing parallel operations: array processors and vector processors. Understanding these helps illustrate general principles behind GPU designs.

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

**How it fits into GPU programming?**

I. **Array Processors (Spatial Parallelism)**

- Multiple processing elements (PEs) operate in parallel on different data elements.  
- In each clock cycle, all PEs perform the same instruction (like LD or ADD) but on separate parts of the data.  
- This gives high throughput since all data elements are processed simultaneously.  
- Hardware cost is higher (because each PE needs its own functional unit), but you complete one operation across all data elements in one cycle.

II. **Vector Processors (Temporal Parallelism)**

- A single (or a few) functional unit(s) are reused over time to process many data elements in a pipeline.  
- Each data element flows through the pipeline stages (LD, ADD, MUL, ST) one after another.  
- Hardware cost is lower because the functional units are shared in time.  
- It takes multiple cycles to get all elements processed since they move through the pipeline in sequence.

In modern GPUs, there are elements of both approaches. You can think of each GPU “core” as being replicated (spatial parallelism), but within each core, pipelined operations occur (temporal parallelism). This combination helps GPUs handle large-scale parallelism efficiently.

### Vector Processor

A **vector processor** is an architecture that operates on entire vectors (1D arrays) with a single instruction. This approach is relevant to GPU programming because GPUs often provide *SIMD* (Single Instruction, Multiple Data) capabilities similar to vector processors.

* A vector is a list of elements, like $[x_0, x_1, x_2, \dots]$
* Instead of running a loop that processes each element one by one, a vector processor uses a single vector instruction (e.g., `VADD`) to add pairs of elements across two vectors in one pipeline.
* Multiple elements can be stored in **vector registers**, which are larger than normal scalar registers.
* A **vector length register** specifies how many elements to process if the vector does not always span the full hardware capacity.
* A **vector stride register** tells the hardware how far apart consecutive elements are in memory. This is crucial for non-contiguous data accesses.
* Vector instructions reduce loop overhead and can increase data throughput by letting the pipeline work on different elements in consecutive cycles.

**Benefits**

* Each pipeline stage can process a different element in parallel.
* No intra-vector data dependencies mean simpler hardware scheduling (no waiting for one element to finish before starting the next).
* Regular memory access patterns (especially stride-1) are easy to fetch into registers or caches.

### Strides

When data is laid out in memory, sometimes elements that belong together (e.g., all elements of a column in a matrix) are not next to each other. **Stride** tells the hardware how far to jump in memory to reach the next element of a vector.

#### Row-major layout example

Consider a $4 \times 4$ matrix $A$ in row-major order:

$$
\begin{bmatrix}
A(0,0) & A(0,1) & A(0,2) & A(0,3) \\
A(1,0) & A(1,1) & A(1,2) & A(1,3) \\
A(2,0) & A(2,1) & A(2,2) & A(2,3) \\
A(3,0) & A(3,1) & A(3,2) & A(3,3)
\end{bmatrix}
$$

In memory (row-major), row 0 is first, then row 1, etc.. Accessing a **row** is contiguous ($\text{stride} = 1$), but accessing a **column** is not contiguous ($\text{stride} = \text{number of columns}$)

#### Matrix multiplication

When computing

$$
C = A \times B,
$$

each element $C(i,j)$ is the dot product of row $i$ of $A$ and column $j$ of $B$. Both $A$ and $B$ are also in row-major format. So:

**I. Load row of $A$**

* Elements are in consecutive memory addresses ($\text{stride} = 1$)
* Example: row $i$ might be

$$
[A(i,0), A(i,1), \dots, A(i,n-1)]
$$

**II. Load column of $B$**

* Elements are spaced by the row width of $B$. If $B$ has $m$ columns, the stride is $m$.
* Example: column $j$ might be

$$
[B(0,j), B(1,j), B(2,j), \dots, B(p-1,j)]
$$

**III. Vector multiply and accumulate**

Multiply corresponding elements and sum them up:

$$
C(i,j) = \sum_{k=0}^{n-1} A(i,k) \cdot B(k,j)
$$

By setting a vector stride correctly, a single vector instruction can fetch or process these elements in a single pass. This eliminates the need for complex address computations in a loop, letting the hardware handle it efficiently.

### Loading Storing Vectors from Memory

* When *loading and storing vectors* from memory retrieves multiple elements in one instruction, loop overhead and instruction counts are reduced, whereas scalar loads require many more instructions; for example, a SIMD load can fetch an entire row of a 2-D matrix at once.
* If the *stride between elements* is configurable, the same instruction can access contiguous elements (stride = 1) or spaced elements (stride > 1), whereas a fixed stride limits usable memory layouts; for example, stride-1 supports row-major arrays while stride-n can traverse columns.
* By allowing *elements to be loaded in consecutive cycles* with pipelining, each cycle starts a new access and overlaps completion, whereas without pipelining each access must finish before the next begins; for example, vector loads can achieve one element per cycle once the pipeline is filled.
* Using *banked or interleaved memory* enables multiple simultaneous accesses, which overlaps operations across banks, whereas single-bank memory forces sequential accesses; for example, a four-bank memory can serve four vector elements in parallel.
* Assuming a *stride of one* simplifies address generation to a base pointer plus an increment, whereas arbitrary strides require extra arithmetic each iteration; for example, accessing a contiguous array in row-major order increments the address by one element size per step.

In practice, vector loads/stores are a key part of high-performance computing. When data is well-organized in memory and has a suitable stride, the hardware can fetch multiple elements at once, reducing the number of individual instructions and improving overall throughput.

### Memory Banking

Each bank has its own **Memory Address Register (MAR)** and **Memory Data Register (MDR)**, and they all connect to the **CPU** via a shared **address bus** and **data bus**. The following diagram shows the high-level structure of a system with multiple memory banks:

```
#
                 +----------------+
                 |      CPU       |
                 |  (exec unit,   |
                 |   control)     |
                 +--------+-------+
                          |
             +------------+------------+
             |   Address Bus &         |
             |     Data Bus            |
             +------------+------------+
                          |
         -------------------------------------------------
         |          |           |       ...       |      |
         v          v           v                 v      v
+-----------+  +-----------+  +-----------+   ...   +-----------+
|  Bank 0   |  |  Bank 1   |  |  Bank 2   |         | Bank 15   |
| (MAR,MDR) |  | (MAR,MDR) |  | (MAR,MDR) |         | (MAR,MDR) |
+-----------+  +-----------+  +-----------+   ...   +-----------+
```

* When the *CPU* issues addresses and performs reads or writes through the memory controller, memory operations are coordinated centrally, whereas without this role there is no component directing when or where to access memory; for example, a GPU core issues load/store instructions that translate into memory transactions.
* With an *address bus* carrying addresses from the CPU to all memory banks, each bank receives the same signals but only the bank selected by the relevant bits responds, whereas omitting this mechanism would require separate dedicated address lines for each bank; for example, address bits can determine whether a request goes to Bank 3 or Bank 7.
* Using a shared *data bus* allows data to travel between the CPU and whichever bank is active, whereas having no shared channel would require costly point-to-point wiring; for example, once Bank 5 is addressed, its MDR places the requested word onto the bus for the CPU to read.
* Organizing memory into *banks*, each with its own MAR and MDR, enables parallelism or pipelined access, whereas relying on a single monolithic memory forces serial handling of requests; for example, interleaving across 16 banks lets consecutive vector elements be fetched simultaneously to improve throughput.

Memory banking is used for high-performance vector or GPU architectures because it increases effective memory bandwidth. If each element of a vector is mapped to a different bank, the system can read multiple elements simultaneously or in rapid succession.

### Vectorizable Loops

* A loop is *vectorizable* when its iterations are independent and free of hazards such as one iteration writing values needed by another, whereas a loop with dependencies cannot be parallelized safely; for example, computing each `C[i]` solely from `A[i]` and `B[i]` satisfies independence.
* The loop `for i = 0 to 49, C[i] = (A[i] + B[i]) / 2` qualifies for vectorization because each element is computed separately, whereas a loop referencing `C[i-1]` would introduce dependencies; for example, all 50 averages can be computed simultaneously without waiting for prior iterations.
* In the *vectorized version*, setting the vector length register to 50 and the stride register to 1 signals that one instruction should process all 50 elements contiguously, whereas omitting these registers would force the processor to default to scalar instructions; for example, stride-1 increments address pointers through arrays A, B, and C.
* Executing instructions to load `A` into V0, load `B` into V1, add them into V2, shift V2 right into V3, and then store V3 into `C` enables processing multiple indices per step, whereas scalar instructions repeat these operations per element; for example, a vector add replaces 50 scalar adds.
* Each *instruction latency* depends on the hardware and vector length, with pipelining allowing new elements to enter the pipeline each cycle, whereas no pipelining would require waiting for each element to complete; for example, a vector add may deliver one result per cycle once the pipeline is filled.
* Using a *shift right by one bit* to divide by two is equivalent to computing the average, whereas explicit division requires more cycles; for example, `(A[i] + B[i]) >> 1` performs the same result faster on integer hardware.
* Vectorizing reduces *loop overhead* because a single vector instruction processes multiple data items, whereas scalar execution issues one instruction per element; for example, 50 additions collapse into one vector add instruction, improving both performance and instruction density.

Vectorizable loops are a prime candidate for GPU acceleration. Each GPU thread might handle a portion of the data, or specialized instructions can process multiple elements per thread. In either case, independence between loop iterations makes it easy to parallelize the computation.

### GPUs Are SIMD Engines

* When *GPUs use an instruction pipeline* that behaves like SIMD, a single instruction can operate on many data elements in parallel, whereas scalar execution would process one element at a time; for example, rendering pixels in an image benefits from issuing one instruction across an entire set of pixels.
* Although programmers work with *threads* in CUDA or OpenCL, the hardware actually executes these threads in SIMD groups, whereas treating each thread as fully independent would misrepresent execution; for example, 32 CUDA threads in a warp all run the same instruction together.
* By having each *GPU core execute many threads in lockstep*, the hardware can switch among threads when some stall on memory, whereas cores with fewer threads would sit idle waiting for data; for example, while one warp waits on global memory, another warp can use the compute units.
* The hardware *groups threads into warps or wavefronts* that follow a single instruction stream, enabling data-level parallelism, whereas ignoring this grouping risks inefficient execution; for example, a 32-thread warp on NVIDIA GPUs executes the same add instruction across 32 data values.
* Writing *parallelizable code* that assigns similar work to many threads maps well to the GPU thread model, whereas code with divergent control flow reduces efficiency; for example, summing an array by dividing elements evenly among threads lets all threads execute the same operations in SIMD style.
* Recognizing the *thread-based versus SIMD-based* execution model helps optimize performance, whereas overlooking the SIMD nature can lead to divergence and poor memory access patterns; for example, arranging memory so that consecutive threads access consecutive addresses ensures coalesced loads and better throughput.

### SIMD vs SIMT

* With *SIMD*, a single sequential instruction stream operates on multiple data elements in parallel, whereas without SIMD support each element would require a separate scalar instruction; for example, one `VADD` instruction can add eight numbers at once using vector registers.
* Under *SIMT*, each thread executes its own scalar instruction stream, but hardware groups threads dynamically into warps that execute in lockstep when instructions match, whereas ignoring this model may lead to poor utilization; for example, 32 CUDA threads can run as a warp that behaves like a SIMD unit if their paths are identical.
* Traditional *SIMD hardware* uses explicit vector instructions such as VLD, VADD, and VST, with a vector length register specifying how many elements to process, whereas scalar-only hardware repeats the operation element by element; for example, setting VL = 64 lets one instruction apply to 64 consecutive data points.
* In *SIMT hardware*, developers specify the number of threads to launch in a kernel, and the GPU arranges them into warps for execution, whereas not understanding this arrangement may waste hardware throughput; for example, launching 1024 threads in CUDA results in 32 warps on NVIDIA GPUs.
* One *advantage of SIMT* is that threads can run independently on scalar pipelines, approximating MIMD-style flexibility, whereas SIMD requires strict instruction uniformity; for example, threads in a GPU can diverge onto different branches, even though performance drops if many diverge.
* Another *advantage of SIMT* is its ability to regroup threads dynamically into warps for SIMD-like efficiency when paths align, whereas SIMD has no mechanism to adapt to divergence; for example, threads executing identical arithmetic instructions can be grouped automatically, but diverging threads continue in scalar mode.

```
===============================================
SIMD (Single Instruction, Multiple Data)
===============================================
          +------------------------+
          |  Instruction Fetch    |
          |  (one instruction)    |
          +-----------+-----------+
                      |
                      v
    +-----------------+-----------------+-----------------+-----------------+
    |     Lane 0      |     Lane 1      |     Lane 2      |     Lane 3      |
    |  (data element) |  (data element) |  (data element) |  (data element) |
    +-----------------+-----------------+-----------------+-----------------+

(One instruction is applied to multiple data elements simultaneously 
 in parallel lanes. All lanes share the same Program Counter.)
```

- One instruction stream (shared Program Counter)  
- Multiple data lanes operate in lockstep (e.g., VADD across 4 elements)  
- Typically uses specialized vector registers.  
- Great for uniform data-parallel tasks where each element does the same operation.

```
===============================================
SIMT (Single Instruction, Multiple Threads)
===============================================
           +-------------------------------------------------+
           |         Warp Scheduler / Dispatch Unit          |
           +------------------------+-------------------------+
                                    |
                (Groups threads     |  Same instruction if
                into warps          |  they converge in PC)
                                    v
   +------------+   +------------+   +------------+   +------------+
   |  Thread 0  |   |  Thread 1  |   |  Thread 2  |   |  Thread 3  |
   | Instr. Strm|   | Instr. Strm|   | Instr. Strm|   | Instr. Strm|
   +------------+   +------------+   +------------+   +------------+

(Each thread has its own instruction stream and Program Counter, 
 but threads that execute the same instruction can run in lockstep 
 as a warp, effectively behaving like SIMD when they follow one path.)
```

- Multiple threads, each with its own Program Counter (PC)  
- A hardware scheduler groups threads with the same PC into a warp for SIMD-like execution.  
- If threads diverge, they can continue independently, though performance may drop.  
- Commonly used by GPUs (e.g., CUDA, OpenCL) where thousands of threads can be launched, and hardware dynamically forms warps from these threads.

### Fine-Grained Multithreading

* With *fine-grained multithreading*, the processor fetches an instruction from a different thread each cycle, whereas a single-thread pipeline must wait for one thread to resolve dependencies; for example, a barrel processor can interleave ten threads in rotation to keep issuing instructions.
* By fetching from a new thread while earlier instructions are still resolving, the pipeline reduces idle cycles, whereas sticking to one thread may leave the pipeline stalled on memory; for example, thread B’s computation can proceed while thread A waits for a cache miss.
* Because each thread maintains its own registers and state, there is no extra dependency-tracking logic needed across threads, whereas a single-thread design requires hazard detection and stall mechanisms; for example, switching to another thread avoids hazards caused by back-to-back dependent instructions.
* Overlapping work from multiple threads hides control and data dependence latencies, whereas without overlap the processor idles during stalls; for example, if one thread branches or waits on memory, others can execute to keep the arithmetic units active.
* Keeping multiple threads in flight helps pipeline stages remain utilized, whereas a single-thread pipeline may have stages left empty; for example, the fetch and decode units can serve one thread while the execute unit processes instructions from another.
* Single-thread performance decreases because only a fraction of cycles are devoted to any given thread, whereas dedicating all cycles to one thread yields lower latency; for example, one thread may take longer to complete even though total throughput is higher.
* Supporting multiple *thread contexts* requires additional hardware for program counters, registers, and state storage, whereas a single-thread design needs only one context; for example, a processor with eight contexts must replicate these resources eight times.
* If too few threads are available, the pipeline can still stall despite multithreading, whereas a well-supplied pool of threads hides most delays; for example, when all threads are blocked on long memory accesses, no instructions are available to issue.

### High-level GPU architecture

* A *front-end* containing the program counter, mask registers, instruction cache, and decode logic directs the flow of instructions, whereas omitting such a front-end would leave no mechanism for fetching and decoding instructions; for example, GPU shader cores rely on this unit to supply warps with the correct instructions.
* Multiple *scalar pipelines* combined with a SIMD execution block allow both individual scalar operations and wide data-parallel operations, whereas having only scalar pipelines would limit throughput for vectorizable workloads; for example, arithmetic-heavy pixel shading can exploit SIMD units for parallel math while still handling scalar control code.
* An *interconnection network* links multiple shader cores so they can share data and coordinate execution, whereas without this network each core would operate in isolation; for example, workloads requiring global reductions depend on efficient inter-core communication.
* Multiple *memory controllers* tied to high-bandwidth GDDR memory provide rapid data access for large datasets, whereas a single controller would become a bottleneck; for example, texture fetches and matrix operations benefit from GDDR’s high bandwidth delivered across several controllers in parallel.

```
#
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
                              |  | Scalar Pipe  |              |
                              |  +--------------+              |
                              |         ...                    |
                              |        SIMD Exec               |
                              +---------------+----------------+
                                              |
                                              |
            +----------------------+-----------+--------------------+
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
         |                 Memory Controller(s)                    |
         |                 +--------------+                        |
         |                 |    GDDRx     |  (e.g., GDDR3, GDDR5)  |
         +-----------------+--------------+------------------------+
```

1. **Front-End**  
   - **PC (Program Counter), Mask**: Tracks instruction addresses and active lanes (threads)  
   - **I-Cache**: Holds instructions to be decoded.  
   - **Decode**: Decodes instructions into micro-ops for the pipelines.  
   - **Scalar Pipelines**: Execute scalar operations (e.g., address calculations, control flow)  
   - **SIMD Execution**: Executes vector (wide) operations for parallel data processing (shaders)

2. **Interconnection Network**  
   - Connects the front-end and memory subsystem to multiple **shader cores** (compute units)  

3. **Shader Cores**  
   - Each core has ALUs, registers, and scheduling hardware for parallel threads (shaders)

4. **Memory Controllers + GDDR**  
   - Multiple controllers handle high-bandwidth GDDR memory accesses in parallel.

### General Purpose Programming on GPU

* When *general-purpose processing on GPUs* is applied to data-parallel workloads, thousands of lightweight cores can process large arrays simultaneously, whereas keeping the same workload on CPUs limits parallelism; for example, image filters applied to millions of pixels benefit from GPU acceleration.
* Adopting the *SPMD programming model* allows the same kernel to run across many data points, whereas using distinct programs per data point would be infeasible to manage; for example, in CUDA each thread executes the same kernel but operates on a different element of an array.
* Workloads such as *matrices, image processing, and deep neural networks* exploit GPU parallel execution by mapping repetitive operations to many cores, whereas CPUs process such tasks with fewer concurrent units; for example, multiplying two 4096×4096 matrices scales efficiently across thousands of GPU threads.
* Employing a *GPU programming model* like CUDA or OpenCL provides constructs such as kernels, threads, and blocks to organize parallelism, whereas traditional sequential programming models lack mechanisms for massive concurrency; for example, launching a CUDA kernel spawns tens of thousands of threads to handle individual pixels.
* The *trade-off for flexibility* is higher resource usage and coding complexity, whereas simpler CPU programming requires fewer considerations; for example, managing GPU global memory and thread synchronization adds development effort but unlocks performance gains.
* A *CPU with few out-of-order cores* can efficiently handle diverse workloads with branch-heavy or sequential code, whereas it cannot match GPUs in raw throughput; for example, CPUs excel at running operating systems and compilers but struggle with large matrix multiplications.
* A *GPU with many in-order FGMT cores* maintains high throughput through hardware thread scheduling and fine-grained multithreading, whereas single-threaded performance is weaker; for example, GPUs can keep arithmetic units busy during memory stalls by switching to other ready threads.

**Typical GPU Execution Flow (Three Steps):**  

I. **Load Data to GPU**  

- The CPU (host) allocates GPU (device) memory.  
- Input data is transferred from **host memory** to **device memory**.  

II. **Execute GPU Kernel**  

- A **kernel** function is launched on the GPU.  
- Thousands of **threads** run in parallel on the GPU, accessing device memory to process data.  

III. **Copy Results Back**

- Once the kernel finishes, the CPU transfers the **results** from **device memory** back to **host memory**.  
- The CPU can then use or display the computed results.

Here’s a simplified diagram showing the **basic** data flow:

```
+-------------------------+       Step 1        +------------------------+
|         CPU (Host)      | --(Host->Device)--> |       GPU (Device)     |
|                         | <-(Device->Host)--  |                        |
|   (Runs host code)      |       Step 3        | (Executes GPU kernels) |
+-------------------------+                     +-----------+------------+
                                                   ^
                                                   | Step 2
                                                   |
                                                 Kernel
                                                 Launch
```

* In *Step 1 (Host → Device)*, the CPU allocates GPU memory with functions like `cudaMalloc` and transfers input data using `cudaMemcpy`, whereas skipping this step leaves the GPU without access to necessary inputs; for example, image pixels must be copied to device memory before a filter kernel can run.
* In *Step 2 (Kernel Launch)*, the CPU instructs the GPU to execute a kernel with a launch configuration such as `myKernel<<<blocks, threads>>>(args)`, whereas without this step the GPU performs no computation; for example, a matrix multiplication kernel assigns rows and columns to thousands of threads in parallel.
* In *Step 3 (Device → Host)*, the CPU retrieves results from GPU memory using `cudaMemcpy`, whereas skipping this step is only possible if further GPU processing or direct display output is planned; for example, graphics rendering often leaves images in device memory for display without returning them to the CPU.
* Running *multiple kernels in sequence* on the same data avoids repeated transfers, whereas copying data back and forth between kernels wastes bandwidth; for example, deep learning frameworks keep tensors on the GPU across many layers of computation.
* The *three-step process* of copy-in, run, and copy-out provides a consistent mental model for GPU programming, whereas ignoring it makes memory movement unclear; for example, even optimized pipelines conceptually follow this structure, with optional skips of Step 3.

### Warps and Blocks

* When *CPU threads* handle sequential or modestly parallel tasks while GPU kernels handle massively parallel workloads, each processor type plays to its strengths, whereas assigning irregular control-heavy code to GPUs or bulk-parallel code to CPUs underutilizes resources; for example, a CPU can parse input while a GPU multiplies large matrices.
* With *GPU warps* acting as hardware-level groups of typically 32 threads on NVIDIA devices, threads run in lockstep on the same instruction, whereas programmers cannot directly configure warp formation; for example, 256 threads in a block are automatically divided into eight warps.
* Launching a *GPU kernel* from host code using syntax such as `KernelA<<<nBlk, nThr>>>(args)` specifies grid and block dimensions, whereas omitting this launch leaves the GPU idle; for example, setting `nBlk=64` and `nThr=128` spawns 8192 threads to process data.
* After launch, *each kernel splits into many threads*, with hardware grouping them into warps automatically, whereas manual warp management is unnecessary and not exposed; for example, a 2-D grid of threads is internally decomposed into warps by the GPU scheduler.
* Because the programmer writes *thread-level code* as if each thread runs independently, warp grouping is transparent, whereas exposing warp details would complicate programming; for example, a kernel computing `C[i] = A[i] + B[i]` is written once and applied across all threads.
* In typical *host–device separation*, CPU code runs serially between kernel launches, and GPU code runs in parallel within each kernel, whereas trying to parallelize all control logic on the GPU wastes its efficiency; for example, a CPU can enqueue multiple kernels while the GPU executes thousands of threads.
* Since *warps schedule threads* that execute the same instruction stream, execution is SIMD-like when threads follow the same path, whereas divergence within a warp reduces efficiency; for example, an `if` condition that splits threads forces the warp to execute both paths sequentially.
* This *separation of abstraction* lets GPU hardware optimize warp scheduling internally, while programmers focus on simple thread-based kernels, whereas exposing hardware details would burden developers; for example, CUDA developers write kernels as if for independent threads, even though execution occurs in warps.

Below is a diagram that visualizes how **blocks** and **warps** fit into the overall GPU execution model. Each **grid** consists of one or more **blocks**, and each block contains many **threads**. Under the hood, the GPU hardware groups these threads into **warps** (on NVIDIA GPUs, typically 32 threads per warp)  

```
+-------------------------------------------------------------+
|                    GPU Grid                                 |
| (launched by Kernel<<< numBlocks, threadsPerBlock >>>(...)) | 
|                                                             |
|  +------------------+    +------------------+               |
|  |    Block 0       |    |    Block 1       |               |
|  | (threads)        |    | (threads)        |  ...          |
|  | +--------------+ |    | +--------------+ |               |
|  | |   Warp 0     | |    | |   Warp 0     | |               |
|  | | (th0..th31)  | |    | | (th0..th31)  | |               |
|  | +--------------+ |    | +--------------+ |               |
|  | +--------------+ |    | +--------------+ |               |
|  | |   Warp 1     | |    | |   Warp 1     | |               |
|  | | (th32..th63) | |    | | (th32..th63) | |               |
|  | +--------------+ |    | +--------------+ |               |
|  |       ...        |    |       ...        |               |
|  +------------------+    +------------------+               |
+-------------------------------------------------------------+
```

**How Blocks and Warps Relate**

1. **Grid**: The entire problem domain, composed of multiple blocks. A GPU kernel launch specifies:
   - **Number of blocks** (e.g., `numBlocks`)
   - **Threads per block** (e.g., `threadsPerBlock`)

2. **Blocks**: Each block is an independent group of threads:
   - All threads in a block can cooperate using **shared memory** and **barrier synchronization** (`__syncthreads()` in CUDA)  
   - Blocks are also often arranged in one-, two-, or three-dimensional configurations (e.g., a 2D grid of threads for image processing)

3. **Threads**: Each block has many threads:
   - Each thread has its own registers and local memory.  
   - Threads in the same block can communicate and synchronize.

4. **Warps**: The GPU hardware divides the threads in each block into **warps**:
   - A warp is typically **32 threads** on NVIDIA GPUs (the exact number can vary by architecture/vendor)  
   - All threads in a warp **execute in lockstep** (SIMD fashion)  
   - If threads in the same warp diverge (e.g., different branches of an `if` statement), the warp executes each branch sequentially, reducing overall efficiency.

#### Warp-Based SIMD

* When *GPU cores rely on SIMD pipelines* inside streaming multiprocessors, each multiprocessor can execute a single instruction across many data elements at once, whereas a purely scalar pipeline would process one element per instruction; for example, a streaming multiprocessor may run several warps concurrently for higher throughput.
* A *block of threads is divided into warps* of typically 32 threads on NVIDIA GPUs, which standardizes scheduling, whereas without fixed-size warps the hardware would need more complex grouping logic; for example, a block of 128 threads is split automatically into four warps.
* Because *warps execute in lockstep*, all threads within a warp progress together under the same instruction stream, whereas divergence within a warp forces serialized execution; for example, if all threads perform an add, the GPU achieves full SIMD efficiency.
* Since the *hardware manages warp formation* transparently, programmers focus only on defining grids and blocks, whereas exposing warp management would complicate kernel design; for example, CUDA programmers write kernels for threads, and the driver maps them to warps.
* This *design supports workload balancing* by allowing the GPU to switch among warps when some stall on operations like memory fetches, whereas a pipeline without warp switching would sit idle; for example, if one warp waits on global memory, another warp can be issued to keep compute units active.

#### Comparing Traditional SIMD vs. Warp-Based SIMD

* With *traditional SIMD*, one thread issues a single instruction that drives multiple data lanes in lockstep, whereas scalar execution would handle each element separately; for example, a `VADD` instruction can add four 32-bit integers at once in a 128-bit register.
* In the *programming model for traditional SIMD*, developers explicitly use vector instructions and registers of fixed width, whereas scalar code requires no such constructs; for example, Intel AVX uses 256-bit registers to process eight 32-bit floats at a time.
* Under *warp-based SIMD (SIMT)*, many scalar threads each hold their own registers and program counters, but hardware groups them into warps that execute in lockstep, whereas exposing this grouping would complicate programming; for example, 32 CUDA threads form a warp that runs the same instruction on different data.
* Because the *hardware automatically groups threads into warps*, programmers only define grids and blocks of threads, whereas in traditional SIMD they must manage vector widths; for example, launching 1024 threads in CUDA results in automatic warp creation without specifying vector lengths.
* In *traditional SIMD*, software must declare vector lengths to control how many elements are processed, whereas SIMT hides this detail; for example, an AVX instruction may require the programmer to design loops that align with eight-element chunks.
* With *warp-based SIMD*, vector-length details are concealed from the programmer, making thread-based coding easier, whereas traditional SIMD exposes them; for example, a GPU kernel is written per thread, and the hardware transparently groups 32 threads into a warp.
* *Traditional SIMD ISAs* rely on explicit vector instructions like VLD, VADD, and VST, while warp-based SIMD relies on scalar instructions executed across many threads, whereas conflating the two models leads to confusion; for example, a CUDA thread issues scalar adds, but the GPU executes them in SIMD groups.

### Program Structure in CUDA

A CUDA kernel is a function marked with `__global__` that runs on the GPU across many threads; each thread computes its own index and works on a slice of the data.

```cpp
__global__ void scale(float* d, float s, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) d[i] *= s;
}
```

Before the GPU can touch your data, you allocate memory on the device; the pointer you hold on the CPU is just a handle to memory that physically lives on the GPU.

```cpp
float* d_data = nullptr;
size_t bytes = n * sizeof(float);
cudaError_t err = cudaMalloc(&d_data, bytes);
if (err != cudaSuccess) { /* handle error */ }
```

Inputs must be moved from host RAM to device memory; `cudaMemcpy` does a raw byte copy and you must state the direction explicitly.

```cpp
cudaMemcpy(d_data, h_data, bytes, cudaMemcpyHostToDevice);
```

Kernels are launched with the `<<<grid, block>>>` syntax that sets parallelism; pick a block size (often 128–1024) and compute how many blocks cover your data, then optionally synchronize to surface errors before continuing.

```cpp
int block = 256;
int grid  = (n + block - 1) / block;
scale<<<grid, block>>>(d_data, 2.0f, n);
cudaDeviceSynchronize();  // wait & surface any runtime errors
```

Results live on the device until you copy them back; once you’re done, free the device memory to avoid leaks.

```cpp
cudaMemcpy(h_data, d_data, bytes, cudaMemcpyDeviceToHost);
cudaFree(d_data);
```

Inside a kernel, registers hold most scalars (fast but limited), while `__shared__` memory lets threads in the same block cooperate; use `__syncthreads()` to make sure all threads reach a safe point before using shared results.

```cpp
__global__ void blockSum(const float* in, float* out) {
  __shared__ float buf[256];
  int t = threadIdx.x;
  int i = blockIdx.x * blockDim.x + t;
  buf[t] = in[i];
  __syncthreads();
  // (tiny reduction sketch)
  for (int s = blockDim.x/2; s > 0; s >>= 1) {
    if (t < s) buf[t] += buf[t + s];
    __syncthreads();
  }
  if (t == 0) out[blockIdx.x] = buf[0];
}
```

Below is a table of common CUDA concepts. Each row outlines the **concept**, gives a **brief description**, shows a **typical usage or syntax**, and provides **extra notes** or examples for clarity.

| **Concept**          | **Description**                                                                                                                                                                        | **Syntax / Example**                                                                                                                                                                      | **Extra Notes / Examples**                                                                                                                                                                                                                                          |
|----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Kernel**           | A function executed on the GPU in parallel by many threads. Marked with `__global__`.                                                                                                  | `<br>__global__ void myKernel(float* data) {<br>    // GPU code<br>}<br><br>// Launch<br>myKernel<<<numBlocks, threadsPerBlock>>>(d_data);<br>`                                       | - Each thread runs `myKernel()` independently.<br>- Grids and blocks specify the total thread count.<br>- Often used for data-parallel tasks (e.g., vector addition)                                                                                               |
| **Block**            | A group of threads that can cooperate via shared memory and synchronization.                                                                                                          | Specified in the kernel launch: `<numBlocks, threadsPerBlock>`                                                                                                                              | - Example: `myKernel<<<64, 256>>>(...)` creates 64 blocks, each with 256 threads.<br>- Threads in the same block can access shared memory declared with `__shared__`.                                                                                              |
| **Grid**             | The entire set of blocks launched for a kernel.                                                                                                                                       | Same syntax as block specification but at a higher level (`numBlocks`)                                                                                                                     | - A grid can be 1D, 2D, or 3D, matching the shape of the data (e.g., image processing often uses 2D grids)                                                                                                                 |
| **Thread**           | The basic unit of execution on the GPU. Each thread has its own registers and can identify itself by `threadIdx` within a block and `blockIdx` within the grid.                                                              | `<br>int tx = threadIdx.x;<br>int bx = blockIdx.x;<br>int idx = bx * blockDim.x + tx;<br>`                                                                                           | - `threadIdx`, `blockIdx`, and `blockDim` are built-in variables.<br>- Each thread typically handles one or more elements of the data.                                                                                                                           |
| **Barrier** / `__syncthreads()` | Synchronizes all threads in the same block. No thread passes the barrier until all threads have reached it.                                                                                                    | `<br>__shared__ float temp[256];<br>temp[threadIdx.x] = data[threadIdx.x];<br>__syncthreads();<br>// All threads have updated temp here<br>`                                        | - Only valid within a kernel.<br>- Ensures partial updates in shared memory are visible to all threads before proceeding.<br>- Does **not** synchronize across multiple blocks.                                                                                 |
| **Shared Memory**    | Fast on-chip memory visible to all threads in the same block. Declared with `__shared__`.                                                                                             | `<br>__global__ void myKernel(float* in){<br>    __shared__ float tile[256];<br>    // ...<br>}<br>`                                                                                 | - Useful for caching data reused by threads in a block.<br>- Access is much faster than global memory but limited in size.<br>- Must be accessed carefully to avoid bank conflicts.                                                                              |
| **Global Memory**    | Main device memory accessible by all threads but with higher access latency than shared memory.                                                                                       | Allocated via `cudaMalloc((void**)&d_ptr, size);`<br>Accessed in kernels by pointer (`d_ptr[index]`)                                                                                       | - Large capacity but slower than shared or local memory.<br>- Important to ensure coalesced access (threads access consecutive addresses) for best performance.                                                                                                   |
| **Host Code**        | CPU-side code that sets up kernel launches, manages device memory, and coordinates data transfers.                                                                                    | `<br>// Allocate GPU memory<br>cudaMalloc((void**)&d_data, size);<br>// Launch kernel<br>myKernel<<<blocks, threads>>>(d_data);<br>// Copy results back<br>cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);<br>` | - The host is responsible for orchestrating GPU operations (allocation, copy, launch)<br>- Synchronization functions (`cudaDeviceSynchronize()`) can ensure the GPU has finished before using results on the CPU.                                                |
| **Device Function**  | A function that runs on the GPU but is **only** called from another GPU function (kernel or device function). Declared with `__device__`.                                             | `<br>__device__ float deviceFunc(float x) {<br>    return x * x;<br>}<br><br>__global__ void kernelFunc(float* data){<br>    data[threadIdx.x] = deviceFunc(data[threadIdx.x]);<br>}<br>`                                 | - Not callable from the host.<br>- Often used to break large kernels into smaller subroutines.                                                                                                                                                                    |
| **Host Function**    | Normal CPU function compiled for the host. Marked with `__host__` (optional unless combined with `__device__` in a function qualifier).                                               | `<br>__host__ void hostFunc(){<br>    // Runs on CPU<br>}<br>`                                                                                                                      | - Host code typically handles memory management, kernel launches, etc.<br>- `__host__ __device__` can make a function callable from both CPU and GPU, although limitations apply.                                                                              |

- This table is not exhaustive. Advanced CUDA concepts include streams, events, unified memory, texture memory, constant memory, warp-level primitives, cooperative groups, and more.  
- The examples shown above are simplified to illustrate syntax and usage. Real-world code often includes error checking on all CUDA function calls (e.g., checking the return value of `cudaMalloc`, `cudaMemcpy`, etc.)  
- The concept of **warps** is a lower-level detail (hardware grouping of threads). It is usually not directly controlled in the kernel code, but it’s useful to understand for performance tuning.
