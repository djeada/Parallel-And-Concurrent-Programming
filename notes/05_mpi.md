
## MPI

MPI is a distributed memory device programming model. Parallelism is achieved by running various processes on different machines. 

### Examples
#### Examples in C++
#### Examples in Python

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/mpi/hello_world_mpi.py">hello world mpi</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/mpi/point_to_point_communication.py">point to point communication</a>

## Rules of thumb

In the preceding sections, I attempted to state the facts as objectively as possible. I'd like to offer some of my personal perspectives on the issue here.

1. Avoid concurrency as much as possible. I think that this is an intriguing issue, however it is difficult to design well and far more difficult to debug. Make sure your programs are both synchronous and sequential.

2. If performance becomes an issue, take the following steps: 
     - If your system's performance is dominated by I/O waits, you should use asynchronous programming.
     - If your system's performance is predicated on CPU-intensive calculation, you should think about employing processes.
     Processes are more isolated than threads and eliminate various kinds of mistakes that might occur in multi-threaded systems.
     - Threads are the ideal solution for obtaining the absolute most CPU speed out of a multi-core computer since they share memory, but this is still dependent on the language implementation, so double-check. 
