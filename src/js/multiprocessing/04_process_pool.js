/*
 * Process Pool Pattern
 *
 * A true process pool maintains a fixed set of persistent worker processes.
 * Workers are forked once and receive multiple tasks over IPC — they are
 * never restarted between tasks. This avoids fork() overhead per task and
 * is the correct pattern for CPU-bound parallelism in Node.js.
 *
 * Key concepts:
 * - Fixed pool of persistent worker processes
 * - Task queue dispatched via IPC (no new fork per task)
 * - CPU-bound task parallelization (real math, not sleep)
 * - os.availableParallelism() for correct core count in containers
 * - Graceful pool shutdown via shutdown message
 */

"use strict";

const { fork } = require("child_process");
const os = require("os");

const POOL_SIZE = Math.min(os.availableParallelism?.() ?? os.cpus().length, 4);
const NUM_TASKS = 10;
const WORK_N = 1_500_000; // iterations per task

// CPU-bound computation: sum of sqrt(i) for i in [0, n)
const cpuWork = (n) => {
  let sum = 0;
  for (let i = 0; i < n; i++) sum += Math.sqrt(i);
  return sum;
};

if (process.argv[2] === "worker") {
  // Persistent worker: loop until "shutdown" message
  process.on("message", (msg) => {
    if (msg.type === "task") {
      const start = Date.now();
      const result = cpuWork(msg.n);
      process.send({
        type: "result",
        taskId: msg.taskId,
        result: result.toFixed(2),
        elapsed: Date.now() - start,
      });
    } else if (msg.type === "shutdown") {
      process.exit(0);
    }
  });
} else {
  console.log("=== Process Pool Demo ===\n");
  console.log(`Pool size : ${POOL_SIZE} persistent workers`);
  console.log(`Tasks     : ${NUM_TASKS}`);
  console.log(`Work (N)  : ${WORK_N.toLocaleString()} sqrt iterations per task\n`);

  const pending = Array.from({ length: NUM_TASKS }, (_, i) => ({ taskId: i, n: WORK_N }));
  const results = [];
  const startTime = Date.now();

  // Fork the pool once — workers stay alive across all tasks
  const pool = Array.from({ length: POOL_SIZE }, () => fork(__filename, ["worker"]));
  const idle = [...pool]; // all workers start idle

  const dispatch = () => {
    while (idle.length > 0 && pending.length > 0) {
      const worker = idle.shift();
      const task = pending.shift();
      console.log(`  [Pool] Task ${task.taskId} → worker ${worker.pid}`);
      worker.send({ type: "task", ...task });
    }
  };

  pool.forEach((worker) => {
    worker.on("message", (msg) => {
      if (msg.type !== "result") return;
      console.log(`  [Task ${msg.taskId}] done in ${msg.elapsed}ms  result: ${msg.result}`);
      results.push(msg);
      idle.push(worker); // return worker to idle pool

      if (results.length === NUM_TASKS) {
        const elapsed = Date.now() - startTime;
        console.log("\n=== Results ===");
        console.log(`All ${NUM_TASKS} tasks completed in ${elapsed}ms`);
        console.log(`Pool reused ${POOL_SIZE} workers — no re-fork overhead`);
        pool.forEach((w) => w.send({ type: "shutdown" }));
        return;
      }
      dispatch(); // next task for this now-idle worker
    });

    worker.on("error", (err) =>
      console.error(`Worker ${worker.pid} error: ${err.message}`)
    );
  });

  dispatch(); // initial dispatch fills all idle workers
}
