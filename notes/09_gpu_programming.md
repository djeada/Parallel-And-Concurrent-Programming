## GPU Programming

GPUs (Graphics Processing Units) are throughput-oriented processors designed to apply similar operations across large numbers of data elements in parallel. They were originally built for graphics workloads, where millions of pixels or vertices must be processed, but the same architecture is also effective for scientific simulation, image processing, machine learning, numerical computing, and other data-parallel workloads.

**What makes GPU programming different?**

- GPUs contain many execution lanes organized for massive parallelism.
- They use a specialized memory hierarchy that includes global memory, shared memory, registers, caches, constant memory, texture memory, and banked memory structures.
- Programming models such as CUDA, HIP, OpenCL, and SYCL let developers launch large numbers of lightweight threads or work-items.
- High performance depends on memory access patterns, thread organization, occupancy, synchronization, and limiting unnecessary branch divergence.

> A GPU is fast when many threads perform similar work on different data with predictable memory access.

### Time-Space Duality

In parallel architecture, **time-space duality** describes two complementary ways to achieve high throughput:

1. Use many replicated processing elements at the same time (**spatial parallelism**).
2. Reuse fewer processing elements over time through a pipeline (**temporal parallelism**).

Modern GPUs combine both ideas: they contain many replicated execution resources, and those resources are also pipelined.

```
====================================================
ARRAY PROCESSOR: Spatial Parallelism
====================================================
         |   T0   |   T1   |   T2   |   T3   |
----------------------------------------------------
 PE0     |  LD0   |  ADD0  |  MUL0  |  ST0   |
 PE1     |  LD1   |  ADD1  |  MUL1  |  ST1   |
 PE2     |  LD2   |  ADD2  |  MUL2  |  ST2   |
 PE3     |  LD3   |  ADD3  |  MUL3  |  ST3   |
----------------------------------------------------
Many processing elements work at the same time.

====================================================
VECTOR PIPELINE: Temporal Parallelism
====================================================
Time -->    T0     T1     T2     T3     T4     T5     T6
----------------------------------------------------------------
 Elem0:     LD0    ADD0   MUL0   ST0
 Elem1:            LD1    ADD1   MUL1   ST1
 Elem2:                   LD2    ADD2   MUL2   ST2
 Elem3:                          LD3    ADD3   MUL3   ST3
----------------------------------------------------------------
Each element moves through pipeline stages over time.
```

**Analogy:** Imagine a bakery that must make thousands of identical cookies before sunrise. One strategy is to give many bakers identical workstations so they can perform the same step on different batches at the same time. This resembles an **array processor**, where many processing elements work side by side. Another strategy is to use a pipelined cookie-making machine: while one batch is being shaped, the next can already be mixed and another can be loaded. This resembles a **vector pipeline**, where hardware stages are reused over time. A modern GPU combines both ideas by providing many parallel execution resources, each backed by deeply pipelined hardware.

#### I. Array Processors: Spatial Parallelism

An array processor contains multiple processing elements that work on different data elements, often under a common instruction stream.

- Many elements are processed at the same time.
- Throughput is high when all processing elements have useful work.
- Hardware cost is higher because more execution resources are replicated.
- This model works well for image filters, vector arithmetic, matrix operations, and stencil computations.

#### II. Vector Processors: Temporal Parallelism

A vector processor applies one vector instruction to many elements. The hardware may pipeline the operation so that a new element enters the pipeline every cycle.

- The same functional units are reused across elements over time.
- Loop overhead is reduced because one instruction represents many scalar operations.
- Performance is best when memory access is regular and predictable.
- Startup cost matters: short vectors may not fully utilize the pipeline.

#### How This Relates to GPUs

A modern GPU is not just an array processor or just a vector processor. It combines:

- **Spatial parallelism:** many streaming multiprocessors / compute units.
- **Temporal parallelism:** pipelined execution inside each unit.
- **Thread-level parallelism:** many resident warps or wavefronts hide latency.
- **SIMT execution:** programmers write scalar thread code, while hardware executes groups of threads in SIMD-like fashion.

### Vector Processor

A **vector processor** executes operations on vectors, which are ordered collections of elements. Instead of issuing one scalar instruction per element, a vector instruction expresses the same operation across many elements.

For example, a vector add instruction can represent:

$$
C[i] = A[i] + B[i] \quad \text{for many values of } i
$$

Important vector-processor concepts include:

| Concept | Meaning |
|---|---|
| **Vector register** | A register that stores multiple elements instead of one scalar value. |
| **Vector length** | The number of elements to process in the current vector instruction. |
| **Vector stride** | The spacing between consecutive elements in memory. |
| **Vector pipeline** | A pipeline that processes one or more vector elements per cycle after startup. |

