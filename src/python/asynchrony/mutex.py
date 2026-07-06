"""
Async Lock Example: Preventing a real race condition

Problem:
Two async tasks try to withdraw money from the same bank account.

The account starts with $100.
Two ATMs both try to withdraw $80 at the same time.

Correct behavior:
- One withdrawal should succeed
- The other should fail because only $20 remains

Without a lock:
- Both tasks may read the balance as $100
- Both think the withdrawal is allowed
- Both dispense cash
- The stored balance becomes wrong compared to the money actually dispensed

With asyncio.Lock:
- Only one task can check and update the balance at a time
- The second task sees the updated balance
- The account rules stay correct

Key lesson:
A race condition can happen in async code whenever you:
1. read shared state
2. await something
3. write shared state based on the old value

The lock protects that read-modify-write sequence.
"""

import asyncio


class BankAccount:
    def __init__(self, balance: int):
        self.balance = balance
        self.cash_dispensed = 0
        self.lock = asyncio.Lock()

    async def withdraw_without_lock(self, atm_name: str, amount: int) -> None:
        """
        Broken version.

        This shows what can go wrong when two coroutines access shared state
        without coordination.
        """
        print(f"{atm_name}: wants to withdraw ${amount}")

        # Step 1: read shared state
        balance_seen = self.balance
        print(f"{atm_name}: sees balance = ${balance_seen}")

        if balance_seen < amount:
            print(f"{atm_name}: rejected, not enough money")
            return

        # Step 2: await while still depending on the old balance
        #
        # This simulates something slow:
        # - calling a payment service
        # - writing an audit record
        # - waiting for a database operation
        #
        # While this coroutine is paused, another coroutine can run and read
        # the same old balance.
        await asyncio.sleep(0.5)

        # Step 3: write shared state using a stale value
        self.balance = balance_seen - amount
        self.cash_dispensed += amount

        print(
            f"{atm_name}: approved ${amount}. "
            f"New stored balance = ${self.balance}"
        )

    async def withdraw_with_lock(self, atm_name: str, amount: int) -> None:
        """
        Correct version.

        The lock makes the check-and-update sequence atomic from the point of
        view of other coroutines using the same lock.
        """
        print(f"{atm_name}: wants to withdraw ${amount}")

        # Work that does not touch shared state can happen outside the lock.
        await asyncio.sleep(0.1)

        async with self.lock:
            print(f"{atm_name}: acquired lock")
            print(f"{atm_name}: current balance = ${self.balance}")

            if self.balance < amount:
                print(f"{atm_name}: rejected, not enough money")
                return

            # This await is here to make the race condition visible.
            #
            # In real code, keep locked sections small.
            # If this represents a database update, a real database transaction
            # would often be the better tool.
            await asyncio.sleep(0.5)

            self.balance -= amount
            self.cash_dispensed += amount

            print(
                f"{atm_name}: approved ${amount}. "
                f"New stored balance = ${self.balance}"
            )


async def demonstrate_problem_without_lock():
    print("\n--- WITHOUT LOCK: broken behavior ---")

    account = BankAccount(balance=100)

    await asyncio.gather(
        account.withdraw_without_lock("ATM-1", 80),
        account.withdraw_without_lock("ATM-2", 80),
    )

    print("\nFinal result without lock:")
    print(f"Stored balance: ${account.balance}")
    print(f"Cash dispensed: ${account.cash_dispensed}")

    expected_balance = 100 - account.cash_dispensed
    print(f"Expected balance based on cash dispensed: ${expected_balance}")

    if account.balance != expected_balance:
        print("BUG: the stored balance does not match the money dispensed!")


async def demonstrate_solution_with_lock():
    print("\n--- WITH LOCK: correct behavior ---")

    account = BankAccount(balance=100)

    await asyncio.gather(
        account.withdraw_with_lock("ATM-1", 80),
        account.withdraw_with_lock("ATM-2", 80),
    )

    print("\nFinal result with lock:")
    print(f"Stored balance: ${account.balance}")
    print(f"Cash dispensed: ${account.cash_dispensed}")

    expected_balance = 100 - account.cash_dispensed
    print(f"Expected balance based on cash dispensed: ${expected_balance}")

    if account.balance == expected_balance:
        print("OK: the account state is consistent.")


async def main():
    await demonstrate_problem_without_lock()
    await demonstrate_solution_with_lock()


if __name__ == "__main__":
    asyncio.run(main())
