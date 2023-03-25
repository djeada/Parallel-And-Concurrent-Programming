
## Asynchrony

Asynchronous programming is a technique used to achieve concurrency, where tasks can be executed independently without waiting for other tasks to finish. It allows for nonblocking behavior, in contrast to synchronous execution that waits for one task to complete before starting the next task.

Asynchronous programming is particularly useful for tasks that involve I/O operations, such as fetching data from a database, where waiting for the data retrieval could freeze the user interface.

## Building blocks of asynchronous programming

### Event loop

An event loop is a core component of asynchronous programming. It is responsible for managing the execution of tasks, scheduling them, and handling events related to these tasks. The event loop runs in the main thread and orchestrates the execution of coroutines.

An event loop typically provides the following functionalities:

1. Registering tasks for execution
2. Scheduling tasks based on priority or other criteria
3. Handling I/O events and dispatching them to appropriate handlers
4. Handling timers and timeouts
5. Managing concurrency and parallelism

### Coroutines

Coroutines are special functions that can be paused and resumed during their execution. They allow a function to yield control back to the event loop, enabling other tasks to run concurrently. Coroutines are often defined using the `async` keyword.

To work with coroutines, you also need the `await` keyword, which is used to call other asynchronous functions from within a coroutine. The `await` keyword ensures that the coroutine is paused until the awaited function completes, allowing other tasks to run in the meantime.

Example of a simple coroutine:

```python
async def fetch_data():
    # Simulate data fetching with a sleep
    await asyncio.sleep(3)
    return "Data fetched"
```

### Corutines

A Future is an object that represents the result of a coroutine. It contains state information, such as pending, canceled, or completed. Futures act as placeholders for the result of a computation that may not have completed yet.

A Task is a subclass of Future that wraps a coroutine. When a coroutine is scheduled to run, it is wrapped in a Task object, which manages its execution and stores its result or exception once it completes.

Example of working with Future:

```python
import asyncio

async def main():
    loop = asyncio.get_event_loop()
    future = loop.create_future()

    async def set_result():
        await asyncio.sleep(2)
        future.set_result("Hello, world!")

    loop.create_task(set_result())
    result = await future
    print(result)

asyncio.run(main())
```

## Asynchrony vs Multithreading

Asynchrony and multithreading are distinct concepts but can be used together to achieve parallelism. The key difference is that asynchronous functions switch cooperatively, while threads switch preemptively.

1. Synchronous execution with a single thread:

```
single thread: AAAABBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCC
```

2. Synchronous execution with multiple threads:

```
thread 1: AAAAAAAAAAAA---------------------------------

thread 2: ------------BBBBBB--------------------------

thread 3: ------------------CCCCCCCCCCCCCCCCCCCCCCCCCCC
```

3. Asynchronous execution with a single thread:

```
single thread: AAAABBBBAAAACCCAAABBBCCCCCBBBBBBBBBCCCCC
```

4. Asynchronous execution with multiple threads:

```
thread 1: AAAAAAAAA-----
thread 2: ---BBBBBBBBBBB
thread 3: ----CCCC------
```

### Challenges and Considerations

Asynchronous functions, unlike multithreading, switch cooperatively, so the programmer is responsible for inducing a task switch whenever appropriate. This eliminates the need for locks and other synchronization mechanisms, and the cost of task switches is relatively low. However, dealing with existing synchronous functions can be challenging.

When working with asynchronous programming, consider the following:

1. Mixing synchronous and asynchronous code: If you need to use synchronous functions in your asynchronous code, you may run into issues with blocking behavior. To avoid this, consider using `run_in_executor` to run synchronous functions in a separate thread or refactoring the synchronous functions to be asynchronous.

2. Error handling: Asynchronous code can be more complex when it comes to error handling. Make sure to handle exceptions appropriately in coroutines and be aware of the impact of exceptions on the event loop.

3. Debugging: Debugging asynchronous code can be more challenging than synchronous code due to the cooperative nature of task switching. Make use of logging and built-in debug tools like `asyncio.debug()` to help you understand and troubleshoot your asynchronous code.

