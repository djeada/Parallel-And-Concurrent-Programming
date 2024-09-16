#### Q. In Python's threading module, which method allows a thread to wait until it is notified?

- [ ] wait()
- [ ] hold()
- [x] wait() on a Condition object
- [ ] sleep()
- [ ] pause()

#### Q. In Python's threading module, which class should be used to implement a thread-safe counter?

- [ ] Lock
- [ ] Semaphore
- [ ] Event
- [ ] Timer
- [x] RLock

#### Q. A _____ contains one or more _____.

- [ ] process; other processes
- [ ] thread; processes
- [ ] thread; other threads
- [x] process; threads

#### Q. Parallel computing primarily enhances the _____.

- [ ] number of tasks a program executes in a set time
- [x] speed at which a program executes a set number of tasks
- [ ] scale of problems a program can tackle
- [ ] all of these answers
- [ ] efficiency of memory usage

#### Q. A Symmetric Multi-Processing (SMP) system has two or more _____ processors connected to a single _____ main memory.

- [x] identical; shared
- [ ] dissimilar; shared
- [ ] dissimilar; distributed
- [ ] identical; distributed
- [ ] asynchronous; shared

#### Q. The operating system assigns each process a unique _____.

- [x] process ID number
- [ ] CPU core
- [ ] process name
- [ ] number of threads
- [ ] memory address

#### Q. In C++, which of these scenarios describes the best use case for using a shared_mutex?

- [ ] Lots of threads need to modify the value of a shared variable, but only a few threads need to read its value.
- [ ] Only a few threads need to both read and modify the value of a shared variable.
- [ ] Lots of threads need to both read and modify the value of a shared variable.
- [x] Lots of threads need to read the value of a shared variable, but only a few threads need to modify its value.
- [ ] All threads need exclusive access to the shared variable.

#### Q. Why would you use detached daemon threads to handle continuous background tasks?

- [ ] Daemon threads always execute at the lowest priority which makes them ideal for background tasks.
- [ ] Daemon threads will automatically spawn additional helper threads as needed when their workload increases.
- [ ] You should never trust a daemon. They're mischievous!
- [x] The daemon thread will not prevent the program from terminating when the main thread is finished.
- [ ] Daemon threads are more efficient for high-priority tasks.

#### Q. Processes _____ than threads.

- [ ] are simpler to communicate between
- [ ] are considered more "lightweight"
- [ ] are faster to switch between
- [x] require more overhead to create

#### Q. In C++, what happens to a std::thread object immediately after being instantiated?

- [x] It begins execution, pending OS scheduling.
- [ ] It waits in a "blocked" state until the parent thread starts it.
- [ ] It waits in a "new thread" state until the parent thread starts it.
- [ ] It remains idle until explicitly started.

#### Q. Barriers can be used to control _____.

- [ ] runaway threads to prevent memory leaks
- [x] the relative order in which threads execute certain operations
- [ ] when operations are executed atomically
- [ ] the priority that determines which threads get scheduled first
- [ ] the maximum number of threads that can be created

#### Q. In C++, what is the maximum number of threads that can have shared ownership of a shared_mutex at the same time?

- [ ] 1
- [ ] 2
- [ ] 0
- [x] no limit
- [ ] depends on the implementation

#### Q. In C++, calling the detach() function on a thread makes it joinable.

- [x] FALSE
- [ ] TRUE
- [ ] It depends on the thread state.
- [ ] Only if the thread has not started yet.

#### Q. In C++, how many times must a thread unlock a recursive_mutex before another thread can acquire it?

- [ ] once
- [ ] none because multiple threads can lock a recursive_mutex at the same time
- [x] as many times as that thread locked it
- [ ] twice
- [ ] depends on the priority of the threads

#### Q. In C++, what happens when a thread calls the try_lock() function on a mutex that is NOT currently locked by another thread?

- [ ] The function will block execution until the mutex is available, then lock it and return true.
- [x] The function immediately locks the mutex and returns true.
- [ ] The function will block execution until the mutex is available, then lock it and return false.
- [ ] The function immediately locks the mutex and returns false.
- [ ] The function does nothing and returns false.

#### Q. A hyperthreaded processor with 8 logical cores will usually provide _____ performance compared to a regular processor with 8 physical cores.

- [x] lower
- [ ] higher
- [ ] equivalent
- [ ] variable

#### Q. Which of these applications would benefit the most from parallel execution?

