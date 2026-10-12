"""Concurrency regression checks. Run: python3 -m unittest discover -s tests -v"""

import concurrent.futures
import os
from pathlib import Path
import signal
import socket
import subprocess
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]


class CppRegressions(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory(prefix="cpp-regressions-")
        cls.addClassCleanup(cls.directory.cleanup)
        cls.binaries = {}
        examples = {
            "latch": "multithreading/latch.cpp",
            "try_lock": "multithreading/try_lock.cpp",
            "pause_resume": "asynchrony/pause_resume.cpp",
            "heavy_functions": "asynchrony/heavy_functions.cpp",
            "signals": "multiprocessing/signals.cpp",
            "message_queue": "multiprocessing/message_queue.cpp",
            "process_barrier": "multiprocessing/process_barrier.cpp",
            "process_pool": "multiprocessing/process_pool.cpp",
            "distributed": "asynchrony/distributed_computing.cpp",
        }
        for name, source in examples.items():
            binary = Path(cls.directory.name) / name
            flags = ["-fsanitize=address", "-g"] if name == "distributed" else []
            subprocess.run(
                ["g++", "-std=c++20", "-pthread", *flags,
                 str(ROOT / "src/cpp" / source), "-o", str(binary)],
                check=True, capture_output=True, text=True,
            )
            cls.binaries[name] = binary

    def run_example(self, name, input_text="", timeout=8):
        process = subprocess.Popen(
            [str(self.binaries[name])], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, start_new_session=True,
        )
        try:
            stdout, stderr = process.communicate(input_text, timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate()
            self.fail(f"{name} timed out")
        self.assertEqual(process.returncode, 0, stderr)
        return stdout

    def test_latch_releases_after_all_producers(self):
        self.assertIn("Final counter: 512", self.run_example("latch"))

    def test_try_lock_finishes(self):
        self.assertIn("Final count:", self.run_example("try_lock"))

    def test_pause_resume_stops_on_quit_and_eof(self):
        for commands in ("", "q\n", "p\n", "p\nq\n", "p\np\nq\n"):
            with self.subTest(commands=commands):
                self.assertIn("Task completed", self.run_example("pause_resume", commands, 2))

    def test_heavy_functions_finishes_with_idle_or_partial_input(self):
        for partial_input in (b"", b"unfinished line"):
            with self.subTest(input=partial_input):
                process = subprocess.Popen(
                    [str(self.binaries["heavy_functions"])], stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                )
                try:
                    process.stdin.write(partial_input)
                    process.stdin.flush()
                    # Keep the pipe open: communicate() would close stdin and
                    # conceal the original uncancellable-getline defect.
                    process.wait(timeout=6)
                    self.assertEqual(process.returncode, 0)
                    self.assertIn(b"0 10 20 30", process.stdout.read())
                finally:
                    if process.poll() is None:
                        process.kill()
                    process.wait()
                    process.stdin.close()
                    process.stdout.close()
                    process.stderr.close()

    def test_signals_reap_children_and_restore_handlers(self):
        for _ in range(20):
            output = self.run_example("signals", timeout=2)
            self.assertIn("all 4 children reaped", output)
            self.assertIn("Child: received SIGUSR1", output)
            self.assertEqual(output.count("child exited, status=0"), 2)

    def test_message_queue_startup_and_concurrent_runs(self):
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
            outputs = list(executor.map(lambda _: self.run_example("message_queue"), range(4)))
        for output in outputs:
            for item in range(1, 6):
                self.assertIn(f"Consumer: received {item}", output)

    def test_child_output_survives_exit(self):
        barrier = self.run_example("process_barrier")
        self.assertEqual(barrier.count("passed barrier"), 4)
        pool = self.run_example("process_pool")
        self.assertEqual(pool.count("starting (PID:"), 10)
        self.assertEqual(pool.count("finished\n"), 10)
        self.assertEqual(pool.count("Task result:"), 10)

    def test_full_socket_buffer_under_address_sanitizer(self):
        with socket.socket() as reservation:
            reservation.bind(("127.0.0.1", 0))
            port = reservation.getsockname()[1]
        process = subprocess.Popen(
            [str(self.binaries["distributed"]), "server", str(port)],
            stdout=subprocess.DEVNULL, stderr=subprocess.PIPE,
            start_new_session=True,
        )
        try:
            deadline = time.monotonic() + 3
            while True:
                try:
                    connection = socket.create_connection(("127.0.0.1", port), timeout=1)
                    break
                except OSError:
                    if process.poll() is not None or time.monotonic() >= deadline:
                        self.fail("Socket server did not start")
                    time.sleep(0.02)
            with connection:
                payload = b"A" * 1024
                connection.sendall(payload)
                received = b""
                while len(received) < len(payload):
                    chunk = connection.recv(1024)
                    self.assertTrue(chunk, "Connection closed before echo completed")
                    received += chunk
                self.assertEqual(received, payload)
        finally:
            if process.poll() is None:
                os.killpg(process.pid, signal.SIGTERM)
            _, stderr = process.communicate(timeout=3)
        self.assertNotIn(b"ERROR: AddressSanitizer", stderr)

    def test_socket_client_handles_empty_tasks_and_fragmented_replies(self):
        task = "A" * 4096
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            listener.listen()
            listener.settimeout(3)
            port = listener.getsockname()[1]

            def echo():
                with listener.accept()[0] as connection:
                    connection.settimeout(3)
                    # A blank input line must not block waiting for an echo.
                    received = b""
                    while len(received) < len(task):
                        chunk = connection.recv(4096)
                        if not chunk:
                            return
                        received += chunk
                    self.assertEqual(received, task.encode())
                    # Force multiple reads rather than relying on TCP timing.
                    for offset in range(0, len(received), 256):
                        connection.sendall(received[offset:offset + 256])
                        time.sleep(0.002)

            with concurrent.futures.ThreadPoolExecutor(max_workers=1) as executor:
                server = executor.submit(echo)
                result = subprocess.run(
                    [str(self.binaries["distributed"]), "client", str(port)],
                    input="\n" + task + "\n", capture_output=True, text=True, timeout=5,
                )
                server.result(timeout=4)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("Result: " + task + "\n", result.stdout)

    def test_socket_client_reports_a_truncated_reply(self):
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            listener.listen()
            listener.settimeout(3)
            port = listener.getsockname()[1]

            def echo_part():
                with listener.accept()[0] as connection:
                    connection.settimeout(3)
                    self.assertTrue(connection.recv(1024))
                    connection.sendall(b"ta")

            with concurrent.futures.ThreadPoolExecutor(max_workers=1) as executor:
                server = executor.submit(echo_part)
                result = subprocess.run(
                    [str(self.binaries["distributed"]), "client", str(port)],
                    input="task\n", capture_output=True, text=True, timeout=5,
                )
                server.result(timeout=4)
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn("Connection closed before the full reply", result.stderr)


if __name__ == "__main__":
    unittest.main()
