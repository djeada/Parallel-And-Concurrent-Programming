# GPU Programming Quiz

Notes: [GPU Programming](../notes/09_gpu_programming.md)

Check your understanding of SIMT execution, CUDA thread indexing, warps and divergence, the GPU memory hierarchy, synchronisation, host-device transfers, streams and atomics.

#### Q. One design has 64 processing elements that each apply the same instruction to a different element in the same cycle. Another feeds elements one per cycle through a single deeply pipelined unit. Which pairing is correct?

* [ ] The first is temporal parallelism; the second is spatial parallelism
* [x] The first is spatial parallelism; the second is temporal parallelism
* [ ] Both are temporal parallelism, because both repeat one instruction
* [ ] Both are spatial parallelism, because both process many elements

> **Explanation:** Replicating processing elements that work side by side is spatial parallelism (an array processor). Reusing pipelined hardware over successive cycles is temporal parallelism (a vector pipeline). GPUs combine both.

#### Q. A `float` matrix with 4 rows and 6 columns is stored in row-major order. A loop walks down column 2. How many elements apart in memory are consecutive accesses?

* [ ] 1
* [ ] 4
* [x] 6
* [ ] 24

> **Explanation:** In row-major order the element below `A(i,j)` is one full row later, so the stride equals the number of columns, here 6. Walking along a row has stride 1.

#### Q. A matrix with 5 rows and 7 columns is stored row-major in a flat array starting at index 0. At which index is `A(2,3)`?

* [x] 17
* [ ] 13
* [ ] 23
* [ ] 35

> **Explanation:** The flat index is `row * cols + col`, so $2 \cdot 7 + 3 = 17$. Multiplying by the row count instead gives 13, and swapping row and column gives 23.

#### Q. A kernel stores particles as an array of `struct { float x, y, z; }` and thread `i` reads only `particles[i].x`. Why might converting to separate `x[]`, `y[]` and `z[]` arrays speed it up?

* [x] Neighbouring threads then read neighbouring `x` values, so fewer memory transactions are needed
* [ ] Separate arrays are stored in shared memory automatically instead of global memory
* [ ] The compiler can only vectorise arrays of primitive types, never structures
* [ ] Structures are padded to 64 bytes on GPUs, so each read fetches a whole cache line

> **Explanation:** With the array of structures, consecutive threads read addresses 12 bytes apart and fetch `y` and `z` they do not use. A structure of arrays puts the `x` values contiguously, so a warp's loads coalesce.

#### Q. Which loop cannot be mapped directly to one GPU thread per iteration without changing its result?

* [ ] `for (i = 0; i < n; ++i) C[i] = A[i] + B[i];`
* [ ] `for (i = 0; i < n; ++i) C[i] = 2 * A[i];`
* [x] `for (i = 1; i < n; ++i) C[i] = C[i - 1] + A[i];`
* [ ] `for (i = 0; i < n; ++i) C[i] = (A[i] + B[i]) / 2;`

> **Explanation:** Each `C[i]` depends on `C[i - 1]` from the previous iteration, a loop-carried dependency. The other loops write each output from independent inputs, so their iterations can run in any order.

#### Q. Compiled with GCC on x86-64, what does this C program print?

```c
#include <stdio.h>
int main(void) {
    int a = -1, b = -2;
    printf("%d %d\n", (a + b) / 2, (a + b) >> 1);
    return 0;
}
```

* [ ] `-1 -1`
* [ ] `-2 -2`
* [ ] `-2 -1`
* [x] `-1 -2`

> **Explanation:** Integer division truncates towards zero, so $-3 / 2 = -1$. GCC implements `>>` on a negative `int` as an arithmetic shift, which rounds towards negative infinity, giving `-2`. Shifting is not a drop-in replacement for halving signed values.

#### Q. What does SIMT (Single Instruction, Multiple Threads) mean for the programmer?

* [ ] They write explicit vector instructions whose width matches the hardware lanes
* [x] They write scalar per-thread code, and the hardware runs threads in groups (warps)
* [ ] Every thread runs on its own independent core with its own instruction fetch unit
* [ ] Each thread executes a different kernel, chosen by its thread index

> **Explanation:** In SIMT each thread has its own logical control flow and indices, while the hardware issues instructions for a warp or wavefront across its active lanes. Explicit vector width is the SIMD style, such as AVX.

