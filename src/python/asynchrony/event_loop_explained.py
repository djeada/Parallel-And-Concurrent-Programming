"""
event_loop_explained.py
=======================

A runnable lesson about Python's asyncio event loop.

What this script teaches
------------------------
1. An event loop runs coroutine tasks and lets other tasks progress whenever
   one task reaches an ``await`` point.
2. Modern application entry point:
       asyncio.run(main())
   Inside a coroutine or callback, obtain the active loop with:
       asyncio.get_running_loop()
3. Older pattern to avoid for application startup:
       loop = asyncio.get_event_loop()
       loop.run_until_complete(main())
   Relying on an implicitly supplied "current loop" is no longer reliable:
   Python 3.14 raises RuntimeError when no current loop has been set.
4. Precise terminology:
   ``new_event_loop()`` and ``run_until_complete()`` are still available
   low-level APIs. The deprecated part is the asyncio *policy system* and
   code that depends on policy-driven implicit loop creation.
5. When multiple top-level async calls must share one managed loop, use:
       with asyncio.Runner() as runner:
           runner.run(first())
           runner.run(second())

Recommended Python version: 3.11 or newer.
"""

from __future__ import annotations

import asyncio
import sys
import warnings


LINE = "=" * 74


def heading(title: str) -> None:
    """Print a visible section heading."""
    print(f"\n{LINE}\n{title}\n{LINE}")


def elapsed(loop: asyncio.AbstractEventLoop, started: float) -> str:
    """Return a timestamp relative to the beginning of a demonstration."""
    return f"{loop.time() - started:0.2f}s"


async def pretend_io(
    name: str,
    delay: float,
    loop: asyncio.AbstractEventLoop,
    started: float,
) -> str:
    """
    Simulate an I/O operation.

    asyncio.sleep() stands in for real waiting, such as a response from a
    website, database, or file/socket operation. At ``await``, this task
    pauses and the event loop can run another ready task.
    """
    print(f"[{elapsed(loop, started)}] {name}: started; now awaiting I/O")
    await asyncio.sleep(delay)
    print(f"[{elapsed(loop, started)}] {name}: resumed after I/O was ready")
    return f"{name} result"


async def event_loop_lesson(label: str) -> None:
    """Run concurrent tasks and explain what the event loop is doing."""
    loop = asyncio.get_running_loop()
    started = loop.time()

    print(f"\nRunning lesson via {label}")
    print(f"Inside async code, get_running_loop() returned: {type(loop).__name__}")
    print("One event loop cooperatively schedules these three Tasks.")
    print("Watch them all start before the first one finishes:\n")

    async with asyncio.TaskGroup() as group:
        task_a = group.create_task(pretend_io("request A", 0.30, loop, started))
        task_b = group.create_task(pretend_io("request B", 0.10, loop, started))
        task_c = group.create_task(pretend_io("request C", 0.20, loop, started))

    print("\nAll results:", [task_a.result(), task_b.result(), task_c.result()])
    print(
        "Meaning: each await yielded control; while one task waited, "
        "the same loop advanced another."
    )


def show_old_implicit_loop_pattern() -> None:
    """Explain and safely probe the old policy-dependent pattern."""
    heading("1. THE OLD IMPLICIT-LOOP PATTERN (DO NOT USE FOR NEW APPS)")
    print(
        """A common older program entry point looked like this:

    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(main())
    finally:
        loop.close()

The risky part is asking for a loop outside running async code and expecting
asyncio's policy system to silently supply one."""
    )

    print(f"\nSafe probe on this interpreter: Python {sys.version.split()[0]}")
    with warnings.catch_warnings(record=True) as caught:
        warnings.simplefilter("always", DeprecationWarning)
        try:
            discovered_loop = asyncio.get_event_loop()
        except RuntimeError as error:
            print("asyncio.get_event_loop() outside async code raised RuntimeError.")
            print("This is the Python 3.14+ behavior when no current loop is set.")
            print(f"Message: {error}")
        else:
            print(
                "This Python version returned a loop outside async code. "
                "New code should not depend on that behavior."
            )
            if not discovered_loop.is_running():
                discovered_loop.close()
                asyncio.set_event_loop(None)

    for warning in caught:
        print(f"Deprecation warning emitted: {warning.message}")

    print(
        "\nNote: loop.run_until_complete() itself remains a low-level API; "
        "policy-based implicit loop discovery is the deprecated direction."
    )


def run_modern_entry_point() -> None:
    """Demonstrate the standard modern entry point."""
    heading("2. THE MODERN DEFAULT: asyncio.run(main())")
    print(
        """Recommended application structure:

    async def main():
        loop = asyncio.get_running_loop()  # only if the loop object is needed
        ...

    asyncio.run(main())

asyncio.run() creates, manages, and closes the event loop for this top-level
async program."""
    )
    asyncio.run(event_loop_lesson("asyncio.run(...)"))


async def identify_runner_loop(phase: str) -> int:
    """Show which loop a Runner-managed phase uses."""
    loop = asyncio.get_running_loop()
    print(f"{phase}: active loop identity is {id(loop)}")
    await asyncio.sleep(0.05)
    return id(loop)


def run_runner_example() -> None:
    """Show the managed option for multiple top-level async calls."""
    heading("3. WHEN ONE MANAGED LOOP MUST RUN MULTIPLE TOP-LEVEL CALLS")
    print(
        """Use asyncio.Runner instead of manually opening and closing a loop:

    with asyncio.Runner() as runner:
        runner.run(first_phase())
        runner.run(second_phase())
"""
    )
    with asyncio.Runner() as runner:
        first_loop_id = runner.run(identify_runner_loop("first phase "))
        second_loop_id = runner.run(identify_runner_loop("second phase"))

    print(f"Same loop reused inside Runner: {first_loop_id == second_loop_id}")


def run_low_level_comparison() -> None:
    """Run a low-level equivalent solely for comparison."""
    heading("4. LOW-LEVEL MANUAL CONTROL: SOMETIMES VALID, USUALLY UNNECESSARY")
    print(
        """Framework/library authors may deliberately control a loop:

    loop = asyncio.new_event_loop()
    try:
        asyncio.set_event_loop(loop)
        loop.run_until_complete(main())
    finally:
        asyncio.set_event_loop(None)
        loop.close()

This is more responsibility than most applications need. It is shown for
comparison, not as the preferred application entry point."""
    )

    loop = asyncio.new_event_loop()
    try:
        asyncio.set_event_loop(loop)
        loop.run_until_complete(event_loop_lesson("manual low-level loop"))
    finally:
        asyncio.set_event_loop(None)
        loop.close()


def print_takeaways() -> None:
    """Finish with a compact summary."""
    heading("TAKEAWAYS")
    print(
        """• The event loop is the scheduler at the heart of asyncio.
• await pauses one task so the loop can progress other ready work.
• New application entry points should normally use asyncio.run(main()).
• Inside async code, prefer asyncio.get_running_loop() when you need the loop.
• Use asyncio.Runner when several top-level async calls should share one loop.
• Avoid policy-dependent implicit-loop startup patterns in new code."""
    )


def main() -> None:
    """Execute the complete educational walkthrough."""
    heading("PYTHON ASYNCIO: UNDERSTANDING THE EVENT LOOP")
    print(
        "This demonstration compares an old implicit-loop habit with modern "
        "managed asyncio entry points."
    )
    show_old_implicit_loop_pattern()
    run_modern_entry_point()
    run_runner_example()
    run_low_level_comparison()
    print_takeaways()


if __name__ == "__main__":
    main()