- [x] tool for downloading multiple files from the Internet at the same time
- [ ] math library for processing large matrices
- [ ] system logging application that frequently writes to a database
- [ ] graphical user interface (GUI) for an accounting application
- [ ] real-time data streaming service

#### Q. You can safely expect threads to execute in the same relative order that you create them.

- [ ] TRUE
- [x] FALSE
- [ ] It depends on the operating system
- [ ] It depends on the processor

#### Q. Flynn's Taxonomy categorizes computer architectures based on the number of concurrent _____ streams and _____ streams.

- [ ] input; output
- [x] instruction; data
- [ ] program; data
- [ ] memory; I/O
- [ ] data; cache

#### Q. Which of these describes a program's "latency"?

- [ ] ratio of sequential execution time to the parallel execution time with some number of processors
- [x] amount of time a task takes to execute
- [ ] number of tasks that can be executed in a certain amount of time
- [ ] number of processors used by the program

#### Q. If 85% of a program is parallelizable so that using an 6-core processor will produce a 6x speedup for that portion of the code, what is the maximum overall speedup the program can achieve?

- [x] 4.62
- [ ] 5.1
- [ ] 1
- [ ] 6
- [ ] 5.5

#### Q. In C++, a recursive_mutex can be locked _____.

- [ ] multiple times by different threads
- [x] multiple times by the same thread
- [ ] once by multiple threads at the same time
- [ ] only once in total
- [ ] depends on the compiler settings

#### Q. What is an advantage of using fine-grained parallelism with a large number of small tasks?

- [ ] high computation-to-communication ratio
- [ ] poor load balancing
- [ ] low computation-to-communication ratio
- [x] good load-balancing
- [ ] reduced synchronization overhead

#### Q. Distributed memory architectures are often considered _____ compared to shared memory systems.

- [ ] more responsive
- [x] more scalable
- [ ] easier to program for
- [ ] less complex
- [ ] more cost-effective

#### Q. What does calculating a program's efficiency (speedup divided by number of parallel processors) provide an indicator of?

- [ ] the maximum number of parallel processors the program can utilize
- [ ] how well the parallel program is performing compared to its sequential implementation
- [x] how well the parallel processing resources are being utilized
- [ ] the optimal number of parallel processors for the program to use
- [ ] the minimum number of processors needed

#### Q. In C++, why is the try_lock() function useful?

- [x] It enables a thread to execute alternate operations if the mutex it needs to acquire is already taken.
- [ ] It enforces fairness among multiple threads competing for ownership of the same mutex.
- [ ] If multiple threads try to lock a mutex simultaneously, the try_lock() method will randomly pick one to succeed.
- [ ] It includes built-in protection against common locking errors.
- [ ] It guarantees immediate access to the mutex.

#### Q. In C++, what is the maximum number of threads that can have exclusive ownership of a shared_mutex at the same time?

- [x] 1
- [ ] no limit
- [ ] 2
- [ ] 0
- [ ] depends on the operating system

#### Q. Which classification of Flynn's Taxonomy do modern multi-core PCs fall under?

- [ ] SIMD
- [ ] MISD
- [ ] SISD
- [x] MIMD
- [ ] SPMD

#### Q. In most modern multi-core CPUs, cache coherency is usually handled by the _____.

- [ ] user
- [ ] application software
- [ ] operating system
- [x] processor hardware
- [ ] compiler

#### Q. A thread that calls the join method on another thread will enter a _____ state until the other thread finishes executing.

- [ ] new
- [ ] runnable
- [ ] terminated
- [x] blocked
- [ ] waiting


#### Q. Why would you use the condition_variable's notify_one() function instead of notify_all()?

- [ ] You need to wake up a random number of waiting threads.
- [ ] You need all of the waiting threads to wake up and continue executing.
- [ ] You need to wake up one specific waiting thread.
- [x] You only need to wake up one waiting thread and it does not matter which one.
- [ ] You want to ensure maximum efficiency.

#### Q. It is possible for two tasks to execute _____ using a single-core processor.

- [ ] in parallel
- [x] concurrently
- [ ] concurrently or in parallel
- [ ] sequentially

#### Q. When should a thread typically notify a condition_variable?

- [ ] Before locking the associated mutex
- [ ] It does not matter
- [x] After doing something to change the state associated with the condition_variable and unlocking the associated mutex
- [ ] After locking the associated mutex and checking whether or not the condition is true
- [ ] Before checking the condition

#### Q. How should the average rates of production and consumption be related in a producer-consumer architecture?

