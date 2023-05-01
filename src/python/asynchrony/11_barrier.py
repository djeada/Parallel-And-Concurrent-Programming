"""
This script demonstrates the use of asyncio.Barrier in Python. asyncio.Barrier is
a synchronization primitive that can be used to coordinate multiple async tasks.
In this example, we have a set of tasks that each perform some work and then
wait for all tasks to reach the barrier before continuing. This ensures that
the tasks move to the next phase of their work in a coordinated fashion.
"""

import asyncio
import random

async def task_worker(task_id, barrier):
    print(f"Task {task_id} starting phase 1...")
    await asyncio.sleep(random.randint(1, 5))  # Simulate some processing time
    print(f"Task {task_id} finished phase 1")
    
    await barrier.wait()  # Wait for all tasks to reach the barrier

    print(f"Task {task_id} starting phase 2...")
    await asyncio.sleep(random.randint(1, 5))  # Simulate some processing time
    print(f"Task {task_id} finished phase 2")

async def main():
    num_tasks = 5
    barrier = asyncio.Barrier(num_tasks)

    tasks = [asyncio.create_task(task_worker(i, barrier)) for i in range(num_tasks)]

    await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())
