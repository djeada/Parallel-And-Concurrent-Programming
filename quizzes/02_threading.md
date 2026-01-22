# Multithreading Quiz

Test your understanding of threads, synchronization primitives, and thread management.

---

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

#### Q. When should a thread typically notify a condition_variable?

- [ ] Before locking the associated mutex
- [ ] It does not matter
- [x] After doing something to change the state associated with the condition_variable and unlocking the associated mutex
- [ ] After locking the associated mutex and checking whether or not the condition is true
- [ ] Before checking the condition

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

#### Q. In C++, a thread that needs to execute a section of code before the barrier should call the Barrier's _____ function _____ executing the code.

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

#### Q. Why are thread pools useful?

- [ ] Threads like to relax and go for a swim every once in a while.
- [ ] Threads within the same thread pool can more easily share data with each other than standard non-pool threads.
- [ ] They provide a convenient way to group and organize a collection of related threads.
- [x] They reuse threads to reduce the overhead that would be required to create a new, separate thread for every concurrent task.
- [ ] They automatically manage thread synchronization issues.

#### Q. In Python's threading module, which class provides a way to run code periodically in the background?

- [ ] BackgroundTask
- [x] Timer
- [ ] PeriodicExecutor
- [ ] IntervalThread
- [ ] PeriodicTask

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

#### Q. In Python, how can you ensure a section of code is executed by only one thread at a time?

- [ ] Use the Queue module
- [x] Use a Lock
- [ ] Use a Semaphore
- [ ] Use an Event
- [ ] Use a Timer

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

#### Q. In Python's threading module, which synchronization primitive allows multiple threads to synchronize on a shared resource by acquiring and releasing it?

- [ ] Barrier
- [x] Semaphore
- [ ] Event
- [ ] Timer
- [ ] Lock

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

#### Q. In Python's threading module, which method is used to start a new thread?

- [ ] run()
- [ ] create()
- [x] start()
- [ ] initiate()
- [ ] launch()

#### Q. In Python, which method is used to stop a thread immediately?

- [ ] terminate()
- [ ] kill()
- [ ] exit()
- [ ] stop()
- [x] None of the above

#### Q. In C++, a recursive_mutex can be locked _____.

- [ ] multiple times by different threads
- [x] multiple times by the same thread
- [ ] once by multiple threads at the same time
- [ ] only once in total
- [ ] depends on the compiler settings

#### Q. What is a "spinlock" and when is it useful?

- [ ] A lock that permanently holds a thread in place
- [x] A lock where a thread continuously checks (spins) until the lock becomes available, useful for short wait times
- [ ] A lock that rotates between threads automatically
- [ ] A lock that prevents threads from running too fast
- [ ] A lock used exclusively for disk I/O operations

#### Q. What is the difference between a Lock and an RLock in Python?

- [ ] Lock is for single-threaded programs; RLock is for multi-threaded
- [ ] Lock is faster than RLock
- [x] RLock can be acquired multiple times by the same thread; Lock cannot
- [ ] Lock is deprecated; RLock is the modern replacement
- [ ] There is no difference

#### Q. What happens if you call start() twice on the same Thread object in Python?

- [ ] The thread restarts from the beginning
- [ ] Nothing, the second call is ignored
- [x] A RuntimeError is raised
- [ ] The thread runs twice as fast
- [ ] The second call blocks until the first run completes

#### Q. In the context of threading, what is a "livelock"?

- [ ] A thread that is permanently locked
- [ ] Another name for deadlock
- [x] A situation where threads keep changing state in response to each other without making progress
- [ ] A thread that runs forever
- [ ] A lock that keeps a thread alive

#### Q. What is "thread-local storage" (TLS)?

- [ ] Storage that is locked by a thread
- [x] Memory that is unique to each thread and not shared with other threads
- [ ] Temporary storage for thread data
- [ ] Storage located near the CPU for threads
- [ ] Encrypted storage for sensitive thread data

#### Q. What is the purpose of a "read-write lock"?

- [ ] To prevent threads from reading and writing
- [x] To allow multiple readers or one writer at a time, but not both simultaneously
- [ ] To log all read and write operations
- [ ] To alternate between read and write modes
- [ ] To compress data during read/write operations

#### Q. In Java, what is the purpose of the volatile keyword?

- [ ] To make a variable constant
- [ ] To prevent garbage collection of the variable
- [x] To ensure that changes to the variable are visible to all threads
- [ ] To make the variable private to a thread
- [ ] To make the variable faster to access

#### Q. What is "thread starvation"?

- [ ] A thread that uses too much memory
- [ ] A thread that runs out of work
- [x] A thread that cannot access resources because other threads monopolize them
- [ ] A thread that has been terminated
- [ ] A thread with no input data

#### Q. What is a "monitor" in concurrent programming?

- [ ] A debugging tool for threads
- [x] A synchronization construct that combines mutex and condition variables
- [ ] A thread that watches other threads
- [ ] A process that monitors system resources
- [ ] A logging mechanism for threads

#### Q. What is "lock-free" programming?

- [ ] Programming without any synchronization
- [x] Designing algorithms that don't use traditional locks but still ensure thread safety
- [ ] Programming that automatically handles locking
- [ ] Single-threaded programming
- [ ] Programming that ignores race conditions

#### Q. What is a "compare-and-swap" (CAS) operation?

- [ ] An operation that compares two values and swaps them
- [x] An atomic operation that updates a value only if it matches an expected value
- [ ] A sorting algorithm
- [ ] A memory allocation technique
- [ ] A debugging operation

#### Q. What happens when a thread is "blocked"?

- [ ] The thread is terminated
- [ ] The thread runs at reduced speed
- [x] The thread is waiting for a resource and cannot proceed
- [ ] The thread is prevented from accessing memory
- [ ] The thread is locked by another thread

#### Q. What is the "thundering herd" problem in threading?

- [ ] Too many threads accessing memory at once
- [x] Many waiting threads all being woken up when only one can proceed, causing contention
- [ ] Threads creating too many child threads
- [ ] A memory leak caused by threads
- [ ] Threads consuming too much CPU

#### Q. What is priority inversion?

- [ ] When a low-priority thread always preempts high-priority threads
- [x] When a low-priority thread holds a resource needed by a higher-priority thread, blocking it
- [ ] When thread priorities are randomly shuffled
- [ ] When all threads run at the same priority
- [ ] When a thread's priority increases without reason

#### Q. What is a thread pool best suited for?

- [ ] Single long-running task
- [ ] Tasks requiring exclusive hardware access
- [x] Many short-lived tasks to amortize creation overhead
- [ ] Tasks that must run sequentially
- [ ] Tasks that cannot be parallelized