**Benefits**

- Fewer loop-control instructions.
- High arithmetic throughput on regular data.
- Efficient memory access for contiguous or predictable strides.
- Less scheduling complexity when elements are independent.

**Limitations**

- Poor fit for irregular control flow.
- Poor fit for pointer-heavy or dependency-heavy code.
- Performance drops when memory access is scattered or badly aligned.

### Strides

A **stride** is the distance in memory between consecutive elements of a logical vector.

- **Stride 1:** consecutive elements are adjacent in memory.
- **Stride k:** consecutive elements are separated by `k` elements in memory.

Stride matters because regular, contiguous memory access is usually more efficient than scattered access. On GPUs, adjacent threads should ideally access nearby addresses so the hardware can combine their requests into a small number of memory transactions.

#### Row-Major Layout Example

Consider a $4 \times 4$ matrix $A$ stored in row-major order:

$$
A =
\begin{bmatrix}
A(0,0) & A(0,1) & A(0,2) & A(0,3) \\
A(1,0) & A(1,1) & A(1,2) & A(1,3) \\
A(2,0) & A(2,1) & A(2,2) & A(2,3) \\
A(3,0) & A(3,1) & A(3,2) & A(3,3)
\end{bmatrix}
$$

In row-major memory, the layout is:

```text
A(0,0), A(0,1), A(0,2), A(0,3),
A(1,0), A(1,1), A(1,2), A(1,3),
A(2,0), A(2,1), A(2,2), A(2,3),
A(3,0), A(3,1), A(3,2), A(3,3)
```

| Access pattern | Example | Stride |
|---|---|---|
| Row access | `A(2,0), A(2,1), A(2,2), A(2,3)` | `1` |
| Column access | `A(0,1), A(1,1), A(2,1), A(3,1)` | number of columns |

So in a row-major $N \times M$ matrix, moving across a row has stride `1`, while moving down a column has stride `M`.

#### Matrix Multiplication Example

For matrix multiplication,

$$
C = A \times B
$$

where $A$ is $m \times n$, $B$ is $n \times p$, and $C$ is $m \times p$:

$$
C(i,j) = \sum_{k=0}^{n-1} A(i,k) \cdot B(k,j)
$$

To compute one element $C(i,j)$:

I. Load row $i$ of $A$:

$$
[A(i,0), A(i,1), \dots, A(i,n-1)]
$$

In row-major order, this is contiguous, so the stride is `1`.

II. Load column $j$ of $B$:

$$
[B(0,j), B(1,j), \dots, B(n-1,j)]
$$

In row-major order, this is not contiguous. Since $B$ has $p$ columns, the stride is `p`.

III. Multiply corresponding elements and accumulate the sum.

**GPU performance note:** A single thread walking down a column of a row-major matrix performs strided accesses. Whether those accesses are coalesced at the GPU level also depends on how neighboring threads map to matrix rows and columns. Optimized matrix multiplication usually uses tiling so that blocks load contiguous tiles of $A$ and $B$ into shared memory and reuse those values many times.

### Loading and Storing Vectors from Memory

Vector loads and stores move multiple elements between memory and registers. A closely related GPU concern is **coalesced memory access**, where threads in the same warp access nearby addresses that the hardware can serve with a small number of memory transactions.

| Pattern | Effect |
|---|---|
| Consecutive addresses | Usually efficient and coalesced. |
| Large stride | May require more memory transactions. |
| Random addresses | Often slow because requests cannot be combined well. |
| Aligned addresses | Usually better than misaligned addresses. |

#### Why Memory Layout Matters

```text
Good pattern: consecutive threads read consecutive elements
Thread:   T0   T1   T2   T3   T4   T5   T6   T7
Address:  A0   A1   A2   A3   A4   A5   A6   A7

Poor pattern: consecutive threads read far-apart elements
Thread:   T0   T1   T2   T3   T4   T5   T6   T7
Address:  A0   A8   A16  A24  A32  A40  A48  A56
```

Efficient GPU programs often organize data so adjacent threads access adjacent elements. This is one reason an array of structures is sometimes converted into a structure of arrays.

```cpp
// Array of Structures: convenient, but fields may be interleaved
typedef struct {
  float x, y, z;
} Particle;
Particle particles[N];

// Structure of Arrays: often better for coalesced GPU access
float x[N];
float y[N];
float z[N];
```

### Memory Banking

**Memory banking** divides storage into multiple banks so several accesses can be serviced in parallel. The idea appears in vector machines, GPU shared memory, register files, caches, and memory controllers.

