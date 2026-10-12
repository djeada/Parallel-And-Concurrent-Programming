"""
Pause and Resume Example with asyncio.Event

Problem:
Imagine an async application is processing many files.

We want to control the background job while it is running:

- press "p" to pause processing
- press "r" to resume processing
- press "q" to quit

The processing task should not be destroyed when paused.
It should simply wait at a safe checkpoint until it is allowed to continue.

Why asyncio.Event is useful:
- Event.set() means "you may continue"
- Event.clear() means "pause at the next checkpoint"
- await Event.wait() pauses the coroutine efficiently
- no busy-looping
- no repeated sleep polling
- no blocking the whole event loop

Important:
Console reads are blocking and cannot be cancelled once they start. A daemon thread
for console input forwards commands to the event loop. Unlike a default-executor
thread, it does not keep asyncio.run() waiting when file processing finishes.
Only console input uses a daemon; all processing tasks are awaited on shutdown.
"""

import asyncio
import os
import sys
import threading
from contextlib import aclosing


def log(message: str) -> None:
    """
    Print a message with the current thread ID.

    The async worker runs on the main event-loop thread.
    Blocking console reads run in a separate thread.
    """
    thread_id = threading.get_ident()
    thread_name = threading.current_thread().name
    print(f"[thread={thread_name}:{thread_id}] {message}")


async def process_file(filename: str) -> None:
    """
    Simulate async file processing.

    In a real app this might be:
    - uploading a file
    - downloading a file
    - processing a message
    - writing to a database
    - sending data to an API
    """
    log(f"Started processing {filename}")

    # Simulate several async steps inside one file.
    for step in range(1, 4):
        log(f"{filename}: processing chunk {step}/3")
        await asyncio.sleep(0.7)

    log(f"Finished processing {filename}")


async def pausable_file_processor(
    filenames: list[str],
    resume_event: asyncio.Event,
    stop_event: asyncio.Event,
) -> None:
    """
    Process files one by one.

    The important line is:

        await resume_event.wait()

    If the event is set, the worker continues.
    If the event is cleared, the worker pauses here without wasting CPU.
    """
    for filename in filenames:
        if stop_event.is_set():
            log("Stop requested. Exiting processor.")
            return

        # Pause checkpoint.
        #
        # If the user pressed "p", resume_event is cleared and this line waits.
        # If the user pressed "r", resume_event is set and this line continues.
        await resume_event.wait()

        # Quitting also wakes a paused worker. Do not start another file then.
        if stop_event.is_set():
            return

        await process_file(filename)

    log("All files processed.")


async def console_commands():
    """Bridge blocking console input to this event loop for one program run.

    Cancellation stops delivery, but cannot interrupt a read. The disposable
    daemon reader may remain blocked until input arrives or the process exits.
    """
    loop = asyncio.get_running_loop()
    commands = asyncio.Queue()
    closed = threading.Event()

    def forward(command):
        if closed.is_set():
            return False
        try:
            # asyncio.Queue is not thread-safe; enqueue on the event loop.
            loop.call_soon_threadsafe(commands.put_nowait, command)
        except RuntimeError:
            return False  # The event loop has already closed.
        return command is not None

    def read_commands():
        try:
            # This CLI owns stdin. A separate, unbuffered descriptor avoids
            # holding sys.stdin's buffer lock during interpreter shutdown.
            with os.fdopen(os.dup(sys.stdin.fileno()), "rb", buffering=0) as stream:
                while not closed.is_set():
                    line = stream.readline()
                    command = line.decode(sys.stdin.encoding or "utf-8") if line else None
                    if not forward(command):
                        return
        except Exception as exc:
            forward(exc)

    threading.Thread(target=read_commands, name="console-input", daemon=True).start()
    try:
        while True:
            print("\nCommand: [p]ause, [r]esume, [q]uit: ", end="", flush=True)
            command = await commands.get()
            if command is None:
                return
            if isinstance(command, Exception):
                raise command
            yield command
    finally:
        closed.set()


async def command_listener(
    resume_event: asyncio.Event,
    stop_event: asyncio.Event,
) -> None:
    """
    Listen for user commands without blocking the event loop.

    End of input requests shutdown just like the quit command.
    """
    async with aclosing(console_commands()) as commands:
        async for command in commands:
            if stop_event.is_set():
                return
            command = command.strip().lower()

            if command == "p":
                resume_event.clear()
                log("Paused. Processing will stop at the next checkpoint.")
            elif command == "r":
                resume_event.set()
                log("Resumed.")
            elif command == "q":
                stop_event.set()
                resume_event.set()
                log("Quit requested.")
                return
            else:
                log("Unknown command. Use p, r, or q.")

    stop_event.set()
    resume_event.set()
    log("End of input. Exiting.")


async def main() -> None:
    filenames = [
        "users.csv",
        "orders.csv",
        "payments.csv",
        "products.csv",
        "events.csv",
    ]

    resume_event = asyncio.Event()
    stop_event = asyncio.Event()

    # Start in the resumed state.
    resume_event.set()

    processor_task = asyncio.create_task(
        pausable_file_processor(
            filenames,
            resume_event,
            stop_event,
        )
    )

    command_task = asyncio.create_task(
        command_listener(
            resume_event,
            stop_event,
        )
    )

    tasks = {processor_task, command_task}
    try:
        done, _ = await asyncio.wait(tasks, return_when=asyncio.FIRST_COMPLETED)
        for task in done:
            task.result()  # Propagate failures instead of silently discarding them.
    finally:
        stop_event.set()
        resume_event.set()
        for task in tasks:
            if not task.done():
                task.cancel()
        await asyncio.gather(*tasks, return_exceptions=True)

    log("Program finished.")


# ---------------------------------------------------------------------
# What goes wrong without asyncio.Event?
# ---------------------------------------------------------------------

async def bad_pause_with_boolean_flag() -> None:
    """
    BAD APPROACH:

    A simple boolean flag is easy to get wrong.

    Problems:
    - the task may poll repeatedly
    - it may waste CPU
    - it needs artificial sleep calls
    - many tasks may check the flag inconsistently
    - there is no clean "wait until resumed" primitive

    This is commented out because it demonstrates what NOT to do.
    """

    # paused = False
    #
    # while True:
    #     if paused:
    #         # This is manual polling.
    #         # The task wakes up again and again just to ask:
    #         # "Am I still paused?"
    #         await asyncio.sleep(0.1)
    #         continue
    #
    #     await do_some_work()


async def bad_blocking_input_inside_async() -> None:
    """
    BAD APPROACH:

    Calling input() directly inside async code blocks the event loop.

    While input() is waiting for the user, no other coroutine can run.
    That means your background job would freeze completely.
    """

    # command = input("Enter command: ")
    #
    # Problem:
    # This blocks the whole event-loop thread.
    # Other async tasks cannot continue while input() waits.


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nStopped")
