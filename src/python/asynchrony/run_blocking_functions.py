import asyncio
import concurrent.futures
import time
import numpy as np

def blocking_function(i):
    print(f'running blocking function {i}')
    a = np.random.rand(1000, 1000)
    b = np.random.rand(1000, 1000)
    c = np.dot(a, b)
    print(f'done running blocking function {i}')
    return i

async def run_blocking_functions(executor, callback):
    print('start run_blocking_functions')

    loop = asyncio.get_event_loop()
    blocking_tasks = [
        loop.run_in_executor(executor, callback, i)
        for i in range(6)
    ]
    print('waiting for executor tasks')
    completed, pending = await asyncio.wait(blocking_tasks)
    results = [task.result() for task in completed]
    print(f'results: {results}')
    print('end run_blocking_functions')

def main():
  print('START main')
  executor = concurrent.futures.ProcessPoolExecutor(max_workers=3)
  
  event_loop = asyncio.get_event_loop()
  event_loop.run_until_complete(
      run_blocking_functions(executor, blocking_function)
  )
  event_loop.close()
  print("STOP main")

main()