- [ ] The consumption and production rates must be exactly the same.
- [ ] The consumption rate should be greater than or equal to the production rate.
- [ ] It does not matter.
- [x] The consumption rate should be less than or equal to the production rate.
- [ ] The production rate should always exceed the consumption rate.

#### Q. Which of these is a common use case for a counting semaphore?

- [x] track the availability of a limited resource
- [ ] track how many threads the program has created
- [ ] track how long a program has been running
- [ ] enforce mutual exclusion in a critical section of code
- [ ] monitor memory usage

#### Q. Which two mechanisms can be used together with a queue to make it thread-safe?

- [ ] semaphore and condition_variable
- [ ] mutex and semaphore
- [ ] standard C++ queues are already thread-safe
- [x] mutex and condition_variable
- [ ] atomic variables and spinlocks

#### Q. In CPP a thread that needs to execute a section of code before the barrier should call the Barrier's _____ function _____ executing the code.

- [ ] wait(); before
- [x] wait(); after
- [ ] notify_all(); after
- [ ] notify_all(); before
- [ ] signal(); before

#### Q. Which function can be used to submit tasks to a Boost C++ Library thread_pool?

- [ ] std::async()
- [ ] boost::asio::submit()
- [x] boost::asio::post()
- [ ] boost::asio::swim()
- [ ] boost::asio::execute()

#### Q. In C++, how many threads can take shared ownership of a shared_mutex while another thread has exclusive ownership of it?

- [ ] 1
- [ ] 2
- [ ] no limit
- [x] 0
- [ ] depends on the operating system

#### Q. In C++, what is the difference between the try_lock() and regular lock() functions?

- [ ] try_lock() checks whether or not a mutex is already taken without actually locking it.
- [x] try_lock() will not block execution if the mutex is already taken by another thread.
- [ ] try_lock() will continuously try to lock the mutex if it is already taken by another thread.
- [ ] lock() returns a boolean value to indicate whether or not it was successful.
- [ ] lock() can be used only with specific mutex types.


#### Q. What type of object does the std::async() function return?

- [x] Future
- [ ] Thread
- [ ] Thread Pool
- [ ] Boolean
- [ ] Promise

#### Q. What does the semaphore's acquire() function do to the counter value?

- [ ] always decrement the counter's value
- [ ] if the counter is positive, increment its value
- [ ] always increment the counter's value
- [x] if the counter is positive, decrement its value
- [ ] reset the counter value

#### Q. Which statement describes a difference between a binary semaphore and a mutex?

- [ ] The binary semaphore will have a value of 0, 1, 2, 3, etc.
- [x] The binary semaphore can be acquired and released by different threads.
- [ ] The binary semaphore can only be acquired and released by the same thread.
- [ ] The binary semaphore can have a positive or negative value.
- [ ] The binary semaphore does not enforce mutual exclusion.

#### Q. In addition to modifying the counter value, what else does calling the semaphore's release() function do?

- [ ] block and wait until the semaphore is available
- [ ] nothing else
- [ ] block all other threads waiting on the semaphore
- [x] signal another thread waiting to acquire the semaphore
- [ ] reset the semaphore's counter

#### Q. Condition variables serve as a _____ for threads to _____.

- [ ] storage location; save their current state information
- [ ] conditional statement; pick among several possible paths of execution
- [x] holding place; wait for a certain condition before continuing execution
- [ ] checkpoint; synchronize their execution steps
- [ ] lock; ensure mutual exclusion

#### Q. What does the semaphore's release() function do to the counter value?

- [ ] if the counter is positive, decrement its value
- [ ] if the counter is positive, increment its value
- [x] always increment the counter's value
- [ ] always decrement the counter's value
- [ ] reset the counter to zero

#### Q. Condition variables work together with which other mechanism to serve as a monitor?

- [x] a mutex
- [ ] a thread
- [ ] the OS execution scheduler
- [ ] a process
- [ ] a semaphore

#### Q. Which of these describes a program's "throughput"?

- [x] number of tasks that can be executed in a certain amount of time
- [ ] ratio of sequential execution time to the parallel execution time with some number of processors
- [ ] amount of time a task takes to execute
- [ ] number of processors used by the program

#### Q. Which of these is responsible for causing a race condition?

- [ ] corrupted memory
- [ ] the execution scheduler
- [ ] a slow Internet connection
- [x] not using a semaphore
- [ ] inadequate CPU resources

#### Q. A race condition _____ a data race.

- [ ] only occurs when there is
- [x] can occur independently of
- [ ] is just another name for
- [ ] is more severe than
- [ ] is unrelated to