A simplified banked-memory system looks like this:

```text
                 +----------------+
                 |  Processor /   |
                 | Memory Control |
                 +--------+-------+
                          |
        +-----------------+-----------------+
        |                 |                 |
        v                 v                 v
   +---------+       +---------+       +---------+      ...
   | Bank 0  |       | Bank 1  |       | Bank 2  |
   +---------+       +---------+       +---------+
```

If consecutive addresses map to different banks, the system can serve multiple accesses efficiently:

```text
Address:  0   1   2   3   4   5   6   7
Bank:     0   1   2   3   0   1   2   3
```

If many threads access different addresses that map to the same bank, a **bank conflict** may occur. The hardware may need to serialize some of those accesses, reducing throughput.

#### Example: Shared Memory Bank Conflict

```text
Good: each thread hits a different bank
T0 -> Bank 0
T1 -> Bank 1
T2 -> Bank 2
T3 -> Bank 3

Bad: several threads hit the same bank
T0 -> Bank 0
T1 -> Bank 0
T2 -> Bank 0
T3 -> Bank 0
```

Banked memory increases bandwidth only when access patterns spread requests across banks. Poor strides can create bank conflicts.

#### CUDA Shared Memory Banks and Padding

On NVIDIA GPUs, shared memory has 32 banks, and successive 32-bit words map to successive banks, so word `k` is in bank `k % 32`. A conflict occurs when threads of one warp access *different* words in the same bank. When several threads read the *same* word, it is broadcast to them, so there is no conflict. If each thread reads `s[threadIdx.x * 32]` from a `float` array, every address falls in bank 0 and the 32 accesses are serialized. A stride of 2 words gives a 2-way conflict.

A common case is reading a column of a `32 x 32` tile, for example in a matrix transpose. Adding one padding column shifts each row by one bank, so a column read touches 32 different banks:

```cpp
__shared__ float tile[32][32];  // tile[t][c] for t = 0..31 all map to one bank
__shared__ float tile[32][33];  // padded: tile[t][c] is in bank (t * 33 + c) % 32,
                                //         which differs for every t
```

### Vectorizable Loops

A loop is **vectorizable** when its iterations are sufficiently independent to execute in parallel without changing the program's result.

#### Vectorizable Loop

```cpp
for (int i = 0; i < n; ++i) {
  C[i] = A[i] + B[i];
}
```

Each iteration reads `A[i]` and `B[i]` and writes `C[i]`. No iteration depends on another, so the loop can be vectorized or mapped to GPU threads.

#### Not Safely Vectorizable Without Transformation

```cpp
for (int i = 1; i < n; ++i) {
  C[i] = C[i - 1] + A[i];
}
```

Here, `C[i]` depends on `C[i - 1]`, which was computed by the previous iteration. This loop has a loop-carried dependency.

#### Average Example

```cpp
for (int i = 0; i < 50; ++i) {
  C[i] = (A[i] + B[i]) / 2;
}
```

This is vectorizable because each output element is independent. For integer arrays, a shift such as `(A[i] + B[i]) >> 1` may look like an alternative to division by 2, but modern compilers often optimize division by powers of two automatically, and the two expressions are not always equivalent:

- Signed right shifts and integer division can differ for negative values.
- `A[i] + B[i]` can overflow before either the shift or division.
- The shift form is not appropriate for floating-point averages.

A safer integer average may require a wider intermediate type or a formula that avoids overflow.

### GPUs Are SIMT Engines

GPUs are often described as SIMD-like, but many GPU programming models are better described as **SIMT: Single Instruction, Multiple Threads**.

In SIMT:

- The programmer writes code as if each thread is scalar and independent.
- The hardware groups threads into fixed execution groups: **warps** on NVIDIA GPUs and **wavefronts/waves** on AMD GPUs.
- Threads in the same group normally issue instructions together across active lanes.
- If threads diverge into different branches, the hardware tracks active lanes and executes the required paths separately, reducing efficiency.

#### Good GPU Workload

```cpp
C[i] = A[i] + B[i];
```

All threads perform the same operation on different elements.

#### Less Efficient GPU Workload

```cpp
if (A[i] > threshold) {
  C[i] = expensive_path(A[i]);
} else {
  C[i] = cheap_path(A[i]);
}
```

If neighboring threads choose different branches, the warp may have to run both paths with different active lanes.

### SIMD vs. SIMT

| Feature | SIMD | SIMT |
|---|---|---|
| Programming view | Explicit vector operations | Many scalar-looking threads |
| Execution group | Vector lanes | Warps / wavefronts |
| Control flow | One instruction stream for all lanes | Threads have their own logical control flow, but warp execution is masked/serialized on divergence |
| Programmer controls | Vector width and vector instructions | Grid, blocks, and threads |
| Example | AVX vector add | CUDA kernel with many threads |

