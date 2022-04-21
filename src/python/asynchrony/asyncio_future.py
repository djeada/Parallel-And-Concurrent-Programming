import asyncio
import sys

# coroutine that sets the result directly in the future object
async def sum_n_numbers(future, n):
    result = 0
    for i in range(n):
        result += i + 1
    future.set_result(f"Sum of n integers result = {result}")


# coroutine that returns the result like normal function
async def second_coroutine(n):
    result = 1
    for i in range(n - 1):
        result *= i + 2

    return f"{n}! = {result}"


def display_result(future):
    print(future.result())


if __name__ == "__main__":

    n = 5

    loop = asyncio.get_event_loop()
    future = asyncio.Future()

    # create tasks from coroutines
    task1 = loop.create_task(sum_n_numbers(future, n))
    task2 = loop.create_task(second_coroutine(n))

    # callback will be called when the task is completed
    # task1.add_done_callback(display_result)
    task2.add_done_callback(display_result)

    loop.run_until_complete(asyncio.wait((task1, task2)))

    display_result(future)

    loop.close()
