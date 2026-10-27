"""Modern C++ lifetime and synchronization checks (g++, C++20, POSIX).

Run: python3 -m unittest discover -s tests -p 'test_cpp*.py' -v
"""

from pathlib import Path
import re
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class CppModernExamples(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory(prefix="cpp-modern-")
        cls.addClassCleanup(cls.directory.cleanup)
        cls.binaries = {}

        # Match libstdc++'s selected execution backend rather than assuming
        # either serial execution or the presence of TBB development files.
        macros = subprocess.run(
            ["g++", "-std=c++20", "-dM", "-E", "-x", "c++", "-"],
            input="#include <execution>\n", capture_output=True, text=True, check=True,
        ).stdout
        parallel_libraries = ["-ltbb"] if "#define _PSTL_PAR_BACKEND_TBB" in macros else []
        examples = {
            "atomic_wait": "multithreading/atomic_wait.cpp",
            "algorithms": "multithreading/parallel_algorithms.cpp",
            "queue": "multithreading/producer_consumer.cpp",
            "barrier": "multithreading/std_barrier.cpp",
            "latch": "multithreading/std_latch.cpp",
            "semaphore": "multithreading/std_semaphore.cpp",
            "pool": "multithreading/thread_pool.cpp",
        }
        for name, source in examples.items():
            libraries = parallel_libraries if name == "algorithms" else []
            cls.compile(name, ROOT / "src/cpp" / source, libraries)

        harnesses = {
            "scheduler_lifetime": ("multithreading/scheduler.cpp", r"""
                #include <latch>
                int main() {
                    std::latch ran(1);
                    {
                        Scheduler scheduler(60, [&] { ran.count_down(); });
                        ran.wait();
                    } // Stop must interrupt the 60-second interval.
                    std::latch ran_again(1);
                    Scheduler scheduler(60, [&] { ran_again.count_down(); });
                    ran_again.wait();
                    scheduler.stop();
                    scheduler.stop(); // Repeated explicit stop is harmless.
                    std::cout << "Schedulers stopped\n";
                }
            """),
            "queue_lifetime": ("multithreading/producer_consumer.cpp", r"""
                #include <cassert>
                #include <latch>
                int main() {
                    TaskQueue drained(1);
                    assert(drained.add_task(-1)); // Every int is valid data.
                    drained.close();
                    assert(drained.take_task() == -1);
                    assert(!drained.take_task());
                    assert(!drained.add_task(2));

                    TaskQueue full(1);
                    assert(full.add_task(7));
                    std::latch adding(1);
                    bool rejected = false;
                    {
                        std::jthread producer([&] {
                            adding.count_down();
                            rejected = !full.add_task(8);
                        });
                        adding.wait();
                        full.close(); // Wake a producer waiting for capacity.
                    }
                    assert(rejected);
                    assert(full.take_task() == 7);
                    assert(!full.take_task());

                    TaskQueue empty(1);
                    std::latch taking(1);
                    bool cancelled = false;
                    {
                        std::jthread consumer([&](std::stop_token stop) {
                            taking.count_down();
                            cancelled = !empty.take_task(stop);
                        });
                        taking.wait();
                    } // Cancellation wakes a consumer without queue closure.
                    assert(cancelled);
                    std::cout << "Queue closed and cancelled cleanly\n";
                }
            """),
            "pool_lifetime": ("asynchrony/thread_pool_futures.cpp", r"""
                #include <cassert>
                int main() {
                    bool rejected = false;
                    try { ThreadPool invalid(0); }
                    catch (const std::invalid_argument&) { rejected = true; }
                    assert(rejected);

                    std::vector<std::future<int>> results;
                    std::future<int> failure;
                    std::future<int> moved;
                    {
                        ThreadPool pool(4);
                        for (int i = 0; i < 100; ++i) {
                            results.push_back(pool.submit([i] { return i * i; }));
                        }
                        failure = pool.submit([]() -> int {
                            throw std::runtime_error("expected failure");
                        });
                        moved = pool.submit([](std::unique_ptr<int> value) {
                            return *value;
                        }, std::make_unique<int>(42));
                    } // Accepted work must drain before any future is read.
                    for (int i = 0; i < 100; ++i) {
                        assert(results[i].get() == i * i);
                    }
                    assert(moved.get() == 42);
                    bool propagated = false;
                    try { failure.get(); }
                    catch (const std::runtime_error&) { propagated = true; }
                    assert(propagated);
                    std::cout << "Pool drained and propagated exceptions\n";
                }
            """),
        }
        for name, (example, body) in harnesses.items():
            source = Path(cls.directory.name) / f"{name}.cpp"
            source.write_text(
                '#define main example_main\n'
                f'#include "{ROOT / "src/cpp" / example}"\n'
                '#undef main\n' + body
            )
            cls.compile(name, source)

    @classmethod
    def compile(cls, name, source, libraries=()):
        binary = Path(cls.directory.name) / name
        subprocess.run(
            ["g++", "-std=c++20", "-O1", "-Wall", "-Wextra", "-pthread",
             str(source), "-o", str(binary), *libraries],
            capture_output=True, text=True, check=True,
        )
        cls.binaries[name] = binary

    def run_example(self, name, timeout=5):
        result = subprocess.run(
            [str(self.binaries[name])], capture_output=True, text=True, timeout=timeout,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout

    def test_atomic_wait_publishes_payload(self):
        self.assertIn("Payload sum: 15", self.run_example("atomic_wait"))

    def test_parallel_algorithms_match_reference(self):
        output = self.run_example("algorithms")
        self.assertIn("Sum: 499999500000", output)
        self.assertIn("Expected: 499999500000", output)
        self.assertIn("Sorted: true", output)

    def test_bounded_queue_delivers_all_tasks(self):
        output = self.run_example("queue")
        counts = re.findall(r"Consumer \d+: completed (\d+) tasks", output)
        self.assertEqual(len(counts), 2, output)
        self.assertEqual(sum(map(int, counts)), 10000)
        self.assertIn("All tasks completed.", output)

    def test_barrier_completes_each_phase_once(self):
        output = self.run_example("barrier")
        for phase in range(1, 4):
            self.assertEqual(output.count(f"Phase {phase} complete. Sum = {6 * phase}"), 1)

    def test_latch_releases_only_after_all_workers_load(self):
        output = self.run_example("latch")
        release = output.index("All workers ready")
        for worker in range(1, 6):
            self.assertLess(output.index(f"Worker {worker}: data loaded."), release)
            self.assertGreater(output.index(f"Worker {worker}: processing!"), release)

    def test_basic_pool_drains_accepted_jobs(self):
        output = self.run_example("pool")
        tasks = re.findall(r"processed task (\d+)\.", output)
        self.assertEqual(sorted(map(int, tasks)), list(range(20)))

    def test_semaphore_bounds_access_and_publishes_result(self):
        output = self.run_example("semaphore")
        occupants = set()
        entries = []
        for worker, event in re.findall(r"Worker (\d+) (entered|leaving) room\.", output):
            if event == "entered":
                self.assertNotIn(worker, occupants)
                occupants.add(worker)
                entries.append(int(worker))
                self.assertLessEqual(len(occupants), 3)
            else:
                self.assertIn(worker, occupants)
                occupants.remove(worker)
        self.assertFalse(occupants)
        self.assertEqual(sorted(entries), list(range(1, 8)))
        self.assertIn("Consumer: received value = 99", output)

    def test_scheduler_shutdown_interrupts_long_interval(self):
        self.assertIn("Schedulers stopped", self.run_example("scheduler_lifetime", timeout=2))

    def test_queue_closure_and_cancellation_release_waiters(self):
        self.assertIn("Queue closed and cancelled cleanly", self.run_example("queue_lifetime", timeout=2))

    def test_future_pool_drains_jobs_and_transports_failures(self):
        self.assertIn("Pool drained and propagated exceptions", self.run_example("pool_lifetime", timeout=2))


if __name__ == "__main__":
    unittest.main()
