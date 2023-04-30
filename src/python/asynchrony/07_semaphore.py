"""
This Python script demonstrates the use of an async semaphore to control access
to a limited resource in an asynchronous context. Async semaphores work similarly
to threading semaphores but are specifically designed for use with async functions
and coroutines.

In this example, we simulate multiple tasks attempting to access a limited resource
simultaneously. The resource can only handle three concurrent accesses. We use
an async semaphore to ensure that no more than three tasks access the resource at
the same time.
"""

import asyncio
import random

# The limited resource can only handle three concurrent accesses.
resource_semaphore = asyncio.Semaphore(3)


async def limited_resource(task_id):
    async with resource_semaphore:
        print(f"Task {task_id} is using the limited resource.")
        await asyncio.sleep(random.uniform(0.5, 2))
        print(f"Task {task_id} is done using the limited resource.")


async def main():
    tasks = [asyncio.create_task(limited_resource(i)) for i in range(10)]
    await asyncio.gather(*tasks)


if __name__ == "__main__":
    asyncio.run(main())