#### Q. A CUDA block is one-dimensional with 256 threads. On an NVIDIA GPU, which warp and lane does the thread with `threadIdx.x == 70` occupy?

* [ ] Warp 1, lane 38
* [ ] Warp 2, lane 70
* [ ] Warp 3, lane 6
* [x] Warp 2, lane 6

> **Explanation:** A block is split into warps of 32 threads with consecutive thread IDs, starting with thread 0 in warp 0. So $70 / 32 = 2$ (integer division) and $70 \bmod 32 = 6$.

#### Q. A CUDA block has `blockDim = (16, 16, 1)`. Which warp contains the thread with `threadIdx.x == 5` and `threadIdx.y == 3`?

* [x] Warp 1
* [ ] Warp 0
* [ ] Warp 3
* [ ] Warp 5

> **Explanation:** CUDA linearises the thread ID as `x + y * blockDim.x`, so this thread is $5 + 3 \cdot 16 = 53$. Warps hold IDs 0–31, 32–63 and so on, so ID 53 is in warp 1.

#### Q. A kernel is launched with 100 threads per block on an NVIDIA GPU. How many warps does each block occupy?

* [ ] 3, and the remaining 4 threads are dropped
* [ ] 3, and the remaining 4 threads join the next block's warp
* [x] 4, with only 4 active threads in the last warp
* [ ] 100, one warp per thread

> **Explanation:** Warps are allocated in units of 32 threads, so 100 threads need $\lceil 100 / 32 \rceil = 4$ warps. The last warp has 28 inactive lanes, which is why block sizes are usually multiples of 32.

#### Q. A kernel uses 256-thread blocks. Which branch condition causes no divergence within any warp?

* [ ] `if (threadIdx.x % 2 == 0)`
* [ ] `if (threadIdx.x < 48)`
* [ ] `if (threadIdx.x % 32 < 16)`
* [x] `if ((threadIdx.x / 32) % 2 == 0)`

> **Explanation:** `threadIdx.x / 32` is the same for every thread in a warp, so each warp takes one path. The other conditions split warps: odd and even lanes, warp 1 at thread 48, or lower and upper half-lanes.

#### Q. Threads in a warp take both sides of an `if`/`else`. What does the hardware typically do?

* [ ] It moves the threads that took the `else` path into a different warp for that branch
* [x] It runs each path in turn with the lanes not on that path masked off, then reconverges
* [ ] It runs only the path taken by the majority of lanes and discards the others
* [ ] It aborts the kernel with a divergence error

> **Explanation:** Warps are fixed groups of threads and are not regrouped on divergence. The paths are serialised with an active mask, so the warp spends time on both paths and lane utilisation drops.

#### Q. A warp issues a load from global memory that takes hundreds of cycles. What does the SM scheduler usually do meanwhile?

* [ ] It saves the warp's registers to memory and loads another warp, like an OS context switch
* [ ] It idles the SM until the load returns, because warps run in a fixed order
* [ ] It moves the stalled warp to a different SM that has the data cached
* [x] It issues instructions from other resident warps that are ready to run

> **Explanation:** GPUs use fine-grained multithreading: many warps stay resident with their registers on chip, so switching between them costs almost nothing. Keeping enough ready warps is how GPUs hide memory latency.

#### Q. An SM has 65,536 registers and supports at most 64 resident warps. A kernel uses 64 registers per thread and 256-thread blocks. Ignoring allocation granularity and other limits, what occupancy does the register limit allow?

* [x] 50%
* [ ] 25%
* [ ] 75%
* [ ] 100%

> **Explanation:** One block needs $64 \cdot 256 = 16{,}384$ registers, so 4 blocks fit. That is 1,024 threads, or 32 warps, out of a maximum of 64 warps.

#### Q. You cut a kernel's register use so occupancy rises from 25% to 50%, but the kernel gets slower. What is a plausible reason?

* [ ] Higher occupancy always increases branch divergence
* [ ] Doubling occupancy halves the number of threads in each warp
* [x] Values now spill to local memory or are recomputed, adding slow memory traffic
* [ ] The GPU throttles its clock whenever occupancy exceeds a fixed threshold

> **Explanation:** Occupancy only measures resident warps relative to the maximum. If fewer registers force spills to local memory, which lives in device memory, or reduce data reuse, the extra traffic can outweigh the latency hiding.

