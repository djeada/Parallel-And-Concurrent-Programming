# Shared headers: include these from an example

The `.hpp` files in this directory provide helpers. They have no `main()` and
are not runnable examples. Select a `.cpp` file in a topic directory when
building or using an IDE's Run action.

| Header | Example that uses it |
|---|---|
| [posix.hpp](posix.hpp) | [basic_process.cpp](../multiprocessing/basic_process.cpp) |
| [bounded_queue.hpp](bounded_queue.hpp) | [producer_consumer.cpp](../multithreading/producer_consumer.cpp) |
| [curl_support.hpp](curl_support.hpp) | [parallel_fetch.cpp](../asynchrony/parallel_fetch.cpp) |
| [mpi_support.hpp](mpi_support.hpp) | [hello_world_mpi.cpp](../mpi/hello_world_mpi.cpp) |

From this `common/` directory, build and run the basic process example with:

```bash
g++ -std=c++20 -Wall -Wextra -pthread ../multiprocessing/basic_process.cpp -o /tmp/basic_process
/tmp/basic_process
```

GCC treats a directly compiled `.hpp` file as a precompiled header. Naming that
output `posix` does not make it an executable: running it can report
`Permission denied`. Changing its executable permission does not turn it into a
program.

In an IDE, choose the example's `.cpp` file as the source and use `-std=c++20`
with `-pthread`. A command containing both `-std=c++17` and `-std=c++20` uses the
last setting; keep the C++20 setting alone to make the configuration clear.
HTTP and MPI examples need their additional toolchains described in the
[C++ build guide](../README.md#build-and-run).
