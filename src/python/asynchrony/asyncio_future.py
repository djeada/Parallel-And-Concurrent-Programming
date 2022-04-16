import asyncio
import sys


async def sum_n_numbers(future, n):
    result = 0
    for i in range(n):
        result += i + 1
    future.set_result(f"Sum of n integers result = {result}")


async def second_coroutine(future, n):
    result = 1
    for i in range(n - 1):
        result *= i + 2
    future.set_result(f"{n}! = {result}")


if __name__ == "__main__":

    n = 5

    loop = asyncio.get_event_loop()
    future1 = asyncio.Future()
    future2 = asyncio.Future()

    tasks = (sum_n_numbers(future1, n), second_coroutine(future2, n))

    loop.run_until_complete(asyncio.wait(tasks))

    future1.result()
    future2.result()

    loop.close()
