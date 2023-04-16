import asyncio


async def foo():
    for i in range(5):
        print("foo")
        await asyncio.sleep(1)


async def bar():
    for i in range(10):
        print("bar")
        await asyncio.sleep(1)


loop = asyncio.get_event_loop()

# The event loop is executed until the task is completed.
loop.run_until_complete(foo())
loop.run_until_complete(bar())

loop.close()