4. Scalability: Asynchronous programming can significantly improve the performance and scalability of I/O-bound applications, but it may not provide the same benefits for CPU-bound tasks. In such cases, consider using a combination of multithreading and multiprocessing to achieve the desired performance.

5. Backpressure: When working with asynchronous systems that produce and consume data at different rates, you might need to handle backpressure. This refers to the need to control the rate at which data is generated to prevent overwhelming the system. Techniques like buffering, throttling, and load shedding can be employed to manage backpressure effectively.

6. Testing: Testing asynchronous code can be more challenging due to the nondeterministic nature of task execution. To simplify testing, use dedicated testing libraries and frameworks, such as `pytest-asyncio`, that are designed to work with asynchronous code.

## Examples

### Examples in C++

Asynchronous programming in C++ is conceptually similar to Python but has its own set of libraries and constructs. C++ offers several options for asynchronous programming, including the C++ Standard Library, Boost.Asio, and third-party libraries.

### C++ Standard Library

The C++11 standard introduced support for multithreading and concurrency, including features for asynchronous programming. The most commonly used constructs for asynchronous programming in C++ are `std::async`, `std::future`, and `std::promise`.

#### std::async

`std::async` is a function template that allows you to run a function asynchronously, returning a `std::future` that represents the result of the computation. It can execute tasks concurrently or lazily, depending on the launch policy.

Example of using `std::async`:

```cpp
#include <iostream>
#include <future>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::future<int> result = std::async(std::launch::async, add, 10, 20);
    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}
```

#### std::future and std::promise
 
std::future is a class template that represents the result of a computation that may not have completed yet. A `std::promise` is a class template that is used to set the value (or exception) of a `std::future`. It provides a way to communicate the result of a computation from one thread to another.

Example of using `std::future` and `std::promise`:

```cpp
#include <iostream>
#include <thread>
#include <future>

void add(std::promise<int> &&promise, int a, int b) {
    promise.set_value(a + b);
}

int main() {
    std::promise<int> promise;
    std::future<int> result = promise.get_future();

    std::thread t(add, std::move(promise), 10, 20);
    std::cout << "Result: " << result.get() << std::endl;
    t.join();

    return 0;
}
```

I used the <code>GCC</code> compiler with the following flags to compile the examples:

```
g++ file_name.cpp -std=c++17 -pthread -o executable_name
```

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_read_result.cpp">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_status.cpp">future status</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/async_reuses_threads.cpp">async reuses threads</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/merge_sort.cpp">merge sort</a>

### Examples in Python

Asynchronous programming in Python is made possible through the `asyncio` library. In order to understand and utilize asynchronous programming, it's essential to grasp the following concepts:

1. async: The `async` keyword is used to define a coroutine function. When placed before `def`, it signifies that the function is asynchronous and can be paused and resumed during execution. This allows other tasks to run concurrently, improving the efficiency of I/O-bound operations.

An async function cannot call a non-async function directly, as it would block the event loop. However, you can use `loop.run_in_executor()` to run synchronous functions in a separate thread, or refactor the non-async function to be async.

Example:

```python
async def async_function():
await asyncio.sleep(1)
return "Async result"
```

2. await: The await keyword is used within a coroutine to call another coroutine and pause the current one until the called coroutine finishes. This allows other tasks to run while the current coroutine is waiting for the operation to complete. The await keyword can only be used inside coroutines.

Example:

```python
async def main():
    result = await async_function()
    print(result)
```

3. Task switching: In asynchronous programming, tasks switch cooperatively, meaning that they voluntarily yield control to other tasks by using await. The event loop schedules and manages the execution of these tasks, ensuring that they are run concurrently without blocking each other.

When a coroutine encounters an `await` expression, it yields control back to the event loop, which can then execute other tasks. Once the awaited operation is complete, the event loop resumes the coroutine from where it left off.

4. Running the event loop: The event loop is responsible for executing and managing asynchronous tasks. You can use `asyncio.run()` to run a coroutine as the main entry point of your asynchronous program. This function takes care of creating and running the event loop, executing the coroutine, and closing the loop when it's done.

