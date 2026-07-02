## GPU Programming

GPUs (Graphics Processing Units) are throughput-oriented processors designed to run the same kind of operation across many data elements at once. They were originally built for graphics workloads, where millions of pixels or vertices must be processed in parallel, but the same architecture is also effective for scientific simulation, image processing, machine learning, numerical computing, and other data-parallel workloads.

**What makes GPU programming different?**

- GPUs contain many execution lanes organized for massive parallelism.
- They rely on specialized memory systems, such as global memory, shared memory, registers, caches, constant memory, texture memory, and memory banks.
- Programming models such as CUDA, HIP, OpenCL, and SYCL let developers launch thousands or millions of lightweight threads.
- High performance depends on memory access patterns, thread organization, occupancy, synchronization, and avoiding excessive branch divergence.

> A GPU is fast when many threads perform similar work on different data with predictable memory access.

### Time-Space Duality

In parallel architecture, **time-space duality** compares two ways to achieve high throughput:

1. Use many replicated processing elements at the same time (**spatial parallelism**).
2. Reuse fewer processing elements over time through a pipeline (**temporal parallelism**).

Modern GPUs combine both ideas: they have many replicated execution units, and each unit is deeply pipelined.

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

**Analogy:** Imagine a bakery trying to make thousands of identical cookies before sunrise. One strategy is to hire many bakers and give each baker their own station: at the same moment, one loads dough, another adds sugar, another mixes, and another places cookies in the oven. This is like an **array processor**, where many processing elements work side by side in space. Another strategy is to use one very efficient cookie-making machine with stations arranged in a line: dough enters the first station, then moves to mixing, then shaping, then baking, while the next cookie enters right behind it. This is like a **vector pipeline**, where fewer processing elements are reused over time. A modern GPU is like a huge bakery that uses both strategies at once: it has many stations working in parallel, and each station is also pipelined so new work keeps flowing through without waiting for the previous item to finish.

#### I. Array Processors: Spatial Parallelism

An array processor has multiple processing elements. Each one works on a different data element, often under a common instruction stream.

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

A **vector processor** executes operations on vectors, which are one-dimensional arrays of elements. Instead of writing a loop that performs one scalar operation at a time, a vector instruction can express the same operation across many elements.

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

Stride matters because contiguous memory access is usually much faster than scattered memory access. On GPUs, adjacent threads should ideally access adjacent memory addresses so the hardware can combine memory requests into fewer transactions.

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

**GPU performance note:** A direct column access in row-major memory can be inefficient because consecutive threads may read nonconsecutive addresses. Optimized matrix multiplication usually uses tiling: each block loads a tile of $A$ and $B$ into shared memory, then reuses those values many times.

### Loading and Storing Vectors from Memory

Vector loads and stores move multiple elements between memory and registers. On GPUs, the closest practical idea is **coalesced memory access**, where threads in the same warp access consecutive memory addresses so the hardware can combine the requests.

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

Efficient GPU programs usually organize data so that adjacent threads access adjacent elements. This is why arrays of structures are sometimes converted into structures of arrays.

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

**Memory banking** divides memory into multiple independent banks so several accesses can proceed in parallel. This idea appears in vector machines, GPU shared memory, register files, caches, and memory controllers.

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

If many threads access different addresses in the same bank, a **bank conflict** may occur. Bank conflicts can serialize memory access and reduce throughput.

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

### Vectorizable Loops

A loop is **vectorizable** when iterations are independent and can safely run in parallel.

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

This is vectorizable because each output element is independent. For integer arrays, a shift such as `(A[i] + B[i]) >> 1` can be faster than division by 2, but it is not always equivalent:

- It may behave differently for negative integers.
- It can overflow before the shift if `A[i] + B[i]` exceeds the integer range.
- It is not appropriate for floating-point averages.

A safer integer average may require a wider type or a formula that avoids overflow.

### GPUs Are SIMT Engines

GPUs are often described as SIMD-like, but the more accurate programming model is **SIMT: Single Instruction, Multiple Threads**.

In SIMT:

- The programmer writes code as if each thread is scalar and independent.
- The hardware groups threads into fixed execution groups: **warps** on NVIDIA GPUs and **wavefronts/waves** on AMD GPUs.
- Threads in the same group usually execute the same instruction together.
- If threads diverge into different branches, the hardware uses masks and may serialize the branch paths.

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

Each thread appears to have its own scalar execution path, but the hardware executes groups of threads together when possible.

> **Correction:** Warps are generally fixed groups of neighboring threads, not arbitrary groups dynamically re-formed every cycle. Divergence is handled with active-lane masks and reconvergence mechanisms, not by freely regrouping all matching threads across the GPU.

### Fine-Grained Multithreading

**Fine-grained multithreading** switches among many hardware thread contexts to hide latency. GPUs use this idea heavily.

When one warp stalls on a long-latency memory access, the scheduler can issue instructions from another ready warp.

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

