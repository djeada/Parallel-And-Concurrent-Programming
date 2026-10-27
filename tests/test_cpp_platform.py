"""C++20 process, MPI, and HTTP checks; optional toolchains skip explicitly."""

import concurrent.futures
import http.server
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import subprocess
import tempfile
import threading
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]


class Examples(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory(prefix="cpp-platform-")
        cls.addClassCleanup(cls.directory.cleanup)
        cls.binaries = {}

    @classmethod
    def compile(cls, source, compiler=("g++",), flags=(), libraries=()):
        name = Path(source).stem
        binary = Path(cls.directory.name) / name
        subprocess.run(
            [*compiler, "-std=c++20", "-pthread", "-O1", "-Wall", "-Wextra",
             *flags, str(ROOT / "src/cpp" / source), "-o", str(binary), *libraries],
            capture_output=True, text=True, check=True,
        )
        cls.binaries[name] = binary

    def run_example(self, name, arguments=(), prefix=(), timeout=8, success=True):
        process = subprocess.Popen(
            [*prefix, str(self.binaries[name]), *map(str, arguments)],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, start_new_session=True,
        )
        try:
            output, errors = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate()
            self.fail(f"{name} timed out")
        if success:
            self.assertEqual(process.returncode, 0, errors)
        else:
            self.assertNotEqual(process.returncode, 0)
        return output, errors


class ProcessExamples(Examples):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        for source in (
            "basic_process", "multiple_processes", "pipe_communication", "spawn_process",
            "exec_process", "shared_memory", "process_mutex", "process_semaphore",
            "process_barrier", "process_pool", "process_manager", "named_pipe",
        ):
            cls.compile(f"multiprocessing/{source}.cpp")
        cls.compile("asynchrony/async_producer_consumer.cpp")
        harness = Path(cls.directory.name) / "pool_wide_result.cpp"
        harness.write_text(
            '#define main example_main\n'
            f'#include "{ROOT / "src/cpp/multiprocessing/process_pool.cpp"}"\n'
            '#undef main\n#include <limits>\n'
            'int main() { ProcessPool pool(2); '
            'pool.submit(std::numeric_limits<int>::max()); }\n'
        )
        cls.compile(harness)

    def test_children_complete_before_parent(self):
        output, _ = self.run_example("basic_process")
        self.assertLess(output.index("Child process finished"), output.index("Parent process finished"))
        output, _ = self.run_example("multiple_processes")
        self.assertIn("Task A finished", output)
        self.assertIn("Task B finished", output)

    def test_pipe_delivers_all_records(self):
        output, _ = self.run_example("pipe_communication")
        self.assertEqual(re.findall(r"Consumer: received (\d+)", output), ["1", "2", "3", "4", "5"])

    def test_spawn_and_exec_report_success_and_missing_program(self):
        for example, message in (("spawn_process", "Hello from posix_spawn"), ("exec_process", "Hello from fork/exec")):
            with self.subTest(example=example):
                output, _ = self.run_example(example)
                self.assertIn(message, output)
                self.assertIn("Child exited successfully", output)
                self.run_example(example, ["/nonexistent/cpp-example-program"], success=False)

    def test_shared_resources_are_isolated_between_concurrent_runs(self):
        for example, expected in (("shared_memory", "Final value: 0"), ("process_mutex", "Final counter: 4000")):
            with self.subTest(example=example), concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
                results = list(pool.map(lambda _: self.run_example(example), range(3)))
                for output, _ in results:
                    self.assertIn(expected, output)

    def test_platform_synchronizers_complete(self):
        output, _ = self.run_example("process_barrier")
        self.assertEqual(output.count("passed barrier"), 4)
        output, _ = self.run_example("process_semaphore")
        self.assertEqual(output.count("acquired resource"), 10)
        self.assertEqual(output.count("releasing resource"), 10)

    def test_process_pool_reuses_workers_and_returns_results(self):
        output, _ = self.run_example("process_pool")
        workers = re.findall(r"starting \(PID: (\d+)\)", output)
        self.assertEqual(len(workers), 10)
        self.assertEqual(len(set(workers)), 3)
        self.assertEqual(sorted(map(int, re.findall(r"Task result: (\d+)", output))), list(range(0, 20, 2)))

    def test_fifo_and_manager_runs_do_not_share_paths(self):
        for example in ("named_pipe", "process_manager"):
            with self.subTest(example=example), concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
                results = list(pool.map(lambda _: self.run_example(example), range(3)))
                for output, _ in results:
                    if example == "named_pipe":
                        self.assertIn("FIFO demo complete", output)
                        for index in range(1, 6):
                            self.assertIn(f"Message {index}", output)
                    else:
                        self.assertEqual(output.count(") completed"), 5)

    def test_pool_scope_exit_drains_and_preserves_full_width_result(self):
        output, _ = self.run_example("pool_wide_result")
        self.assertIn("Task result: 4294967294", output)

    def test_future_queue_drains_without_precomputed_consumer_counts(self):
        output, _ = self.run_example("async_producer_consumer")
        values = re.findall(r"Consumer \d+: (P\d+-Item\d+)", output)
        self.assertEqual(sorted(values), [f"P{p}-Item{i}" for p in range(3) for i in range(4)])
        self.assertEqual(sum(map(int, re.findall(r"consumed (\d+) items", output))), 12)


class MpiExamples(Examples):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        compiler = shlex.split(os.environ.get("MPICXX", "mpicxx"))
        cls.launcher = shlex.split(os.environ.get("MPIEXEC", "mpiexec"))
        if not shutil.which(compiler[0]) or not shutil.which(cls.launcher[0]):
            raise unittest.SkipTest("MPI C++ wrapper and runtime are not installed")
        for source in sorted((ROOT / "src/cpp/mpi").glob("*.cpp")):
            cls.compile(f"mpi/{source.name}", compiler=compiler)

    def test_all_mpi_examples_on_multiple_ranks(self):
        for name in self.binaries:
            with self.subTest(example=name):
                self.run_example(name, prefix=[*self.launcher, "-n", "2"], timeout=15)

    def test_matrix_handles_uneven_and_empty_partitions(self):
        for ranks in (1, 3, 6):
            with self.subTest(ranks=ranks):
                output, _ = self.run_example("parallel_matrix_multiplication", prefix=[*self.launcher, "-n", str(ranks)], timeout=15)
                self.assertIn("Matches sequential reference: true", output)

    def test_single_rank_ring_and_missing_peer(self):
        output, _ = self.run_example("ring_topology", prefix=[*self.launcher, "-n", "1"], timeout=15)
        self.assertIn("Token returned with value 42", output)
        _, errors = self.run_example("non_blocking_communication", prefix=[*self.launcher, "-n", "1"], timeout=15, success=False)
        self.assertIn("requires at least 2 processes", errors)


class HttpExamples(Examples):
    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        flags = shlex.split(os.environ.get("CURL_CXXFLAGS", ""))
        libraries = shlex.split(os.environ.get("CURL_LDFLAGS", "-lcurl"))
        probe = subprocess.run(
            ["g++", "-std=c++20", *flags, "-x", "c++", "-", "-x", "none", "-o", str(Path(cls.directory.name) / "probe"), *libraries],
            input="#include <curl/curl.h>\nint main() { return curl_multi_init() == nullptr; }\n",
            capture_output=True, text=True, timeout=15,
        )
        if probe.returncode:
            raise unittest.SkipTest("libcurl development headers/library are not installed")
        cls.compile("asynchrony/parallel_fetch.cpp", flags=flags, libraries=libraries)
        cls.compile("multithreading/download_images.cpp", flags=flags, libraries=libraries)

    def test_local_http_success_failure_and_concurrency_limit(self):
        lock = threading.Lock()
        state = {"active": 0, "peak": 0}

        class Handler(http.server.BaseHTTPRequestHandler):
            def do_GET(self):
                with lock:
                    state["active"] += 1
                    state["peak"] = max(state["peak"], state["active"])
                try:
                    time.sleep(0.03)
                    body = b"example response"
                    self.send_response(404 if self.path == "/missing" else 200)
                    self.send_header("Content-Length", str(len(body)))
                    self.end_headers()
                    self.wfile.write(body)
                finally:
                    with lock:
                        state["active"] -= 1

            def log_message(self, *_):
                pass

        server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        thread = threading.Thread(target=server.serve_forever)
        thread.start()
        try:
            base = f"http://127.0.0.1:{server.server_port}"
            urls = [f"{base}/item/{index}" for index in range(7)]
            output, _ = self.run_example("parallel_fetch", urls)
            self.assertEqual(output.count("Fetched:"), len(urls))
            self.assertGreater(state["peak"], 1)
            self.assertLessEqual(state["peak"], 4)
            output, errors = self.run_example("parallel_fetch", [urls[0], base + "/missing"], success=False)
            self.assertIn("Fetched:", output)
            self.assertIn("Failed:", errors)
            output, _ = self.run_example("download_images", urls[:2])
            self.assertIn("Sequential: 32 bytes", output)
            self.assertIn("Asynchronous: 32 bytes", output)
        finally:
            server.shutdown()
            server.server_close()
            thread.join()


if __name__ == "__main__":
    unittest.main()