Example:

```python
asyncio.run(main())
```

5. Mixing async and non-async functions: As mentioned earlier, you cannot directly call a non-async function from within an async function. However, you can use loop.run_in_executor() to run synchronous functions in a separate thread, allowing the event loop to continue running other tasks concurrently.

Example:

```python
import asyncio
from concurrent.futures import ThreadPoolExecutor

def sync_function():
    time.sleep(1)
    return "Sync result"

async def main():
    loop = asyncio.get_event_loop()
    with ThreadPoolExecutor() as executor:
        result = await loop.run_in_executor(executor, sync_function)
        print(result)

asyncio.run(main())
```

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_until_complete.py">loop run until complete</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_forever.py">loop run forever</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_read_result.py">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_create_task.py">future create task</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/data_sharing_queue.py">data sharing queue</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/mutex.py">mutex</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/semaphore.py">semaphore</a>

### Examples in JavaScript


Asynchronous programming in Node.js is a fundamental concept, as it is designed to handle non-blocking I/O operations efficiently. The key components of asynchronous programming in Node.js include:

1. Callbacks: Functions that are passed as arguments to other functions and are called when an asynchronous operation is completed.
2. Promises: Objects that represent the eventual completion (or failure) of an asynchronous operation and its resulting value.
3. Async/await: Modern syntax that makes writing asynchronous code easier and more readable, built on top of Promises.

#### Callbacks

Callbacks are a widely used pattern for handling asynchronous operations in Node.js. They are functions that are passed as arguments to other functions and are called when an operation is completed.

Example:

```javascript
const fs = require('fs');

fs.readFile('example.txt', 'utf8', (err, data) => {
  if (err) {
    console.error(err);
  } else {
    console.log(data);
  }
});
```

#### Promises

Promises are a more modern approach for handling asynchronous operations in Node.js. A Promise represents the eventual result of an asynchronous operation and has three states:

* Pending: Initial state, neither fulfilled nor rejected.
* Fulfilled: The operation completed successfully, and the promise has a resulting value.
* Rejected: The operation failed, and the promise has a reason for the failure.

Example:

javascript

const fs = require('fs').promises;

fs.readFile('example
.txt', 'utf8')
  .then(data => {
    console.log(data);
  })
  .catch(err => {
    console.error(err);
  });

#### Async/await

Async/await is a more modern and readable syntax for writing asynchronous code in Node.js, built on top of Promises. It allows you to write asynchronous code that looks similar to synchronous code, making it easier to understand and maintain.

Example:

```javascript
const fs = require('fs').promises;

async function main() {
  try {
    const data = await fs.readFile('example.txt', 'utf8');
    console.log(data);
  } catch (err) {
    console.error(err);
  }
}

main();
```

* <a href=""></a>
* <a href=""></a>
* <a href=""></a>
* <a href=""></a>

### Summary

| Concept    | C++                                                              | Python                                        | Node.js                                                      |
|------------|------------------------------------------------------------------|-----------------------------------------------|--------------------------------------------------------------|
| Library    | Standard Library (std::future, std::promise, std::async) <br> Boost.Asio                                         | asyncio                                        | Native support (callbacks, Promises)                         |
| Event Loop | Depends on the library (e.g., Boost.Asio provides I/O context)   | asyncio event loop                            | Event loop provided by the libuv library                     |
| Coroutine  | C++20 coroutines (co_await, co_yield)                            | async def (coroutine functions)               | Async functions (async keyword)                              |
| Future     | std::future (contains the result of an asynchronous operation)   | asyncio.Future (contains the result of a coroutine) | Promise (represents the eventual result of an async operation) |
| Await      | co_await (C++20)                                                 | await (used inside async def)                 | await (used inside async functions)                          |
| Async Call | std::async or custom implementation with Boost.Asio             | asyncio.create_task(), asyncio.gather(), etc. | Callbacks, Promises, or async/await syntax                   |
| Task       | std::packaged_task (wraps a callable target)                    | asyncio.Task (wraps a coroutine)              | Promises, async functions, or callbacks                      |

