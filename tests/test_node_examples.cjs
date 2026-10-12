"use strict";

// Run: node --test tests/test_node_examples.cjs
const test = require("node:test");
const assert = require("node:assert/strict");
const { spawn } = require("node:child_process");
const { once, getEventListeners } = require("node:events");
const { setTimeout: sleep } = require("node:timers/promises");
const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");
const net = require("node:net");
const http = require("node:http");
const readline = require("node:readline");
const root = path.resolve(__dirname, "..");
const example = (name) => path.join(root, "src/js", name);
const { AsyncQueue } = require(example("asynchrony/06_data_sharing_queue.js"));
const { abortableSleep } = require(example("asynchrony/16_abort_controller.js"));
const { CircuitBreaker, CircuitState, withRetry } = require(example("asynchrony/19_retry_backoff.js"));
const { makeCountingReadable } = require(example("asynchrony/20_async_stream.js"));
const { parallelMergeSort } = require(example("multithreading/09_merge_sort.js"));
const { TaskServer } = require(example("asynchrony/14_distributed_computing.js"));
const { handleRequest } = require(example("asynchrony/13_async_server.js"));

function runExample(name, { args = [], input = "", timeout = 10000, env = {}, onOutput } = {}) {
  return new Promise((resolve, reject) => {
    const child = spawn(process.execPath, [example(name), ...args], {
      env: { ...process.env, ...env }, detached: process.platform !== "win32",
      stdio: ["pipe", "pipe", "pipe"],
    });
    let stdout = "", stderr = "";
    let timedOut = false;
    const timer = setTimeout(() => {
      timedOut = true;
      if (process.platform === "win32") child.kill();
      else process.kill(-child.pid, "SIGKILL");
    }, timeout);
    child.stdout.on("data", (chunk) => {
      stdout += chunk;
      onOutput?.(child, stdout);
    });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.on("error", (error) => { clearTimeout(timer); reject(error); });
    child.on("close", (code, signal) => {
      clearTimeout(timer);
      if (timedOut) reject(new Error(`${name} timed out\n${stdout}\n${stderr}`));
      else resolve({ code, signal, stdout, stderr });
    });
    child.stdin.end(input);
  });
}

function deferred() {
  let resolve, reject;
  const promise = new Promise((yes, no) => { resolve = yes; reject = no; });
  return { promise, resolve, reject };
}

async function within(promise, ms = 2000) {
  let timer;
  try {
    return await Promise.race([promise, new Promise((_, reject) => {
      timer = setTimeout(() => reject(new Error("Operation timed out")), ms);
    })]);
  } finally { clearTimeout(timer); }
}

test("bounded queue reserves slots for waiting producers without losing wakeups", async () => {
  const queue = new AsyncQueue(1);
  await queue.put("first");
  const second = queue.put("second");
  const third = queue.put("third");
  // Take twice before any woken producer gets a microtask turn.
  const firstRead = queue.get();
  const secondRead = queue.get();
  const thirdRead = queue.get();
  assert.deepEqual(await within(Promise.all([firstRead, secondRead, thirdRead])),
    ["first", "second", "third"]);
  await within(Promise.all([second, third]));
  assert.equal(queue.size, 0);
  assert.throws(() => new AsyncQueue(0), RangeError);
});

test("bounded queue handles concurrent consumers and producers exactly once", async () => {
  const queue = new AsyncQueue(2);
  const puts = Array.from({ length: 100 }, (_, i) => queue.put(i));
  const gets = Array.from({ length: 100 }, () => queue.get());
  assert.deepEqual(await within(Promise.all(gets)), Array.from({ length: 100 }, (_, i) => i));
  await within(Promise.all(puts));
  assert.equal(queue.size, 0);
});

test("completed sleeps release abort listeners; cancellation preserves its reason", async () => {
  const controller = new AbortController();
  for (let i = 0; i < 20; i++) await abortableSleep(1, controller.signal);
  assert.equal(getEventListeners(controller.signal, "abort").length, 0);
  const reason = new Error("cancelled");
  const pending = abortableSleep(1000, controller.signal);
  controller.abort(reason);
  await assert.rejects(pending, (error) => error === reason);
  await assert.rejects(abortableSleep(1000, controller.signal), (error) => error === reason);
  assert.equal(getEventListeners(controller.signal, "abort").length, 0);
});

test("half-open circuit allows exactly one recovery probe", async () => {
  const circuit = new CircuitBreaker(1, 0);
  await assert.rejects(circuit.call(async () => { throw new Error("down"); }));
  const gate = deferred();
  const probe = circuit.call(() => gate.promise);
  await assert.rejects(circuit.call(async () => "must not run"), /probe already in progress/);
  gate.resolve("recovered");
  assert.equal(await probe, "recovered");
  assert.equal(circuit.status, CircuitState.CLOSED);
});