#### SIMD

```text
===============================================
SIMD: Single Instruction, Multiple Data
===============================================
          +------------------------+
          |  Instruction Fetch     |
          |  one instruction       |
          +-----------+------------+
                      |
                      v
    +-----------+-----------+-----------+-----------+
    |  Lane 0   |  Lane 1   |  Lane 2   |  Lane 3   |
    +-----------+-----------+-----------+-----------+
```

A single vector instruction drives several data lanes.

#### SIMT

```text
===============================================
SIMT: Single Instruction, Multiple Threads
===============================================
              +----------------------+
              |   Warp Scheduler     |
              +----------+-----------+
                         |
                         v
    +-----------+-----------+-----------+-----------+
    | Thread 0  | Thread 1  | Thread 2  | Thread 3  |
    | scalar PC | scalar PC | scalar PC | scalar PC |
    +-----------+-----------+-----------+-----------+
```

Each thread has its own logical execution state, while the hardware schedules and issues work in groups of threads.

> **Note:** Warps are fixed groups of neighboring threads, not arbitrary groups that are dynamically re-formed each cycle. Divergence is handled through per-lane execution state and reconvergence mechanisms rather than by regrouping matching threads across the GPU.

### Fine-Grained Multithreading

**Fine-grained multithreading** interleaves instructions from many resident hardware thread groups to hide latency. GPUs use this idea heavily.

When one warp stalls on a long-latency operation, the scheduler can issue instructions from another ready warp without performing a heavyweight software context switch.

```text
Cycle:       0      1      2      3      4      5
Issued:   Warp0  Warp1  Warp2  Warp3  Warp0  Warp1
```

This improves throughput because execution units spend less time idle.

#### Benefits

- Hides memory latency.
- Keeps pipelines busy.
- Reduces the need for complex out-of-order execution.
- Works well when many independent warps are resident.

#### Costs

- Each resident thread needs registers and state.
- Single-thread latency may be poor compared with CPUs.
- Too many registers per thread can reduce occupancy.
- If all resident warps stall, the GPU still cannot issue work.

### High-Level GPU Architecture

A modern GPU is organized around many compute units, commonly called **Streaming Multiprocessors (SMs)** on NVIDIA GPUs and **Compute Units (CUs)** on AMD GPUs.

```text
+------------------------------------------------------------------+
|                              GPU                                 |
|                                                                  |
|  +----------------+   +----------------+   +----------------+    |
|  | SM / CU        |   | SM / CU        |   | SM / CU        |    |
|  | - schedulers   |   | - schedulers   |   | - schedulers   |    |
|  | - registers    |   | - registers    |   | - registers    |    |
|  | - shared mem   |   | - shared mem   |   | - shared mem   |    |
|  | - ALUs / lanes |   | - ALUs / lanes |   | - ALUs / lanes |    |
|  +-------+--------+   +-------+--------+   +-------+--------+    |
|          |                    |                    |             |
|          +--------------------+--------------------+             |
|                               |                                  |
|                       +-------v-------+                          |
|                       |      L2       |                          |
|                       |     Cache     |                          |
|                       +-------+-------+                          |
|                               |                                  |
|                  +------------v-------------+                    |
|                  | Memory Controllers       |                    |
|                  | GDDR / HBM Device Memory |                    |
|                  +--------------------------+                    |
+------------------------------------------------------------------+
```

#### Main Components

| Component | Role |
|---|---|
| **SM / CU** | Executes thread blocks using warps or wavefronts. |
| **Warp scheduler** | Chooses ready warps to issue each cycle. |
| **Registers** | Fast per-thread storage. High register use can reduce occupancy. |
| **Shared memory** | Fast per-block memory for cooperation among threads. |
| **L1 / texture / constant caches** | Accelerate specific access patterns. |
| **L2 cache** | Shared cache across the GPU. |
| **Memory controllers** | Connect the GPU to high-bandwidth device memory such as GDDR or HBM. |

### General-Purpose Programming on GPUs

**General-purpose GPU programming (GPGPU)** uses GPUs for non-graphics computation. It works best when a workload exposes substantial data parallelism.

Good GPU workloads usually have:

- Many independent elements.
- High arithmetic intensity or coalesced memory access.
- Limited synchronization.
- Regular control flow.
- Enough work to amortize launch overhead and, on discrete GPUs, host-device transfer costs.

Poor GPU workloads often have:

