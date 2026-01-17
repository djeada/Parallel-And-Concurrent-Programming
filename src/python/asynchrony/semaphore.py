"""
Async Semaphore Example

This script demonstrates using asyncio.Semaphore to limit concurrent access
to a shared resource. Semaphores are essential for rate limiting and resource
pooling in async applications.

Key Concepts:
- asyncio.Semaphore(N): Allows up to N concurrent accesses
- 'async with semaphore:' acquires and releases automatically
- Semaphore(1) behaves like an async mutex (Lock)
- Tasks wait if the semaphore count reaches 0

Use Cases:
- Rate limiting API requests
- Connection pooling (database, HTTP)
- Limiting concurrent file operations
- Controlling resource consumption

Comparison with threading.Semaphore:
- asyncio.Semaphore uses 'async with' syntax
- Works within the async event loop
- No blocking - yields control while waiting
"""

import asyncio
import random

# Limit concurrent resource access to 3
resource_semaphore = asyncio.Semaphore(3)


async def limited_resource(task_id):
    """Access a limited resource protected by semaphore."""
    async with resource_semaphore:
        print(f"Task {task_id} is using the limited resource.")
        await asyncio.sleep(random.uniform(0.5, 2))
        print(f"Task {task_id} is done using the limited resource.")


async def main():
    """Create many tasks that compete for limited resources."""
    tasks = [asyncio.create_task(limited_resource(i)) for i in range(10)]
    await asyncio.gather(*tasks)


if __name__ == "__main__":
    asyncio.run(main())