#### Q. A 1D kernel computes `int i = blockIdx.x * blockDim.x + threadIdx.x;`. What is `i` for the thread with `blockIdx.x == 3`, `blockDim.x == 128` and `threadIdx.x == 5`?

* [ ] 136
* [x] 389
* [ ] 512
* [ ] 645

> **Explanation:** $3 \cdot 128 + 5 = 389$. Blocks 0, 1 and 2 cover indices 0–383, so block 3 starts at 384. The value 136 adds instead of multiplying the block terms.

#### Q. With `n = 1000` and `block = 256`, the host computes `grid = (n + block - 1) / block`. How many launched threads fail the kernel's `if (i < n)` check?

* [x] 24
* [ ] 0
* [ ] 232
* [ ] 256

> **Explanation:** The grid is $(1000 + 255) / 256 = 4$ blocks, which is 1,024 threads. Threads 1,000–1,023 are past the end, so without the bounds check those 24 threads would access invalid memory.

#### Q. A host program uses `grid = n / block;` with `n = 1000` and `block = 256`. What happens?

* [ ] The launch fails because `n` is not a multiple of `block`
* [ ] The runtime rounds the grid up, so every element is processed
* [x] Only 3 blocks run, so the last 232 elements are never processed
* [ ] The last block runs with 232 threads instead of 256

> **Explanation:** Integer division gives 3 blocks covering indices 0–767. The launch succeeds and leaves elements 768–999 untouched. Rounding up with `(n + block - 1) / block` plus a bounds check fixes it.

#### Q. A 2D kernel uses `col = blockIdx.x * blockDim.x + threadIdx.x`, `row = blockIdx.y * blockDim.y + threadIdx.y` and `idx = row * width + col`. With `blockDim = (16, 16)`, `blockIdx = (2, 1)`, `threadIdx = (3, 4)` and `width = 100`, what is `idx`?

* [ ] 435
* [ ] 3520
* [x] 2035
* [ ] 1635

> **Explanation:** `col` is $2 \cdot 16 + 3 = 35$ and `row` is $1 \cdot 16 + 4 = 20$, so `idx` is $20 \cdot 100 + 35 = 2035$. The value 3520 swaps the roles of row and column.

#### Q. A kernel processes a row-major matrix with `col` derived from `threadIdx.x` and `row` from `threadIdx.y`. Which access pattern lets a warp's global loads coalesce best?

* [ ] `M[col * width + row]`
* [x] `M[row * width + col]`
* [ ] `M[(row + col) * width]`
* [ ] `M[col * width * width + row]`

> **Explanation:** Neighbouring threads in a warp differ in `threadIdx.x`, hence in `col`. With `row * width + col` they read consecutive addresses; with `col * width + row` they read addresses `width` elements apart.

#### Q. A kernel uses a grid-stride loop and is launched with 2 blocks of 4 threads and `n = 20`. Which elements does the thread with global index 5 process?

```cpp
for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n;
     i += blockDim.x * gridDim.x) {
    out[i] = 2 * in[i];
}
```

* [ ] 5 only
* [ ] 5, 9, 13 and 17
* [ ] 5, 10 and 15
* [x] 5 and 13

> **Explanation:** The stride is the total thread count, $4 \cdot 2 = 8$, so the thread handles 5 and 13; the next value, 21, fails `i < n`. A grid-stride loop lets a fixed-size grid cover any `n`.

#### Q. A host program launches `kernel<<<1, 2048>>>(...)` on a current NVIDIA GPU. What happens?

* [ ] The runtime splits the block into two blocks of 1,024 threads
* [x] The launch fails with a configuration error and the kernel does not run
* [ ] The kernel runs with 2,048 threads but only 1,024 can use `__syncthreads()`
* [ ] The kernel runs, with each hardware thread executing two logical threads

> **Explanation:** On current GPUs a thread block may contain at most 1,024 threads. An invalid launch configuration is reported as an error, for example by `cudaGetLastError()`, and nothing is executed.

#### Q. A program times a kernel like this. Why is the printed time misleading?

```cpp
auto t0 = std::chrono::steady_clock::now();
myKernel<<<grid, block>>>(d_data, n);
auto t1 = std::chrono::steady_clock::now();
```