- Heavy or highly irregular branching.
- Small input sizes.
- Irregular pointer chasing.
- Strong sequential dependencies.
- Frequent CPU-GPU transfers on systems with separate host and device memory.

#### Typical GPU Execution Flow

```text
+-------------------------+       1. Copy input        +------------------------+
|      CPU / Host         | -------------------------> |      GPU / Device      |
|                         |                            |                        |
| Runs host code          |       3. Copy output       | Executes kernels       |
| Allocates device memory | <------------------------- | Stores device data     |
+-------------------------+                            +-----------+------------+
                                                               ^
                                                               |
                                                       2. Kernel launch
```

I. **Copy input data to the GPU**

- Allocate device memory.
- On a discrete-memory system, transfer input arrays from host memory to device memory.

II. **Launch a GPU kernel**

- The CPU launches a GPU function called a kernel.
- The kernel runs across many blocks and threads.

III. **Copy results back to the CPU**

- Transfer output data from device memory back to host memory.
- Free device memory when finished.

> **Performance note:** On discrete GPUs, transfers across PCIe or other host-device links can be expensive. Reusing data on the GPU across multiple kernels is often faster than repeatedly moving it between host and device memory.

### Warps and Blocks

CUDA organizes GPU work into a hierarchy:

```text
Grid
└── Block 0
    ├── Warp 0: threads 0-31
    ├── Warp 1: threads 32-63
    └── ...
└── Block 1
    ├── Warp 0
    ├── Warp 1
    └── ...
```

A kernel launch specifies a **grid** of **blocks**, and each block contains **threads**.

```cpp
kernel<<<numBlocks, threadsPerBlock>>>(args);
```

#### Grid

The grid is the full set of blocks launched for one kernel. It represents the whole problem.

#### Block

A block is a group of threads that can cooperate.

- Threads in the same block can share `__shared__` memory.
- Threads in the same block can synchronize with `__syncthreads()`.
- Blocks are normally scheduled independently, so a conventional kernel should not rely on execution order or direct synchronization between blocks.

A block barrier has block scope. In a conventional kernel, `__syncthreads()` cannot make another block finish first. If one phase needs results from every block, a separate kernel in the same stream is a common way to order the next phase after the first completes.

All participating threads must reach a block barrier consistently. In a reduction, threads whose input index is out of bounds can contribute zero and still take part in the barrier; placing the barrier only inside `if (i < n)` can leave threads following different synchronization paths. See [CUDA block synchronization](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/intro-to-cuda-cpp.html).

#### Thread

A thread is the basic programming unit. Each thread has:

- Its own registers.
- Its own local variables.
- Built-in indices such as `threadIdx`, `blockIdx`, and `blockDim`.

#### Warp / Wavefront

A warp is a hardware execution group. On NVIDIA GPUs, a warp contains 32 threads. On AMD GPUs, the corresponding group is usually called a wavefront or wave, and its width depends on the architecture and execution mode: 64 lanes on GCN and CDNA GPUs, and 32 or 64 lanes on RDNA GPUs. Portable code should query the width (for example `warpSize` in CUDA and HIP) instead of hard-coding 32.

```text
+-------------------------------------------------------------+
|                         GPU Grid                            |
|                                                             |
|  +------------------+    +------------------+               |
|  |    Block 0       |    |    Block 1       |      ...      |
|  |                  |    |                  |               |
|  | +--------------+ |    | +--------------+ |               |
|  | | Warp 0       | |    | | Warp 0       | |               |
|  | | th0..th31    | |    | | th0..th31    | |               |
|  | +--------------+ |    | +--------------+ |               |
|  | +--------------+ |    | +--------------+ |               |
|  | | Warp 1       | |    | | Warp 1       | |               |
|  | | th32..th63   | |    | | th32..th63   | |               |
|  | +--------------+ |    | +--------------+ |               |
|  +------------------+    +------------------+               |
+-------------------------------------------------------------+
```

#### CUDA and OpenCL Terminology

OpenCL uses different names for the same ideas. A CUDA thread corresponds to an OpenCL **work-item**, a block to a **work-group**, and the grid to the **NDRange**. Memory shared by one block, CUDA `__shared__`, is OpenCL `__local` memory, while CUDA global memory is OpenCL `__global` memory. Note that CUDA "local memory" means something else: per-thread storage that physically lives in device memory.

| CUDA | OpenCL |
|---|---|
| Thread | Work-item |
| Block | Work-group |
| Grid | NDRange |
| `__shared__` memory | `__local` memory |
| Global memory | `__global` memory |
| `threadIdx.x` | `get_local_id(0)` |
| `blockIdx.x * blockDim.x + threadIdx.x` | `get_global_id(0)` |