test("late successes cannot close a circuit opened by another request", async () => {
  const circuit = new CircuitBreaker(1, 0);
  const gate = deferred();
  const oldRequest = circuit.call(() => gate.promise);
  await assert.rejects(circuit.call(async () => { throw new Error("down"); }));
  gate.resolve("late result");
  assert.equal(await oldRequest, "late result");
  assert.equal(circuit.status, CircuitState.OPEN);
  await assert.rejects(withRetry(async () => "x", { maxAttempts: 0 }), RangeError);
});

test("readable backpressure bounds buffered data for a slow consumer", async () => {
  const source = makeCountingReadable(100, "slow");
  const seen = [];
  for await (const chunk of source) {
    seen.push(chunk.index);
    await sleep(1);
    assert.ok(source.readableLength <= source.readableHighWaterMark);
  }
  assert.deepEqual(seen, Array.from({ length: 100 }, (_, i) => i));
});

test("mutex and semaphore finish with the correct counter without overflow", async () => {
  for (const name of ["05_mutex.js", "06_semaphore.js"]) {
    const result = await runExample(`multithreading/${name}`);
    assert.equal(result.code, 0, result.stderr);
    assert.match(result.stdout, /Final counter value: 207360000/);
  }
});

test("pause/resume stops on EOF, including while paused", async () => {
  for (const input of ["", "\n", "\n\n"]) {
    const result = await runExample("asynchrony/04_pause_resume.js", { input, timeout: 2000 });
    assert.equal(result.code, 0, result.stderr);
    assert.match(result.stdout, /Task stopped at step/);
  }
});

test("atomic wait timeout demo runs even after the signaling worker exits", async () => {
  const result = await runExample("multithreading/17_atomics_waitasync.js");
  assert.equal(result.code, 0, result.stderr);
  assert.match(result.stdout, /Short wait resolved: "timed-out"/);
  assert.match(result.stdout, /Demo complete/);
});

test("parallel merge sort handles empty input, duplicates and uneven chunks", async () => {
  for (const input of [[], [1], [5, 1, 5, -2, 8, 0, 3]]) {
    assert.deepEqual(await parallelMergeSort(input), [...input].sort((a, b) => a - b));
  }
  const result = await runExample("multithreading/09_merge_sort.js");
  assert.equal(result.code, 0, result.stderr);
  assert.equal((result.stdout.match(/Correctly sorted: true/g) || []).length, 2);
});

test("shared buffer example observes updates made after worker startup", async () => {
  const result = await runExample("multithreading/16_transferable.js");
  assert.equal(result.code, 0, result.stderr);
  assert.match(result.stdout, /main updated view\[0\]=100, worker read view\[0\]=100/);
});

test("async antipattern demonstrates a real escaped rejection and completes", async () => {
  const result = await runExample("asynchrony/17_async_antipatterns.js");
  assert.equal(result.code, 0, result.stderr);
  assert.match(result.stdout, /Async rejection escaped try\/catch: Op will-throw-but-not-caught failed/);
});

test("HTTP routes reject user-prefix lookalikes", async (t) => {
  const server = http.createServer((req, res) => handleRequest(req, res).catch(() => res.destroy()));
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  t.after(() => new Promise((resolve) => server.close(resolve)));
  const base = `http://127.0.0.1:${server.address().port}`;
  const bad = await fetch(`${base}/userish`);
  assert.equal(bad.status, 404);
  await bad.text();
  const good = await fetch(`${base}/user/42`);
  assert.equal(good.status, 200);
  assert.equal((await good.json()).id, "42");
});

test("distributed server routes work to workers and results to subscribers", async (t) => {
  const server = new TaskServer("127.0.0.1", 0);
  server.start();
  await once(server.server, "listening");
  const port = server.server.address().port;
  const subscriber = net.createConnection({ host: "127.0.0.1", port });
  const worker = net.createConnection({ host: "127.0.0.1", port });
  t.after(() => {
    subscriber.destroy();
    worker.destroy();
    server.stop();
  });
  await Promise.all([once(subscriber, "connect"), once(worker, "connect")]);
  const messages = [];
  const clientLines = readline.createInterface({ input: subscriber });
  clientLines.on("line", (line) => messages.push(JSON.parse(line)));
  const workerLines = readline.createInterface({ input: worker });
  subscriber.write(JSON.stringify({ type: "register", role: "client" }) + "\n");
  worker.write(JSON.stringify({ type: "register", role: "worker" }) + "\n");
  const [line] = await within(once(workerLines, "line"));
  const task = JSON.parse(line);
  assert.equal(task.type, "compute");
  assert.deepEqual(messages, []); // Subscribers must not receive tasks.
  const resultLine = once(clientLines, "line");
  const result = { taskId: task.id, result: task.data * 2 };
  worker.write(JSON.stringify(result) + "\n");
  assert.deepEqual(JSON.parse((await within(resultLine))[0]), result);
});