#### Q. In most operating systems the _____ determines when each of the threads and processes gets scheduled to execute.

- [ ] operating system
- [x] processor hardware
- [ ] application software
- [ ] user
- [ ] scheduler algorithm

#### Q. Which of these describes a program's "critical path"?

- [x] longest series of sequential operations through the program
- [ ] sum of the time for all task nodes in a computational graph
- [ ] sum of the time for all task nodes along the critical path
- [ ] shortest execution path through the program

I apologize for that. Here's the corrected version of the questions tailored for Python without any reference to C++:

#### Q. In Python, a thread that needs to execute a section of code before the barrier should call the Barrier's _____ function _____ executing the code.

- [ ] wait(); before
- [x] wait(); after
- [ ] notify_all(); after
- [ ] notify_all(); before
- [ ] signal(); before

#### Q. Which function can be used to submit tasks to a concurrent.futures ThreadPoolExecutor in Python?

- [ ] submit_task()
- [ ] add_task()
- [x] submit()
- [ ] enqueue()
- [ ] execute()

#### Q. In Python's threading module, how many threads can take shared ownership of a Lock while another thread has exclusive ownership of it?

- [ ] 1
- [ ] 2
- [ ] no limit
- [x] 0
- [ ] depends on the operating system

#### Q. In Python's threading module, what is the difference between the try_lock() (try_acquire() in Python) and regular lock() (acquire() in Python) functions?

- [ ] try_lock() checks whether or not a mutex is already taken without actually locking it.
- [x] try_lock() (try_acquire()) will not block execution if the mutex is already taken by another thread.
- [ ] try_lock() (try_acquire()) will continuously try to lock the mutex if it is already taken by another thread.
- [ ] lock() (acquire()) returns a boolean value to indicate whether or not it was successful.
- [ ] lock() (acquire()) can be used only with specific mutex types.


#### Q. What does a work-to-span ratio less than one indicate?

- [ ] A parallelized version of the program will execute faster than the sequential version.
- [ ] A parallelized version of the program will execute in the same amount of time as the sequential version.
- [ ] The work-to-span ratio cannot be less than one.
- [x] A parallelized version of the program will execute slower than the sequential version.
- [ ] The program has no parallelism opportunities.

#### Q. Why are thread pools useful?

- [ ] Threads like to relax and go for a swim every once in a while.
- [ ] Threads within the same thread pool can more easily share data with each other than standard non-pool threads.
- [ ] They provide a convenient way to group and organize a collection of related threads.
- [x] They reuse threads to reduce the overhead that would be required to create a new, separate thread for every concurrent task.
- [ ] They automatically manage thread synchronization issues.

#### Q. Increasing the number of processors with a fixed problem size per processor leverages weak scaling to accomplish _____ in _____.

- [ ] more work; more time
- [ ] same work; less time
- [x] more work; less time
- [ ] more work; same time
- [ ] less work; same time

#### Q. What is the purpose of a future?

- [ ] It serves as the counterpart to a programming past.
- [ ] It is a task that can be assigned to a thread pool for execution.
- [x] It serves as a placeholder to access a result that may not been computed yet.
- [ ] It allows a program to change how it will function the next time it is run.
- [ ] It provides error handling for asynchronous operations.

#### Q. Which of these describes a program's "work"?

- [ ] sum of the time for all task nodes along the critical path
- [ ] longest series of sequential operations through the program
- [x] sum of the time for all task nodes in a computational graph
- [ ] total number of operations performed by the program

#### Q. What does a divide-and-conquer algorithm do when it reaches the base case?

- [x] Stop subdividing the current problem and solve it.
- [ ] Recursively solve a set of smaller subproblems.
- [ ] Solve all of the subproblems that have been created.
- [ ] Divide the problem into two smaller subproblems.
- [ ] Return the initial problem unchanged.

#### Q. Why are computational graphs useful?

- [x] They include all of the tasks within a program, from start to finish.
- [ ] They help to identify critical sections of code that will require mutual exclusion.
- [ ] They provide a good excuse to practice your drawing skills.
- [x] They help to identify opportunities for parallel execution.
- [ ] They simplify the debugging process.

#### Q. Why should you average the execution time across multiple runs when measuring a program's performance?

- [ ] The system clock used to determine the start and end times of the program is inconsistent and may fluctuate randomly.
- [ ] It's good luck to do things multiple times.
- [ ] The program might crash so you should measure it multiple times to make sure you get at least one good run.
- [x] The execution time will vary from run-to-run depending on how the operating system chooses to schedule your program.
- [ ] To account for hardware variability.