### Warp-Based SIMD

A warp executes one instruction across many threads. When all lanes are active and follow the same path, the warp behaves like a SIMD vector operation.

#### Efficient Warp Execution

```cpp
int i = blockIdx.x * blockDim.x + threadIdx.x;
C[i] = A[i] + B[i];
```

Every thread executes the same instruction sequence.

#### Divergent Warp Execution

```cpp
if (threadIdx.x % 2 == 0) {
  C[i] = A[i] + B[i];
} else {
  C[i] = A[i] - B[i];
}
```

Threads in the same warp take different branches. The hardware may execute both paths while masking off inactive lanes, reducing efficiency.

#### Traditional SIMD vs. Warp-Based SIMT

| Traditional SIMD | Warp-Based SIMT |
|---|---|
| Programmer uses explicit vector instructions. | Programmer writes scalar thread code. |
| Vector width is visible in software. | Warp width is mostly hidden from source code. |
| One instruction controls vector lanes. | One warp instruction controls active thread lanes. |
| Best for regular vector loops. | Best for large thread-parallel workloads. |

### Program Structure in CUDA

A CUDA program usually has two parts:

1. **Host code** running on the CPU.
2. **Device code** running on the GPU.

#### A Simple Kernel

A CUDA kernel is marked with `__global__`. It runs on the GPU and is launched from host code.

```cpp
__global__ void scale(float* d, float s, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    d[i] *= s;
  }
}
```

#### Allocate Device Memory

```cpp
float* d_data = nullptr;
size_t bytes = n * sizeof(float);

cudaError_t err = cudaMalloc(&d_data, bytes);
if (err != cudaSuccess) {
  // Handle allocation error
}
```

The CPU variable `d_data` is a device pointer that refers to memory allocated in the GPU's address space.

#### Copy Data to the GPU

```cpp
cudaMemcpy(d_data, h_data, bytes, cudaMemcpyHostToDevice);
```

The copy direction must be specified explicitly.

#### Launch the Kernel

```cpp
int block = 256;
int grid  = (n + block - 1) / block;

scale<<<grid, block>>>(d_data, 2.0f, n);
cudaDeviceSynchronize();
```

The grid size is rounded up so every element is covered. The bounds check inside the kernel prevents out-of-range memory access. With `n = 1000` and `block = 256`, the grid has 4 blocks (1,024 threads), and the last 24 threads fail `i < n`. Writing `grid = n / block` instead would launch only 3 blocks and leave elements 768–999 unprocessed.

On current GPUs a block may contain at most 1,024 threads. A launch that exceeds a limit fails with a configuration error and the kernel does not run.

#### Grid-Stride Loops

Instead of sizing the grid to the data, a kernel can loop with a stride equal to the total number of threads. Each thread then handles elements `i`, `i + stride`, `i + 2 * stride`, and so on, so a fixed-size grid covers any `n`.

```cpp
__global__ void scaleGridStride(float* d, float s, int n) {
  int stride = blockDim.x * gridDim.x;
  for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += stride) {
    d[i] *= s;
  }
}
```

With 2 blocks of 4 threads and `n = 20`, the stride is 8, so the thread with global index 5 processes elements 5 and 13.

#### Copy Results Back and Free Memory

```cpp
cudaMemcpy(h_data, d_data, bytes, cudaMemcpyDeviceToHost);
cudaFree(d_data);
```

`cudaMemcpy` is synchronous with respect to the host: it returns only after the copy has completed. It is issued to the default stream, and work in one stream runs in issue order, so when the kernel was also launched in the (legacy) default stream, the device-to-host copy waits for it to finish. No extra synchronization is needed for correctness.

#### Checking for Errors

A kernel launch does not return an error code, and it is asynchronous: control returns to the host before the kernel runs. Errors therefore show up in two places:

- **Launch errors**, such as an invalid configuration, are reported by `cudaGetLastError()` (which also resets the last error) or `cudaPeekAtLastError()` right after the launch.
- **Execution errors**, such as an invalid memory access inside the kernel, appear only after the kernel has run, in the return value of a later synchronizing call such as `cudaDeviceSynchronize()` or `cudaMemcpy()`. Some of these errors leave the context unusable, so later calls keep failing.

```cpp
scale<<<grid, block>>>(d_data, 2.0f, n);
cudaError_t err = cudaGetLastError();        // launch errors
if (err == cudaSuccess) {
  err = cudaDeviceSynchronize();             // errors during execution
}
if (err != cudaSuccess) {
  fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(err));
}
```