* [ ] `steady_clock` cannot be called from a file compiled by `nvcc`
* [ ] The first launch always includes copying the input to the GPU
* [x] Kernel launches are asynchronous, so it measures only the launch, not the execution
* [ ] The kernel runs twice, once for each `now()` call

> **Explanation:** The launch returns to the host before the kernel finishes. Synchronise before reading `t1`, or record `cudaEvent_t` start and stop events in the stream and use `cudaEventElapsedTime()`.

#### Q. In the legacy default stream, the host launches a kernel and then calls `cudaMemcpy(h, d, bytes, cudaMemcpyDeviceToHost)`. Does it need `cudaDeviceSynchronize()` between them to read correct results?

* [x] No, the copy follows the kernel in the same stream and blocks until done
* [ ] Yes, otherwise the copy may start while the kernel is still writing `d`
* [ ] Yes, because `cudaMemcpy` returns before the copy has even started
* [ ] No, because a kernel finishes before its launch call returns

> **Explanation:** Work in one stream executes in issue order, and `cudaMemcpy` is synchronous with respect to the host. An explicit synchronisation is still useful for catching kernel errors close to their source.

#### Q. A kernel launch is followed by `cudaGetLastError()`, which returns `cudaSuccess`. Why might a later `cudaDeviceSynchronize()` still return an error?

* [ ] `cudaGetLastError()` only reports errors from memory allocation calls
* [x] It reports launch errors, but faults during execution surface only once the kernel has run
* [ ] `cudaDeviceSynchronize()` reports an error whenever more than one kernel was launched
* [ ] `cudaGetLastError()` always returns `cudaSuccess` the first time it is called

> **Explanation:** Because launches are asynchronous, an invalid memory access inside the kernel happens after the launch call returns. Checking the return value of a later synchronising call catches these execution errors.

#### Q. A kernel declares `__shared__ float tile[256];`. Who can read and write `tile`?

* [ ] Every thread in the grid, through one copy per SM
* [ ] The host, after the kernel completes, via `cudaMemcpy`
* [ ] Only the single thread that declared it
* [x] All threads of the same block, each block having its own copy

> **Explanation:** Shared memory is allocated per block and lives for the block's lifetime. Threads in other blocks see their own copy, and the host cannot access it directly.

#### Q. A kernel's local array is too large for registers and is placed in "local memory". Where does local memory physically reside?

* [x] In device memory, like global memory, cached in L1/L2
* [ ] In the SM's register file
* [ ] In the host's RAM, reached over PCIe
* [ ] In the constant cache, since it is private to one thread

> **Explanation:** "Local" describes per-thread scope, not location. Local memory and register spills live in device memory, so heavy use adds global-memory-like latency and traffic.

#### Q. Which data suits CUDA `__constant__` memory best?

* [ ] A 200 MB lookup table that every thread updates in place
* [ ] Per-block scratch space that threads write and then read back
* [ ] An output array in which each thread writes its own result
* [x] Small read-only coefficients that a warp's threads all read at one index

> **Explanation:** Constant memory is small (typically 64 KB), read-only to kernels and cached. It works best when a warp's threads read the same address; scattered addresses within a warp are serialised.

#### Q. How are the L1 and L2 caches organised on a typical NVIDIA GPU?

* [x] Each SM has its own L1, and one L2 is shared by all SMs
* [ ] Each warp has its own L1, and each block has its own L2
* [ ] There is one L1 for the GPU, and each SM has a private L2
* [ ] L1 and L2 are both in host memory and cache PCIe transfers

> **Explanation:** L1 sits inside each SM (on many architectures sharing storage with shared memory), while L2 sits in front of the memory controllers and serves all SMs.

#### Q. Shared memory has 32 banks, and successive 32-bit words map to successive banks. Each thread of a warp reads `s[threadIdx.x * 32]` from a `float` array. What happens?

* [ ] No conflict, because each thread reads a different address
* [ ] A 2-way bank conflict
* [x] A 32-way bank conflict, because every address maps to bank 0
* [ ] The hardware broadcasts one value to all 32 threads

> **Explanation:** Word index `32 * t` modulo 32 is always 0, so all 32 different addresses fall in one bank and are serialised. Broadcast applies only when threads read the same word.

