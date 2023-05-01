"""
This script demonstrates the use of asyncio.Lock (a.k.a. async mutex) to protect
access to shared resources in an asynchronous environment. In this example, we
simulate a bank account with two coroutines attempting to transfer money concurrently.
The use of an async mutex (lock) is crucial to prevent race conditions that could lead
to incorrect account balances.
"""

import asyncio


class BankAccount:
    def __init__(self, balance: int):
        self.balance = balance
        self.lock = asyncio.Lock()

    async def transfer(self, amount: int):
        async with self.lock:
            print(f"Transferring {amount}...")
            await asyncio.sleep(0.1)  # Simulate some processing time
            self.balance += amount
            print(f"Transfer of {amount} complete. New balance: {self.balance}")


async def transfer_money(account: BankAccount, amounts: list):
    for amount in amounts:
        await account.transfer(amount)


async def main():
    account = BankAccount(100)

    transfer1 = asyncio.create_task(transfer_money(account, [50, -20, 30]))
    transfer2 = asyncio.create_task(transfer_money(account, [-10, 60, -10]))

    await asyncio.gather(transfer1, transfer2)

    print(f"Final account balance: {account.balance}")


if __name__ == "__main__":
    asyncio.run(main())
