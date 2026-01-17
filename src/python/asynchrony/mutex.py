"""
Async Mutex (Lock) Example

This script demonstrates using asyncio.Lock to protect shared resources
in async code. The Lock ensures that only one coroutine can access the
protected section at a time, preventing race conditions.

Key Concepts:
- asyncio.Lock: Mutual exclusion for async coroutines
- 'async with lock:' acquires and releases automatically
- Prevents race conditions in shared mutable state
- Unlike threading.Lock, uses await instead of blocking

Use Cases:
- Protecting shared data structures
- Ensuring atomic operations
- Coordinating access to external resources
- Preventing concurrent modifications

This example simulates a bank account with concurrent transfers.
Without the lock, transfers could interleave and produce incorrect balances.
"""

import asyncio


class BankAccount:
    """A bank account protected by an async lock."""

    def __init__(self, balance: int):
        self.balance = balance
        self.lock = asyncio.Lock()

    async def transfer(self, amount: int):
        """Transfer money (positive = deposit, negative = withdrawal)."""
        async with self.lock:
            print(f"Transferring {amount}...")
            await asyncio.sleep(0.1)  # Simulate processing time
            self.balance += amount
            print(f"Transfer of {amount} complete. New balance: {self.balance}")


async def transfer_money(account: BankAccount, amounts: list):
    """Perform a series of transfers on the account."""
    for amount in amounts:
        await account.transfer(amount)


async def main():
    """Demonstrate concurrent transfers protected by a lock."""
    account = BankAccount(100)

    # Create concurrent transfer tasks
    transfer1 = asyncio.create_task(transfer_money(account, [50, -20, 30]))
    transfer2 = asyncio.create_task(transfer_money(account, [-10, 60, -10]))

    await asyncio.gather(transfer1, transfer2)

    print(f"Final account balance: {account.balance}")


if __name__ == "__main__":
    asyncio.run(main())