For the same reason, timing a kernel with a host clock around the launch measures only the launch. Synchronize first, or record `cudaEvent_t` events in the stream and use `cudaEventElapsedTime()`.

### Pinned Memory, Streams, and Overlap

Ordinary `malloc` or `new` memory is **pageable**: the operating system may move or swap it. The GPU's copy engines transfer data by DMA, which needs **page-locked (pinned)** host memory, allocated with `cudaMallocHost()` or `cudaHostAlloc()` and released with `cudaFreeHost()`. Pinned memory often copies faster and is necessary for truly asynchronous transfers with `cudaMemcpyAsync()`; with pageable memory, the copy is staged through a pinned buffer and generally cannot overlap with other work. Pinning too much memory can degrade overall system performance, because the OS can no longer page it.

A **stream** is an ordered queue of GPU work. Operations in one stream run in issue order, while operations in different streams may overlap. Splitting data into chunks and giving each chunk its own stream lets copies for one chunk overlap with kernels for another, depending on the GPU's copy engines and available resources:

```cpp
// d: device buffer of n floats, allocated with cudaMalloc
float* h;                                   // pinned host buffer
cudaMallocHost(&h, bytes);

cudaStream_t s[2];
for (int k = 0; k < 2; ++k) cudaStreamCreate(&s[k]);

size_t half = n / 2;                        // assume n is even
for (int k = 0; k < 2; ++k) {
  size_t off = k * half;
  cudaMemcpyAsync(d + off, h + off, half * sizeof(float),
                  cudaMemcpyHostToDevice, s[k]);
  scale<<<(half + 255) / 256, 256, 0, s[k]>>>(d + off, 2.0f, half);
  cudaMemcpyAsync(h + off, d + off, half * sizeof(float),
                  cudaMemcpyDeviceToHost, s[k]);
}
cudaDeviceSynchronize();                    // wait for both streams
```

Work launched without a stream argument goes to the **default stream**. By default this is the *legacy* default stream, which synchronizes implicitly with other blocking streams in the same context: a kernel in it waits for earlier work in those streams, and later work in them waits for it. One stray default-stream launch can therefore serialize an overlapped pipeline. Streams created with `cudaStreamCreateWithFlags(&s, cudaStreamNonBlocking)`, or compiling with `nvcc --default-stream per-thread`, avoid this implicit synchronization.

### Shared Memory Example: Block Reduction

Shared memory lets threads in a block cooperate. The example below computes one partial sum per block.

```cpp
__global__ void blockSum(const float* in, float* out, int n) {
  extern __shared__ float buf[];

  int t = threadIdx.x;
  int i = blockIdx.x * blockDim.x + threadIdx.x;

  buf[t] = (i < n) ? in[i] : 0.0f;
  __syncthreads();

  for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
    if (t < stride) {
      buf[t] += buf[t + stride];
    }
    __syncthreads();
  }

  if (t == 0) {
    out[blockIdx.x] = buf[0];
  }
}
```

Launch example:

```cpp
int block = 256;
int grid = (n + block - 1) / block;
size_t sharedBytes = block * sizeof(float);

blockSum<<<grid, block, sharedBytes>>>(d_in, d_partial, n);
```

This version addresses two common issues:

- It checks `i < n` before reading input.
- It uses dynamically sized shared memory so the buffer matches the block size.

The halving reduction shown here assumes a power-of-two block size, such as the `256` threads used in the launch example. With `blockDim.x == 192`, the strides become 96, 48, 24, 12, 6, 3, 1; after stride 3, `buf[2]` holds a partial sum that the final stride-1 step never adds to `buf[0]`, so the result is wrong.

The kernel produces one partial sum per block. Combining them needs results from every block, which a block cannot safely wait for inside the same conventional kernel: blocks are not guaranteed to run concurrently or in any order. Launch a second kernel, finish on the host, or use atomics as shown below.

### Atomic Operations

When many threads update the same memory location, a plain `x += 1` is a read-modify-write race: two threads can read the same old value and one update is lost. A histogram written with `bins[v] += 1` therefore tends to come out too low. `__syncthreads()` does not fix this, since it only orders threads of one block. **Atomic functions** such as `atomicAdd`, `atomicSub`, `atomicMin`, `atomicMax`, `atomicExch` and `atomicCAS` perform the whole read-modify-write as one indivisible operation.

```cpp
__global__ void histogram(const int* in, int* bins, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    atomicAdd(&bins[in[i]], 1);
  }
}
```

`atomicAdd` returns the value stored *before* the addition. This makes it useful for handing out unique slots, for example to compact selected elements into an output array; every thread gets a distinct index, but the order is unspecified:

