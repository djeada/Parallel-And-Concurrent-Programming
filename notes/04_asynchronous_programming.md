
## Asynchrony

Asynchrony is unrelated to the previous two concepts (multithreading and multiprocessing). When one task is completed in an asynchronous approach, you may go on to the next without waiting for the preceding task to finish. This is known as nonblocking behavior. When you execute anything synchronously, you wait for it to finish before proceeding to the next task. Concurrency is accomplished with the help of asynchronous programming. Parallelism may be achieved by asynchronous programming in a multi-threaded environment. 

"Synchronously" means "using the same clock," therefore synchronous instructions share the same clock and must execute one after the other. The term "asynchronous" translates to "not utilizing the same clock," which indicates that the instructions are not concerned with being in synchronization with one another. The phrase does not refer to the connection of the instructions to one another. It is referring to the connection of each instruction to the clock. 

For example, retrieving data from a database may take some time, but we don't want our UI to freeze while we wait. As soon as the asynchronous function is invoked, the asynchronous call accepts a call-back reference and returns execution to the client function. While the asynchronous function performs any necessary processing, your UI may continue to respond to the user; after the asynchronous function has completed its task, the client function may use the resultant data (e.g. to display it in the ui). 

### Building blocks of asynchronous programming

#### Event loop

Although it is not always necessary, an event loop is frequently used in the implementation of asynchrony in programming languages. An event loop is a loop that may be used to register tasks to be executed, execute them, delay or cancel them, and handle other events linked to these tasks. The event loop blocks the main thread and manages execution of coroutines. It executes one function, then stops it and executes another while the first function waits for IO, then restores the paused functions. 

#### Corutines

Coroutines are functions that may be stopped and restarted while they are running. They are commonly defined with the <code>async</code> keyword. 

#### Futures

A Future is an item that is designed to hold the outcome of a corutine with which it is related. Future contains state information, which can typically be one of the following:

1. pending - there is no outcome or exception specified for the future.
2. canceled - the future has been canceled.
3. completed - the future was finished, either by a result set or by an exception. 

### Asynchrony vs multithreading

Asynchrony and multithreading often go hand in hand, but the concepts itself are orthogonal to each other and do not have to be implemented together.

1. Synchronous execution with single thread:

          single thread: AAAABBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCC

2. Synchronous execution with many threads:

         thread 1: AAAAAAAAAAAA---------------------------------

         thread 2: ------------BBBBBB--------------------------

         thread 3: ------------------CCCCCCCCCCCCCCCCCCCCCCCCCCC

3. Asynchronous execution with single thread:
   
          single thread: AAAABBBBAAAACCCAAABBBCCCCCBBBBBBBBBCCCCC

4. Asynchronous execution with many threads:

          thread 1: AAAAAAAAA-----
          thread 2: ---BBBBBBBBBBB
          thread 3: ----CCCC------

### Challenges

Asynchronous functions, unlike multithreading, switch cooperatively, hence the programmer is obliged to induce a task switch whenever it is thought suitable. Locks and other forms of synchronization are no longer required. Furthermore, the cost of task shifts is quite minimal. In most cases, calling a pure function incurs more cost than restarting a generator or awaitable. However, asynchronous functions (known as coruitnes) annotated with the async keyword are required. The challenge is, how will you deal with the vast world of functions written in a blocking manner? 

### Examples

#### Examples in C++
The <code>std::async()</code> function template is the simplest approach to make a function call asynchronous. This function receives the name of the function to be called as the first parameter, followed by a variadic list of callback parameters. For example, if we wish to call a function <code>int foo(int x, int y)</code> asynchronously, we may do it as follows: 

    auto future = std::async(foo, 3, 5);  
    auto result = future.get(); 

In this approach we have however very little control over the details. We don't even know if the callback was executed concurrently. There is a way to ensure that the function be executed in a separate thread. The template std::packaged_task wraps a function and provides a future for the functions return value, but the object itself is callable so we can delegate it to a separate thread.

    std::packaged_task<int(double, char, bool)> task(foo);
    auto future = task.get_future();
    std::thread thread(std::move(task), 3, 5);
    auto result = future.get();  
    
The thread starts running after the object gets created. We can either join or detach it.

Another option is to overwrite the default value of the first parameter that specifies how the task should be completed. We have two choices:

1. <code>std::launch::async</code> specifies that the function call must be executed on its own thread.
2. <code>std::launch::deferred</code> implies that we do not want to start a new thread and merely want to guarantee that the function is called before its return value is required. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name
     
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_read_result.cpp">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/future_status.cpp">future status</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/async_reuses_threads.cpp">async reuses threads</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/asynchrony/merge_sort.cpp">merge sort</a>

#### Examples in Python

When dealing with I/O operations, a multithreaded Python program that employs threading will be idle between the time the request is received and the time the answer is provided. If a thread knows when an I/O request has been delivered, it may switch to another task without becoming idle, and one thread should be sufficient to handle all of these jobs. Threading, obviously, would not be able to achieve this, but we have <code>asyncio</code>.

The main module we'll be using is named <code>asyncio</code>. It is distinguished for its outstanding speed and simplicity of usage. It makes it easy to write single-threaded concurrent programs using corutines (stripped down version of threads). It conceals the complexities of concurrent programming from us by providing a plethora of different functions that implement numerous synchronization strategies for us.

A coroutine can pause function execution by using the keywords <code>yield</code>, <code>yield from</code>, or <code>await</code> in an expression. The function is stopped until the yield statement returns a value.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_until_complete.py">loop run until complete</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/loop_run_forever.py">loop run forever</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_read_result.py">future read result</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/future_create_task.py">future create task</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/data_sharing_queue.py">data sharing queue</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/mutex.py">mutex</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/asynchrony/semaphore.py">semaphore</a>

#### Examples in JavaScript

* <a href=""></a>
* <a href=""></a>
* <a href=""></a>
* <a href=""></a>