#### Q. Which stage of the parallel design process focuses on combining tasks and replicating data or computation as needed to increase program efficiency?

- [ ] mapping
- [x] agglomeration
- [ ] communication
- [ ] partitioning
- [ ] synchronization

#### Q. In Python's threading module, which class provides a way to run code periodically in the background?

- [ ] BackgroundTask
- [x] Timer
- [ ] PeriodicExecutor
- [ ] IntervalThread
- [ ] PeriodicTask

#### Q. In Python's concurrent.futures module, which method is used to cancel a future that has not started execution?

- [ ] stop()
- [ ] terminate()
- [ ] interrupt()
- [x] cancel()
- [ ] halt()

#### Q. Which of these describes a program's "span"?

- [ ] longest series of sequential operations through the program
- [x] sum of the time for all task nodes along the critical path
- [ ] sum of the time for all task nodes in a computational graph
- [ ] total execution time of the program

#### Q. Every thread is independent and has its own separate address space in memory.

- [x] FALSE
- [ ] TRUE
- [ ] It depends on the operating system
- [ ] It depends on the language used

#### Q. In Python's threading module, what does the is_alive() method do?

- [ ] Returns the thread's priority
- [ ] Returns the thread's current state
- [x] Returns whether the thread is still running
- [ ] Returns the thread's identifier
- [ ] Returns the thread's exit code

#### Q. In Python's threading module, which object can be used to wake up all threads waiting on it?

- [ ] Timer
- [ ] Barrier
- [ ] Lock
- [x] Event
- [ ] Semaphore

#### Q. In Python's threading module, which method can be used to put the current thread to sleep for a specified amount of time?

- [ ] rest()
- [ ] wait()
- [x] sleep()
- [ ] pause()
- [ ] delay()

#### Q. In Python, which module provides support for high-level asynchronous I/O?

- [x] asyncio
- [ ] threading
- [ ] multiprocessing
- [ ] concurrent
- [ ] sched

#### Q. In Python's asyncio module, which function is used to create a new task?

- [ ] start_task()
- [ ] new_task()
- [x] create_task()
- [ ] schedule_task()
- [ ] add_task()

#### Q. In Python, how can you ensure a section of code is executed by only one thread at a time?

- [ ] Use the Queue module
- [x] Use a Lock
- [ ] Use a Semaphore
- [ ] Use an Event
- [ ] Use a Timer

#### Q. In Python's asyncio module, which method of a future object returns the result or raises the exception of the future?

- [ ] fetch()
- [ ] obtain()
- [x] result()
- [ ] acquire()
- [ ] get_value()

#### Q. Why can it be risky to use a detached daemon thread to perform a task that involves writing data to a log file?

- [ ] Daemons only write their own name, so the log will just say "Daemon, Daemon, Daemon, etc."
- [ ] Daemon threads cannot read or write files.
- [x] The log file could be corrupted.
- [ ] The log file could end up with multiple, duplicate entries.
- [ ] Daemon threads might not have file access permissions.

#### Q. In Python's concurrent.futures module, which class is used to create a pool of threads?

- [ ] ThreadPool
- [x] ThreadPoolExecutor
- [ ] ThreadManager
- [ ] TaskPool
- [ ] ConcurrentExecutor

#### Q. Which of these describes a program's "speedup"?

- [ ] amount of time a task takes to execute
- [x] ratio of sequential execution time to the parallel execution time with some number of processors
- [ ] number of tasks that can be executed in a certain amount of time
- [ ] number of processors used by the program

#### Q. In Python's threading module, which synchronization primitive allows multiple threads to synchronize on a shared resource by acquiring and releasing it?

- [ ] Barrier
- [x] Semaphore
- [ ] Event
- [ ] Timer
- [ ] Lock

#### Q. Increasing the number of processors with a fixed total problem size leverages strong scaling to accomplish _____ in _____.

- [ ] more work; less time
- [x] same work; less time
- [ ] more work; same time
- [ ] more work; more time
- [ ] less work; more time

#### Q. Amdahl's Law calculates a(n) _____ for the overall speedup that parallelizing a program will achieve.

- [ ] upper and lower limit
- [ ] lower limit
- [x] upper limit
- [ ] average value
- [ ] median value

#### Q. What is an advantage of using coarse-grained parallelism with a small number of large tasks?