A modern GPU is organized around many compute units, often called **Streaming Multiprocessors (SMs)** on NVIDIA GPUs or **Compute Units (CUs)** on AMD GPUs.

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

**General-purpose GPU programming (GPGPU)** uses GPUs for non-graphics computation. It works best when the workload has large amounts of data parallelism.

Good GPU workloads usually have:

- Many independent elements.
- High arithmetic intensity or coalesced memory access.
- Limited synchronization.
- Regular control flow.
- Enough work to amortize host-device transfer overhead.

Poor GPU workloads often have:

- Heavy branching.
- Small input sizes.
- Irregular pointer chasing.
- Strong sequential dependencies.
- Frequent CPU-GPU transfers.

#### Typical GPU Execution Flow

```text
+-------------------------+       1. Copy input       +------------------------+
|      CPU / Host         | -------------------------> |      GPU / Device      |
|                         |                           |                        |
| Runs host code          |       3. Copy output      | Executes kernels       |
| Allocates device memory | <------------------------- | Stores device data     |
+-------------------------+                           +-----------+------------+
                                                               ^
                                                               |
                                                       2. Kernel launch
```

I. **Copy input data to the GPU**

- Allocate device memory.
- Transfer input arrays from host memory to device memory.

II. **Launch a GPU kernel**

- The CPU launches a GPU function called a kernel.
- The kernel runs across many blocks and threads.

III. **Copy results back to the CPU**

- Transfer output data from device memory back to host memory.
- Free device memory when finished.

> **Performance note:** Transfers across PCIe or other host-device links are expensive. Reusing data on the GPU across multiple kernels is often faster than copying it back and forth repeatedly.

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
- Blocks are scheduled independently, so they should not depend on one another during a kernel.

#### Thread

A thread is the basic programming unit. Each thread has:

- Its own registers.
- Its own local variables.
- Built-in indices such as `threadIdx`, `blockIdx`, and `blockDim`.

#### Warp / Wavefront

A warp is the hardware execution group. On NVIDIA GPUs, a warp is typically 32 threads. On AMD GPUs, the corresponding group is often called a wavefront or wave, and the width depends on the architecture.

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

The CPU variable `d_data` is a handle to memory allocated on the GPU.

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

The grid size is rounded up so every element is covered. The bounds check inside the kernel prevents out-of-range memory access.

#### Copy Results Back and Free Memory

```cpp
cudaMemcpy(h_data, d_data, bytes, cudaMemcpyDeviceToHost);
cudaFree(d_data);
```

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

This version fixes two common issues:

- It checks `i < n` before reading input.
- It uses dynamically sized shared memory so the buffer matches the block size.

### Common CUDA Concepts

| Concept | Description | Typical Syntax / Example | Notes |
|---|---|---|---|
| **Kernel** | GPU function executed by many threads. | `__global__ void kernel(...)` | Launched from host with `<<<grid, block>>>`. |
| **Thread** | Smallest programming unit. | `threadIdx.x` | Usually handles one or more data elements. |
| **Block** | Group of cooperating threads. | `blockDim.x` | Threads in a block share memory and can synchronize. |
| **Grid** | All blocks launched for one kernel. | `gridDim.x` | Can be 1D, 2D, or 3D. |
| **Warp / Wavefront** | Hardware execution group. | 32 threads on many NVIDIA GPUs | Important for divergence and coalescing. |
| **Shared Memory** | Fast per-block memory. | `__shared__ float tile[256];` | Useful for tiling and reductions. |
| **Global Memory** | Main GPU memory. | `cudaMalloc`, pointer access | Large but high latency. Coalescing matters. |
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
| Are host-device transfers minimized? | Transfers can dominate runtime for small workloads. |
| Is occupancy reasonable? | Too many registers or too much shared memory can limit resident warps. |
| Is arithmetic intensity high? | More computation per byte of memory helps performance. |
| Are errors checked? | CUDA errors can otherwise go unnoticed. |

### Common Mistakes

| Mistake | Why It Is a Problem | Fix |
|---|---|---|
| No bounds check in the kernel | Extra threads may access invalid memory. | Use `if (i < n)`. |
| Copying data back after every kernel | Host-device transfers are expensive. | Keep data on the GPU across multiple kernels. |
| Assuming all GPU threads run independently | Warps execute together. | Consider divergence and memory coalescing. |
| Using too many registers per thread | Reduces occupancy. | Inspect compiler reports and tune kernel resource use. |
| Synchronizing across blocks inside one kernel | Standard CUDA has no simple global barrier inside a normal kernel. | Split work into multiple kernel launches or use cooperative groups where supported. |
| Ignoring CUDA return codes | Errors may remain hidden. | Check every CUDA API call and synchronize when debugging. |
| Confusing shared memory with global memory | They have different scope, size, and latency. | Use shared memory only for per-block cooperation. |