#### Q. Under the same 32-bank layout, every thread of a warp reads `s[0]`. Is this a bank conflict?

* [x] No, the word is broadcast to all requesting threads
* [ ] Yes, a 32-way conflict, since all threads use bank 0
* [ ] Yes, a 2-way conflict between the two half-warps
* [ ] It depends on whether `s` was declared `volatile`

> **Explanation:** Conflicts arise when threads access different words in the same bank. When they read the same 32-bit word, the hardware broadcasts it, so the read is served in one pass.

#### Q. A transpose kernel declares `__shared__ float tile[32][33];` instead of `tile[32][32]`. Why the extra column?

* [ ] It stores a per-row checksum
* [ ] It aligns each row to a 128-byte cache line
* [ ] It leaves room for the `__syncthreads()` barrier counter
* [x] It shifts each row by one bank, so reading a column hits 32 different banks

> **Explanation:** With 32 columns, `tile[t][c]` for different `t` all map to the same bank, giving a 32-way conflict on column reads. With 33 columns, row `t` starts one bank later, so the column spreads over all banks.

#### Q. In a 256-thread block, a kernel calls `__syncthreads()` inside `if (threadIdx.x < 128) { ... }`. What does CUDA guarantee?

* [ ] The barrier synchronises only the 128 threads that enter the branch
* [ ] The compiler moves the barrier outside the branch automatically
* [x] No defined behaviour: the program may hang or give wrong results
* [ ] The other 128 threads wait at the end of the kernel instead

> **Explanation:** `__syncthreads()` is allowed in conditional code only if the condition evaluates identically across the whole block. Otherwise behaviour is undefined, so keep the barrier outside the divergent branch.

#### Q. In the note's `blockSum` kernel, why is there a `__syncthreads()` between `buf[t] = ...` and the reduction loop?

* [ ] It flushes `buf` to global memory so the host can read it
* [x] Threads then read `buf[t + stride]`, which another thread wrote
* [ ] It makes the 8 warps of the block execute in order
* [ ] It is needed only when `n` is not a multiple of the block size

> **Explanation:** Without the barrier, a thread could read a neighbour's slot before that thread has stored its input. The barrier makes every store in the block visible before any thread starts reading.

#### Q. The `blockSum` reduction halves `stride` from `blockDim.x / 2` down to 1. What goes wrong if it is launched with `blockDim.x == 192`?

* [ ] The loop never ends, because 192 is not a power of two
* [ ] The kernel deadlocks at the first `__syncthreads()`
* [ ] Thread 0 reads past the end of the shared-memory buffer
* [x] Some values are never added in, so the partial sum is wrong

> **Explanation:** The strides become 96, 48, 24, 12, 6, 3, 1. At stride 3, `buf[2]` gets `buf[5]`, but at stride 1 only `buf[1]` is added to `buf[0]`, so `buf[2]` is lost. The halving scheme assumes a power-of-two block size.

#### Q. After `blockSum` writes one partial sum per block, you need the total. Why not have block 0 wait in the same kernel until all other blocks finish?

* [x] Blocks are not guaranteed to run concurrently or in any order, so waiting can deadlock
* [ ] `__syncthreads()` already waits for all blocks, so the wait is redundant
* [ ] Block 0 cannot read global memory written by other blocks
* [ ] Partial sums are stored in shared memory and vanish when the kernel ends

> **Explanation:** If block 0 spins while occupying an SM, a block it waits for may never be scheduled. Common fixes are a second kernel in the same stream, atomics, or a grid-wide sync with cooperative groups where supported.

#### Q. Many threads run `bins[v] += 1;` on a global histogram, and the counts come out too low. What is the standard fix?

* [x] Use `atomicAdd(&bins[v], 1)`
* [ ] Put `__syncthreads()` before the increment
* [ ] Declare `bins` as `__constant__`
* [ ] Launch the kernel with one thread per bin

> **Explanation:** The increment is a read-modify-write, so concurrent updates to the same bin overwrite each other. `atomicAdd` makes each update indivisible; `__syncthreads()` only orders threads of one block and does not prevent the race.

#### Q. Each thread runs `int slot = atomicAdd(&count, 1); out[slot] = value;` with `count` starting at 0. What does `slot` hold?

