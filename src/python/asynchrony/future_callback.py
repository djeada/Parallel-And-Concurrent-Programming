"""
Task Done Callbacks: Progress Reporting Without Mixing Concerns

Problem:
Imagine we start several async downloads.

We want three things:

1. Run the downloads concurrently
2. Print progress as each download finishes
3. Keep the download function focused only on downloading

Without callbacks:
- we usually await all tasks with gather()
- then we inspect the results after everything is done
- progress reporting happens too late, only at the end

With add_done_callback():
- each task notifies us when it finishes
- the callback can log success, failure, or cancellation
- the worker coroutine does not need to know about progress reporting

Important:
A Task is a Future-like object.

asyncio.create_task() returns a Task.
A Task:
- runs a coroutine
- eventually stores a result, exception, or cancellation
- supports add_done_callback()

Callbacks are useful for:
- logging when background tasks finish
- updating progress counters
- collecting metrics
- notifying another part of the program
- integrating with older callback-style APIs

But:
For normal application flow, async/await is usually cleaner.
Callbacks are best when you want to observe task completion from the side.
"""

import asyncio
import threading
import time


START_TIME = time.perf_counter()


def log(message: str) -> None:
    """
    Print time, thread, and current task information.

    Done callbacks run on the event loop thread, but they are not themselves
    async tasks. That is why current_task() may be None inside the callback.
    """
    elapsed = time.perf_counter() - START_TIME
    thread_id = threading.get_ident()
    thread_name = threading.current_thread().name

    try:
        current_task = asyncio.current_task()
        current_task_name = current_task.get_name() if current_task else "no-task"
    except RuntimeError:
        current_task_name = "no-event-loop"

    print(
        f"{elapsed:6.2f}s | "
        f"thread={thread_name}:{thread_id} | "
        f"current={current_task_name:<15} | "
        f"{message}"
    )


async def download_file(
    filename: str,
    delay: float,
    should_fail: bool = False,
) -> str:
    """
    Simulate an async download.

    This function only knows how to do the work.
    It does not know anything about progress bars, logging callbacks,
    metrics, or notifications.
    """
    log(f"{filename}: download started")

    await asyncio.sleep(delay)

    if should_fail:
        raise RuntimeError(f"{filename}: server returned 500")

    log(f"{filename}: download finished")

    return f"{filename} downloaded successfully"


def make_progress_callback(progress: dict[str, int]):
    """
    Create a callback that observes task completion.

    The callback receives the completed Future/Task as its argument.

    Important:
    task.result() returns the task result if it succeeded.
    task.result() re-raises the exception if the task failed.
    task.result() raises CancelledError if the task was cancelled.
    """

    def on_task_done(task: asyncio.Task) -> None:
        progress["finished"] += 1

        task_name = task.get_name()

        try:
            result = task.result()

        except asyncio.CancelledError:
            progress["cancelled"] += 1
            log(f"CALLBACK: {task_name} was cancelled")

        except Exception as exc:
            progress["failed"] += 1
            log(f"CALLBACK: {task_name} failed: {exc}")

        else:
            progress["succeeded"] += 1
            log(f"CALLBACK: {task_name} succeeded: {result}")

        finally:
            log(
                "CALLBACK: progress "
                f"{progress['finished']}/{progress['total']} finished, "
                f"{progress['succeeded']} succeeded, "
                f"{progress['failed']} failed, "
                f"{progress['cancelled']} cancelled"
            )

    return on_task_done


async def heartbeat() -> None:
    """
    Show that the event loop is still alive while downloads are running.
    """
    for i in range(6):
        log(f"heartbeat {i}")
        await asyncio.sleep(0.5)


async def main() -> None:
    log("program started")

    downloads = [
        ("users.csv", 2.0, False),
        ("orders.csv", 1.0, False),
        ("payments.csv", 1.5, True),
        ("products.csv", 3.0, False),
    ]

    progress = {
        "total": len(downloads),
        "finished": 0,
        "succeeded": 0,
        "failed": 0,
        "cancelled": 0,
    }

    callback = make_progress_callback(progress)

    tasks = []

    for filename, delay, should_fail in downloads:
        task = asyncio.create_task(
            download_file(filename, delay, should_fail),
            name=f"download:{filename}",
        )

        # Register a callback.
        #
        # This does not await the task.
        # It only says:
        #
        #   "When this task is done, call this function."
        task.add_done_callback(callback)

        tasks.append(task)

    log("all downloads started")

    # The main coroutine can do other async work while downloads run.
    heartbeat_task = asyncio.create_task(
        heartbeat(),
        name="heartbeat",
    )

    # We still await the tasks eventually.
    #
    # The callbacks are for observing completion.
    # gather() is for making sure main() waits for everything before exiting.
    results = await asyncio.gather(
        *tasks,
        return_exceptions=True,
    )

    await heartbeat_task

    log(f"main received final results: {results}")
    log("program finished")


# ---------------------------------------------------------------------
# What goes wrong without callbacks?
# ---------------------------------------------------------------------

async def bad_no_progress_until_the_end() -> None:
    """
    BAD FOR PROGRESS REPORTING:

    This waits for all downloads to finish before printing anything.

    The work is concurrent, but result handling is not event-driven.
    You only learn what happened after everything is done.
    """

    # tasks = [
    #     download_file("users.csv", 2.0),
    #     download_file("orders.csv", 1.0),
    #     download_file("products.csv", 3.0),
    # ]
    #
    # results = await asyncio.gather(*tasks)
    #
    # for result in results:
    #     print(result)


async def bad_async_callback() -> None:
    """
    BAD CALLBACK:

    add_done_callback() expects a normal function.

    Do not pass an async function directly as a done callback.
    The event loop will call it like a regular function, which creates a
    coroutine object but does not await it.
    """

    # async def on_done(task):
    #     await asyncio.sleep(1)
    #     print(task.result())
    #
    # task = asyncio.create_task(download_file("users.csv", 1.0))
    #
    # BAD:
    # task.add_done_callback(on_done)


async def better_async_work_from_callback() -> None:
    """
    BETTER:

    If a callback needs to trigger async work, schedule another task from
    inside the normal callback.
    """

    # async def send_notification(result):
    #     await asyncio.sleep(1)
    #     print(f"Notification sent: {result}")
    #
    # def on_done(task):
    #     try:
    #         result = task.result()
    #     except Exception as exc:
    #         print(f"Task failed: {exc}")
    #     else:
    #         asyncio.create_task(send_notification(result))
    #
    # task = asyncio.create_task(download_file("users.csv", 1.0))
    # task.add_done_callback(on_done)


if __name__ == "__main__":
    asyncio.run(main())