```cpp
int slot = atomicAdd(&count, 1);   // count starts at 0
out[slot] = value;
```

Atomics on the same address are serialized, so having every thread in a large grid update one global counter creates a bottleneck. A common improvement is to reduce within each block first (in registers or shared memory, as in `blockSum`) and then issue one atomic per block:

```cpp
if (threadIdx.x == 0) {
  atomicAdd(total, buf[0]);   // one global atomic per block
}
```

Atomics with a narrower scope, such as shared-memory atomics within a block, are generally cheaper than device-wide or system-wide ones.

### Occupancy, Utilization, and Performance

**Occupancy** is the number of resident warps on an SM relative to its supported maximum. Registers, shared memory, block size, and hardware limits determine how much work can reside there at once.

Resident warps are not necessarily executing useful instructions every cycle. **Utilization** describes how actively resources are used, while achieved throughput measures useful work per unit of time. Raising occupancy may help hide latency, but can also require reducing per-thread resources or data reuse. A lower-occupancy kernel can be faster. Compare measured runtime and memory traffic rather than optimizing occupancy alone. See [CUDA occupancy guidance](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#occupancy).

A launch with 1,024 blocks of 256 threads creates 262,144 logical threads. Blocks run as resources become available; that launch does not require all 262,144 threads to be resident simultaneously.

### Common CUDA Concepts

| Concept | Description | Typical Syntax / Example | Notes |
|---|---|---|---|
| **Kernel** | GPU function executed by many threads. | `__global__ void kernel(...)` | Launched from host with `<<<grid, block>>>`. |
| **Thread** | Smallest programming unit. | `threadIdx.x` | Usually handles one or more data elements. |
| **Block** | Group of cooperating threads. | `blockDim.x` | Threads in a block share memory and can synchronize. |
| **Grid** | All blocks launched for one kernel. | `gridDim.x` | Can be 1D, 2D, or 3D. |
| **Warp / Wavefront** | Hardware execution group. | 32 threads on many NVIDIA GPUs | Important for divergence and coalescing. |
| **Shared Memory** | Fast per-block memory. | `__shared__ float tile[256];` | Useful for tiling and reductions. |
| **Global Memory** | Main device memory visible to kernels. | `cudaMalloc`, pointer access | Large but relatively high latency. Coalescing matters. |
| **Register** | Fast per-thread storage. | Local scalar variables | Excessive register use can reduce occupancy. |
| **Barrier** | Synchronizes threads in one block. | `__syncthreads()` | Does not synchronize across blocks. |
| **Device Function** | GPU function called from GPU code. | `__device__ float f(float x)` | Cannot be called directly from host. |
| **Host Function** | CPU function. | normal C/C++ function | Orchestrates allocation, copies, launches. |
| **Stream** | Ordered queue of GPU work. | `cudaStream_t` | Enables overlap of copies and kernels. |
| **Event** | Timing or synchronization marker. | `cudaEvent_t` | Useful for measuring GPU time. |

### Performance Checklist

Use this checklist when improving a GPU kernel:

| Question | Why It Matters |
|---|---|
| Are there enough threads? | GPUs need many active warps to hide latency. |
| Are memory accesses coalesced? | Coalescing reduces global-memory transactions. |
| Is branch divergence low? | Divergent branches serialize work within a warp. |
| Is shared memory used safely? | Bank conflicts and missing barriers cause slowdowns or bugs. |
| Are host-device transfers minimized? | On discrete GPUs, transfers can dominate runtime for small workloads. |
| Is occupancy reasonable? | Too many registers or too much shared memory can limit resident warps. |
| Is arithmetic intensity high? | More computation per byte of memory helps performance. |
| Are errors checked? | CUDA errors can otherwise go unnoticed. |

### Common Mistakes

| Mistake | Why It Is a Problem | Fix |
|---|---|---|
| No bounds check in the kernel | Extra threads may access invalid memory. | Use `if (i < n)`. |
| Copying data back after every kernel | On discrete GPUs, host-device transfers can be expensive. | Keep data on the GPU across multiple kernels when possible. |
| Assuming all GPU threads run independently | Warps execute together. | Consider divergence and memory coalescing. |
| Using too many registers per thread | Reduces occupancy. | Inspect compiler reports and tune kernel resource use. |
| Synchronizing across blocks inside one kernel | Standard CUDA has no simple global barrier inside a normal kernel. | Split work into multiple kernel launches or use cooperative groups where supported. |
| Ignoring CUDA return codes | Errors may remain hidden. | Check every CUDA API call and synchronize when debugging. |
| Confusing shared memory with global memory | They have different scope, size, and latency. | Use shared memory only for per-block cooperation. |
