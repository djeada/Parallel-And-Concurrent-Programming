"""
Async Generator Example

This script demonstrates async generators, which combine the power of
generators with async/await. They allow yielding values asynchronously,
ideal for streaming data or async iteration.

Key Concepts:
- 'async def' + 'yield': Creates an async generator
- 'async for': Iterates over an async generator
- Each yield can be preceded by await operations
- Values are produced on-demand, not all at once

Use Cases:
- Streaming data from async sources (WebSockets, databases)
- Paginated API responses
- Processing large datasets without loading all into memory
- Real-time event streams

Comparison with Regular Generators:
- Regular: def + yield, use with 'for'
- Async: async def + yield, use with 'async for'
- Both are lazy (produce values on demand)
"""

import asyncio
import random


async def async_generator():
    """
    Async generator that yields values with delays.

    Each iteration simulates fetching data asynchronously.
    """
    for i in range(5):
        # Simulate async operation (e.g., fetching from API)
        await asyncio.sleep(random.uniform(0.5, 1.5))
        yield i


async def main():
    """Consume values from the async generator."""
    print("Starting async iteration...")

    async for value in async_generator():
        print(f"Received value: {value}")

    print("Async iteration complete.")


if __name__ == "__main__":
    asyncio.run(main())
