"""
Manager Example

This script demonstrates using multiprocessing.Manager to share complex
data structures between processes. Manager provides a high-level, flexible
way to share Python objects across processes.

Key Concepts:
- Manager creates a server process that hosts shared objects
- Supports list, dict, Namespace, Lock, Queue, and more
- Changes to shared objects are automatically synchronized
- Higher overhead than Value/Array but more flexible

Use Cases:
- Sharing complex data structures (lists, dicts, nested objects)
- When you need Python object semantics (not just ctypes)
- When ease of use is more important than raw performance

For performance-critical applications with simple data, prefer
Value/Array as they use direct shared memory access.
"""

import multiprocessing


def add_to_list(shared_list, value):
    """Add a value to the shared list."""
    shared_list.append(value)
    print(f"Process {value} added {value} to the shared list")


def main():
    # Create a Manager to host shared objects
    manager = multiprocessing.Manager()

    # Create a shared list managed by the Manager
    shared_list = manager.list()

    num_processes = 5
    processes = []

    # Create and start processes
    for i in range(num_processes):
        process = multiprocessing.Process(target=add_to_list, args=(shared_list, i))
        processes.append(process)
        process.start()

    # Wait for all processes to complete
    for process in processes:
        process.join()

    print("Shared list after all processes have finished:")
    print(list(shared_list))


if __name__ == "__main__":
    main()
