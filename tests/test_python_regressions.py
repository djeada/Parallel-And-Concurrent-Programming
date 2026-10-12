"""Python example regressions; no external services or third-party packages needed."""

import asyncio
from contextlib import redirect_stdout
import http.server
import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import threading
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]


def load_example(name):
    path = ROOT / "src/python/asynchrony" / f"{name}.py"
    spec = importlib.util.spec_from_file_location(f"example_{name}", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


pause = load_example("pause_resume")
queue_demo = load_example("data_sharing_queue")
pipeline = load_example("producer_consumer")
dashboard = load_example("future_create_task")
fetch = load_example("fetch_parallel")


class PythonAsyncRegressions(unittest.IsolatedAsyncioTestCase):
    async def test_quit_while_paused_does_not_start_another_file(self):
        resume, stop = asyncio.Event(), asyncio.Event()
        processed = []

        async def process_file(name):
            processed.append(name)

        with patch.object(pause, "process_file", process_file), redirect_stdout(io.StringIO()):
            worker = asyncio.create_task(pause.pausable_file_processor(["file"], resume, stop))
            await asyncio.sleep(0)
            stop.set()
            resume.set()
            await asyncio.wait_for(worker, 1)
        self.assertEqual(processed, [])

    async def test_pause_main_propagates_processor_failure(self):
        async def fail(_):
            raise RuntimeError("processor failed")

        async def listen(*_):
            await asyncio.Event().wait()

        with patch.object(pause, "process_file", fail), patch.object(pause, "command_listener", listen):
            with self.assertRaisesRegex(RuntimeError, "processor failed"):
                await pause.main()

    async def test_async_queue_cancels_producer_when_consumer_fails(self):
        stopped = asyncio.Event()

        async def produce(queue):
            try:
                await queue.put("item")
                await asyncio.Event().wait()
            finally:
                stopped.set()

        async def consume(queue):
            await queue.get()
            raise RuntimeError("consumer failed")

        with patch.object(queue_demo, "producer", produce), patch.object(queue_demo, "consumer", consume):
            with self.assertRaises(ExceptionGroup) as caught:
                await asyncio.wait_for(queue_demo.main(), 1)
        self.assertIn("consumer failed", repr(caught.exception))
        self.assertTrue(stopped.is_set())

    async def test_crawler_cleans_up_all_workers_when_download_fails(self):
        started, stopped = set(), set()

        async def download(url):
            started.add(url)
            try:
                if url.endswith("page-1"):
                    await asyncio.sleep(0)
                    raise RuntimeError("download failed")
                await asyncio.Event().wait()
            finally:
                stopped.add(url)

        with patch.object(pipeline, "fake_download", download), redirect_stdout(io.StringIO()):
            with self.assertRaises(ExceptionGroup) as caught:
                await asyncio.wait_for(pipeline.main(), 1)
        self.assertIn("download failed", repr(caught.exception))
        self.assertTrue(started)
        self.assertEqual(started, stopped)

    async def test_async_queue_cancels_consumer_when_producer_fails(self):
        started, stopped = asyncio.Event(), asyncio.Event()

        async def produce(queue):
            await started.wait()
            raise RuntimeError("producer failed")

        async def consume(queue):
            started.set()
            try:
                await queue.get()
            finally:
                stopped.set()

        with patch.object(queue_demo, "producer", produce), patch.object(queue_demo, "consumer", consume):
            with self.assertRaises(ExceptionGroup) as caught:
                await asyncio.wait_for(queue_demo.main(), 1)
        self.assertIn("producer failed", repr(caught.exception))
        self.assertTrue(stopped.is_set())

    async def test_async_queue_completes_and_accounts_for_sentinel(self):
        output = io.StringIO()
        with patch.object(queue_demo.random, "uniform", return_value=0), redirect_stdout(output):
            await asyncio.wait_for(queue_demo.main(), 1)
        self.assertEqual(output.getvalue().count("Consumer: Consumed item"), 10)

    async def test_crawler_processes_every_url_once_and_exits(self):
        saved = []

        async def download(url):
            await asyncio.sleep(0)
            return url

        async def save(url, html):
            self.assertEqual(url, html)
            saved.append(url)

        with patch.object(pipeline, "fake_download", download), \
             patch.object(pipeline, "save_page", save), redirect_stdout(io.StringIO()):
            await asyncio.wait_for(pipeline.main(), 1)
        expected = {
            f"https://example.com/{kind}-{number}"
            for kind in ("page", "product") for number in range(1, 5)
        }
        self.assertEqual(len(saved), 8)
        self.assertEqual(set(saved), expected)

    async def test_dashboard_header_failure_cancels_requests_before_returning(self):
        started, stopped = set(), set()

        async def request(name):
            started.add(name)
            try:
                await asyncio.Event().wait()
            finally:
                stopped.add(name)

        async def header():
            await asyncio.sleep(0)
            raise RuntimeError("header failed")

        with patch.object(dashboard, "fetch_user_profile", lambda _: request("user")), \
             patch.object(dashboard, "fetch_recent_orders", lambda _: request("orders")), \
             patch.object(dashboard, "fetch_recommendations", lambda _: request("recommendations")), \
             patch.object(dashboard, "render_static_header", header):
            with self.assertRaises(ExceptionGroup) as caught:
                await dashboard.build_dashboard_concurrently(1)
        self.assertIn("header failed", repr(caught.exception))
        self.assertEqual(started, {"user", "orders", "recommendations"})
        self.assertEqual(started, stopped)

    async def test_dashboard_request_failure_cancels_other_work(self):
        started, stopped = set(), set()

        async def fail(_):
            await asyncio.sleep(0)
            raise RuntimeError("profile failed")

        async def wait(name):
            started.add(name)
            try:
                await asyncio.Event().wait()
            finally:
                stopped.add(name)

        with patch.object(dashboard, "fetch_user_profile", fail), \
             patch.object(dashboard, "fetch_recent_orders", lambda _: wait("orders")), \
             patch.object(dashboard, "fetch_recommendations", lambda _: wait("recommendations")), \
             patch.object(dashboard, "render_static_header", lambda: wait("header")):
            with self.assertRaises(ExceptionGroup) as caught:
                await asyncio.wait_for(dashboard.build_dashboard_concurrently(1), 1)
        self.assertIn("profile failed", repr(caught.exception))
        self.assertEqual(started, {"orders", "recommendations", "header"})
        self.assertEqual(started, stopped)

    async def test_dashboard_returns_all_results_after_success(self):
        async def result(value):
            await asyncio.sleep(0)
            return value

        with patch.object(dashboard, "fetch_user_profile", lambda user_id: result({"id": user_id})), \
             patch.object(dashboard, "fetch_recent_orders", lambda _: result(["order"])), \
             patch.object(dashboard, "fetch_recommendations", lambda _: result(["book"])), \
             patch.object(dashboard, "render_static_header", lambda: result("header")):
            actual = await dashboard.build_dashboard_concurrently(7)
        self.assertEqual(actual["user"], {"id": 7})
        self.assertEqual(actual["orders"], ["order"])
        self.assertEqual(actual["recommendations"], ["book"])
        self.assertEqual(actual["header"], "header")
        self.assertGreaterEqual(actual["elapsed"], 0)


class PythonConsoleRegressions(unittest.TestCase):
    def test_completion_does_not_wait_for_idle_or_partial_console_input(self):
        script = f"""
import asyncio
import sys
sys.path.insert(0, {str(ROOT / 'src/python/asynchrony')!r})
import pause_resume as demo
async def fast_process_file(_):
    await asyncio.sleep(0.01)
demo.process_file = fast_process_file
asyncio.run(demo.main())
"""
        for partial in (b"", b"unfinished"):
            with self.subTest(partial=partial):
                process = subprocess.Popen(
                    [sys.executable, "-c", script], stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                )
                try:
                    process.stdin.write(partial)
                    process.stdin.flush()
                    # Keep stdin open: communicate() would hide the blocked input bug.
                    process.wait(timeout=3)
                    self.assertEqual(process.returncode, 0, process.stderr.read().decode())
                    self.assertIn(b"Program finished.", process.stdout.read())
                finally:
                    if process.poll() is None:
                        process.kill()
                    process.wait()
                    process.stdin.close()
                    process.stdout.close()
                    process.stderr.close()

    def test_quit_and_eof_are_normal_shutdowns(self):
        for commands in ("", "p\n", "q\n", "p\nq\n", "p\nr\nq\n"):
            with self.subTest(commands=commands):
                result = subprocess.run(
                    [sys.executable, str(ROOT / "src/python/asynchrony/pause_resume.py")],
                    input=commands, capture_output=True, text=True, timeout=3,
                )
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(result.stderr, "")
                self.assertIn("Program finished.", result.stdout)


class PythonHTTPRegressions(unittest.IsolatedAsyncioTestCase):
    @classmethod
    def setUpClass(cls):
        cls.paths = []

        class Handler(http.server.BaseHTTPRequestHandler):
            def do_GET(self):
                cls.paths.append(self.path)
                self.send_response(503 if self.path.startswith("/failure") else 200)
                self.send_header("Content-Length", "5")
                self.end_headers()
                self.wfile.write(b"hello")

            def log_message(self, *_):
                pass

        cls.server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        cls.thread = threading.Thread(target=cls.server.serve_forever)
        cls.thread.start()
        cls.base = f"http://127.0.0.1:{cls.server.server_port}"

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        cls.server.server_close()
        cls.thread.join()

    async def test_http_and_query_string_are_preserved(self):
        url = self.base + "/search;mode=all?q=threads&page=2#ignored"
        with redirect_stdout(io.StringIO()):
            self.assertEqual(await fetch.fetch_concurrently(url), 5)
        self.assertEqual(self.paths[-1], "/search;mode=all?q=threads&page=2")

    async def test_https_errors_close_the_connection(self):
        with patch.object(fetch.http.client, "HTTPSConnection") as connection:
            client = connection.return_value
            response = client.getresponse.return_value
            response.status, response.reason = 503, "Service Unavailable"
            with self.assertRaisesRegex(RuntimeError, "HTTP 503"):
                fetch.fetch_sync("https://example.com:8443/?q=threads#ignored")
        connection.assert_called_once_with("example.com", port=8443, timeout=10)
        self.assertEqual(client.request.call_args.args[:2], ("GET", "/?q=threads"))
        client.close.assert_called_once()

    async def test_http_errors_are_reported_as_failures(self):
        with redirect_stdout(io.StringIO()):
            self.assertIsNone(await fetch.fetch_concurrently(self.base + "/failure"))
            with self.assertRaisesRegex(RuntimeError, "HTTP 503"):
                await asyncio.to_thread(fetch.fetch_sync, self.base + "/failure")


if __name__ == "__main__":
    unittest.main()