- [x] high computation-to-communication ratio
- [ ] poor load balancing
- [ ] low computation-to-communication ratio
- [ ] good load-balancing
- [ ] reduced memory overhead

#### Q. Why does the mapping design stage not apply to applications written for common desktop operating systems?

- [x] The operating system automatically handles scheduling threads to execute on each processor core.
- [ ] Desktop computers do not have enough processor cores for mapping to be a concern.
- [ ] Most desktop computers only have one processor core.
- [ ] The mapping design stage is not necessary for modern applications.
- [ ] Mapping is handled by the application itself.

#### Q. Why does the partitioning design stage occur before the communication stage?

- [ ] You need to group the tasks together before planning the most effective communication strategy between them.
- [x] You need to know how the problem will be divided in order to assess the communication needs between individual tasks.
- [ ] You need to know where each of the tasks will physically execute before deciding on a communication strategy.
- [ ] The "order" of the four design stages is arbitrary and does not really matter.
- [ ] Communication requirements determine how tasks are grouped.

#### Q. Which stage of the parallel design process focuses on breaking the problem down into discrete pieces of work?

- [ ] mapping
- [ ] agglomeration
- [ ] communication
- [x] partitioning
- [ ] synchronization

#### Q. In Python's threading module, which method is used to start a new thread?

- [ ] run()
- [ ] create()
- [x] start()
- [ ] initiate()
- [ ] launch()

#### Q. Which scenario describes the best use case for a point-to-point communication strategy?

- [ ] A small number of tasks need to send data a large number of other tasks.
- [ ] A large number of tasks will be sending data to a small number of receiving tasks.
- [ ] A large number of tasks need to communicate with each other.
- [x] A small number of tasks need to communicate with each other.
- [ ] All tasks need to broadcast to all other tasks.

#### Q. Which stage of the parallel design process focuses on coordinating task execution and how they share information?

- [ ] mapping
- [ ] agglomeration
- [ ] partitioning
- [x] communication
- [ ] synchronization

#### Q. Granularity can be described as the ratio of _____ over _____.

- [ ] speedup; number of processors
- [ ] tasks; time
- [ ] tasks; number of processors
- [x] computation; communication
- [ ] workload; processors

#### Q. Which architecture consists of a chained together series of producer-consumer pairs?

- [ ] FIFO
- [x] pipeline
- [ ] distributed
- [ ] client-server
- [ ] hierarchical

#### Q. In Python's concurrent.futures module, which method is used to get the result of a future?

- [ ] fetch()
- [ ] obtain()
- [ ] get()
- [x] result()
- [ ] acquire()

#### Q. In Python, which method is used to stop a thread immediately?

- [ ] terminate()
- [ ] kill()
- [ ] exit()
- [ ] stop()
- [x] None of the above

#### Q. Why would ThreadA call the join() function on ThreadB?

- [ ] ThreadB needs to wait until after ThreadA has terminated to continue.
- [x] ThreadA needs to wait until after ThreadB has terminated to continue.
- [ ] ThreadB is blocked so ThreadA needs to tell it to continue executing.
- [ ] ThreadA needs to terminate ThreadB immediately.
- [ ] ThreadA and ThreadB need to synchronize execution.

#### Q. In Python, which module provides a way to create and manage threads?

- [x] threading
- [ ] multiprocessing
- [ ] asyncio
- [ ] concurrent
- [ ] sched

#### Q. In Python's threading module, what is the purpose of the join() method?

- [ ] To start a thread
- [ ] To pause a thread
- [ ] To stop a thread
- [x] To wait for a thread to finish
- [ ] To notify a thread

#### Q. In Python's threading module, which method is used to release a lock?

- [x] release()
- [ ] unlock()
- [ ] free()
- [ ] open()
- [ ] exit()

#### Q. In Python's asyncio module, which function runs the event loop until the future (task) completes?

- [ ] run()
- [ ] execute()
- [ ] perform()
- [x] run_until_complete()
- [ ] run_forever()

#### Q. Which stage of the parallel design process focuses on specifying where each task will execute?

- [ ] communication
- [x] mapping
- [ ] agglomeration
- [ ] partitioning
- [ ] load balancing

#### Q. Which scenario creates the potential for a race condition to occur?

- [ ] the order in which two threads execute their respective operations will change the output
- [x] one thread is modifying a shared variable while another thread concurrently reads its value
- [ ] a single-threaded program is competing with other processes for execution time on the CPU
- [ ] two threads are concurrently reading and writing the same shared variable
- [ ] multiple threads are waiting for a condition variable to be notified
