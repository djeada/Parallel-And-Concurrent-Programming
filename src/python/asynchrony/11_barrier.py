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

try:
    asyncio.ensure_future(foo())
    asyncio.ensure_future(bar())
    loop.run_forever()
except KeyboardInterrupt:
    # use Ctrl+C to close the program
    pass
finally:
    loop.close()

print("The End")