test("lock acquisition failure does not remove another process's lock", async (t) => {
  const directory = fs.mkdtempSync(path.join(os.tmpdir(), "node-lock-test-"));
  t.after(() => fs.rmSync(directory, { recursive: true, force: true }));
  const lock = path.join(directory, "counter.lock");
  fs.writeFileSync(lock, "other-owner");
  const result = await runExample("multiprocessing/10_process_lock.js", {
    args: ["worker"], env: { LOCK_FILE: lock, COUNTER_FILE: path.join(directory, "counter") },
  });
  assert.equal(result.code, 1);
  assert.equal(fs.readFileSync(lock, "utf8"), "other-owner");
});

test("supervisor shuts down its child on SIGTERM and cancels restarts", async () => {
  let childPid;
  let sent = false;
  const result = await runExample("multiprocessing/18_process_restart.js", {
    onOutput(process, output) {
      const match = output.match(/Run #1 started \(PID: (\d+)\)/);
      if (match && !sent) {
        sent = true;
        childPid = Number(match[1]);
        process.kill("SIGTERM");
      }
    },
  });
  assert.equal(result.code, 0, result.stderr);
  assert.ok(sent);
  assert.throws(() => process.kill(childPid, 0), { code: "ESRCH" });
  assert.doesNotMatch(result.stdout, /run #2/);
});

test("process signal example waits for every child to exit", async () => {
  const result = await runExample("multiprocessing/17_process_signals.js");
  assert.equal(result.code, 0, result.stderr);
  assert.equal((result.stdout.match(/Exiting with code 0/g) || []).length, 3);
  assert.match(result.stdout, /All workers exited/);
});

test("cluster waits for listening and restarts before sending requests", async () => {
  const result = await runExample("multiprocessing/16_cluster_module.js", { env: { PORT: "0" } });
  assert.equal(result.code, 0, result.stderr);
  assert.doesNotMatch(result.stderr, /ECONNREFUSED/);
  assert.match(result.stdout, /All workers exited/);
});

test("native and worker fetches handle local HTTP success, failure, and timeout", async (t) => {
  const { fetchWithTimeout, fetchParallel } = require(example("asynchrony/09_fetch_parallel.js"));
  const { fetchUrl, parallelFetchWithWorkers } = require(example("multithreading/08_fetch_parallel.js"));
  const server = http.createServer((req, res) => {
    if (req.url === "/timeout") return;
    res.writeHead(req.url === "/failure" ? 503 : 200, { "Content-Type": "application/json" });
    res.end(JSON.stringify({ title: "local response" }));
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  t.after(() => {
    server.closeAllConnections();
    return new Promise((resolve) => server.close(resolve));
  });
  const base = `http://127.0.0.1:${server.address().port}`;
  assert.deepEqual(await fetchWithTimeout(base), { title: "local response" });
  await assert.rejects(fetchWithTimeout(`${base}/failure`), /HTTP 503/);
  await assert.rejects(fetchWithTimeout(`${base}/timeout`, 20), /timed out/);
  assert.equal((await fetchUrl(`${base}/timeout`, 20)).success, false);
  const urls = [base, base + "/failure", base];
  const native = await fetchParallel(urls);
  const workers = await parallelFetchWithWorkers(urls);
  assert.deepEqual(native.results.map((result) => result.success), [true, false, true]);
  assert.deepEqual(workers.results.map((result) => result.success), [true, false, true]);
});

test("thread wrapper exposes task errors with a nonzero worker exit", async () => {
  const { MyThread } = require(example("multithreading/02_thread_subclass.js"));
  const thread = new MyThread(() => { throw new Error("task failed"); });
  const errors = [];
  thread.worker.on("error", (error) => errors.push(error.message));
  const code = await within(new Promise((resolve) => thread.worker.once("exit", resolve)));
  assert.equal(code, 1);
  assert.deepEqual(errors, ["task failed"]);
});

test("worker pools fail visibly and release workers after an unexpected crash", async (t) => {
  const directory = fs.mkdtempSync(path.join(os.tmpdir(), "node-pool-test-"));
  t.after(() => fs.rmSync(directory, { recursive: true, force: true }));
  const preload = path.join(directory, "fail-worker.cjs");
  fs.writeFileSync(preload, `
    const { isMainThread } = require('node:worker_threads');
    if (!isMainThread || process.argv[2] === 'worker') {
      throw new Error('injected worker failure');
    }
  `);
  for (const name of ["multiprocessing/04_process_pool.js", "multithreading/13_thread_pool.js"]) {
    const result = await runExample(name, {
      env: { NODE_OPTIONS: `--require=${preload}` }, timeout: 3000,
    });
    assert.equal(result.code, 1, result.stderr);
    assert.match(result.stderr, /pool failed/i);
  }
});
