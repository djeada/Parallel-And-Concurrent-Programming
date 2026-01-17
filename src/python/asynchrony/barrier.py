"""
Async Barrier Example

This script demonstrates asyncio.Barrier for synchronizing multiple async
tasks at a specific point in their execution. All tasks must reach the
barrier before any can proceed past it.

Key Concepts:
- asyncio.Barrier(N): Blocks until N tasks call wait()
- barrier.wait(): Async method that blocks until all parties arrive
- Once all arrive, all are released simultaneously
- Useful for phased parallel algorithms

Use Cases:
- Multi-phase algorithms where all workers must complete each phase
- Ensuring all resources are initialized before processing
- Coordinating parallel simulations
- Synchronizing distributed task execution

Note: asyncio.Barrier was added in Python 3.11. For earlier versions,
you can implement similar behavior with asyncio.Event or conditions.
"""

import asyncio
import random


async def task_worker(task_id, barrier):
    """
    Worker that performs work in phases, synchronizing at the barrier.

    Phase 1: Initial work with random duration
    Barrier: Wait for all workers
    Phase 2: Continue with more work
    """
    print(f"Task {task_id} starting phase 1...")
    await asyncio.sleep(random.randint(1, 5))  # Simulate work
    print(f"Task {task_id} finished phase 1")

    # Wait for all tasks to complete phase 1
    await barrier.wait()

    print(f"Task {task_id} starting phase 2...")
    await asyncio.sleep(random.randint(1, 5))  # Simulate more work
    print(f"Task {task_id} finished phase 2")


async def main():
    """Create tasks and synchronize them with a barrier."""
    num_tasks = 5
    barrier = asyncio.Barrier(num_tasks)

    tasks = [asyncio.create_task(task_worker(i, barrier)) for i in range(num_tasks)]

    await asyncio.gather(*tasks)
    print("All tasks completed both phases.")


if __name__ == "__main__":
    asyncio.run(main())