* [ ] The new value of `count`, so the first thread gets 1
* [ ] Always 0, because the atomic result is only visible after the kernel ends
* [ ] An unspecified value unless the threads are in the same warp
* [x] The old value of `count`, so each thread gets a distinct index

> **Explanation:** `atomicAdd` returns the value stored before the addition. Each thread receives a unique slot from 0 upwards, though the order in which threads get them is unspecified.

#### Q. Every thread in a large grid does `atomicAdd(&total, x[i])` on one global variable, and the kernel is slow. What is a common improvement?

* [x] Reduce within each block in shared memory first, then do one atomic per block
* [ ] Replace the atomic with a plain `total += x[i]`, which is equivalent on GPUs
* [ ] Move `total` into `__constant__` memory so reads are cached
* [ ] Add `__syncthreads()` after every atomic to spread them out

> **Explanation:** Atomics to one address are serialised, so millions of them form a bottleneck. A block-level reduction cuts the global atomics to one per block.

#### Q. Why is host memory from `cudaMallocHost()` (pinned memory) used for `cudaMemcpyAsync()`?

* [ ] It is allocated in GPU memory, so no transfer is needed at all
* [x] The GPU can DMA from page-locked pages, which truly async copies require
* [ ] Pageable memory cannot be copied to the GPU at all
* [ ] Pinned memory is the only kind that `cudaFree()` can release

> **Explanation:** The GPU's copy engine needs host pages that cannot be moved or swapped. Pinned memory is necessary for truly asynchronous transfers and often copies faster, but pinning too much can hurt overall system performance.

#### Q. A program splits its data into chunks and issues copy-in, kernel and copy-out for each chunk in its own non-default stream, using pinned host buffers. What can this achieve?

* [ ] It guarantees every chunk finishes in the order its stream was created
* [ ] It removes the need to copy data between host and device
* [ ] It lets operations within one stream run out of order
* [x] Copies for one chunk can overlap with kernels for another chunk

> **Explanation:** Operations in one stream run in order, but different streams may overlap, so transfer time can hide behind computation. How much overlap occurs depends on the GPU's copy engines and resources.

#### Q. A program issues work to streams `s1` and `s2`, both created with default flags, and also launches a kernel in the legacy default stream. What is the effect of that default-stream kernel?

* [ ] It runs concurrently with everything, since the default stream has highest priority
* [x] It waits for earlier work in `s1` and `s2`, and later work in them waits for it
* [ ] It is rejected, because a program cannot mix default and non-default streams
* [ ] It affects only work issued from other host processes

> **Explanation:** The legacy default stream synchronises implicitly with blocking streams in the same context, which can serialise an overlapped pipeline. Non-blocking streams or the per-thread default stream avoid this.

#### Q. Which call is not allowed by CUDA's function execution-space qualifiers?

* [ ] A `__global__` kernel launched from host code with `<<<grid, block>>>`
* [x] Host code calling a `__device__` function directly
* [ ] A `__global__` kernel calling a `__device__` helper
* [ ] Host code calling a `__host__ __device__` function

> **Explanation:** A `__device__` function runs on the GPU and can be called only from device code. Marking a function `__host__ __device__` compiles it for both sides, so host and kernels can both call it.

#### Q. An OpenCL kernel is being ported to CUDA. Which pair of terms corresponds most closely?

* [ ] OpenCL work-group and CUDA warp
* [ ] OpenCL `__global` memory and CUDA `__shared__` memory
* [x] OpenCL `__local` memory and CUDA `__shared__` memory
* [ ] OpenCL work-item and CUDA block

> **Explanation:** OpenCL `__local` memory is shared by the work-items of one work-group, just as CUDA `__shared__` memory is shared by the threads of one block. A work-item maps to a thread and a work-group to a block.

#### Q. Why should code meant to run on both NVIDIA and AMD GPUs avoid hard-coding a group width of 32?

* [ ] NVIDIA warps grow to 64 threads when a block exceeds 512 threads
* [ ] The width is chosen at random by the driver at each launch
* [ ] Group width affects only graphics shaders, not compute kernels
* [x] AMD wavefronts are 64 or 32 lanes, depending on architecture and mode

> **Explanation:** NVIDIA warps have 32 threads, but AMD wavefront width varies (wave64 on GCN and CDNA, wave32 or wave64 on RDNA). Querying the width, for example via `warpSize`, keeps warp-level code portable.
