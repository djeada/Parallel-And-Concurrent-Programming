/*
 * exec(), execFile(), and execSync()
 *
 * child_process offers several APIs for spawning external commands:
 *   spawn()    — streaming I/O, no shell, most flexible
 *   fork()     — spawn() with built-in IPC (Node-to-Node only)
 *   exec()     — wraps command in a shell, buffers all output
 *   execFile() — like exec() but skips the shell (safer)
 *   execSync() — synchronous exec() — blocks the event loop
 *
 * Key concepts:
 * - When to prefer exec() vs execFile() vs spawn()
 * - maxBuffer: output is buffered — large output overflows
 * - timeout: child is killed if it runs too long
 * - Shell injection risk with exec() and dynamic input (⚠️ ANTIPATTERN)
 * - execSync() blocks the event loop — only use in scripts/startup
 */

"use strict";

const { exec, execFile, execSync } = require("child_process");
const { promisify } = require("util");

const execAsync = promisify(exec);
const execFileAsync = promisify(execFile);

// ---------------------------------------------------------------------------
// Demo 1: exec() — command string passed to a shell
// ---------------------------------------------------------------------------
const demo1 = () =>
  new Promise((resolve) => {
    console.log("--- Demo 1: exec() with shell ---");

    // exec() invokes /bin/sh -c "<cmd>", so shell features (pipes, &&) work.
    exec(`${process.execPath} -e "process.stdout.write('hello from exec')"`, (err, stdout, stderr) => {
      if (err) { console.error("exec error:", err.message); return resolve(); }
      console.log("stdout:", stdout);
      if (stderr) console.error("stderr:", stderr);
      resolve();
    });
  });

// ---------------------------------------------------------------------------
// Demo 2: execFile() — no shell, direct binary execution
// ---------------------------------------------------------------------------
const demo2 = async () => {
  console.log("\n--- Demo 2: execFile() without shell ---");

  // execFile() passes args as an array — no shell quoting issues.
  const { stdout } = await execFileAsync(process.execPath, [
    "-e",
    "process.stdout.write('hello from execFile')",
  ]);
  console.log("stdout:", stdout);
};

// ---------------------------------------------------------------------------
// Demo 3: exec() with maxBuffer and timeout options
// ---------------------------------------------------------------------------
const demo3 = async () => {
  console.log("\n--- Demo 3: maxBuffer and timeout ---");

  // maxBuffer: default 1 MB. Exceeding it throws ERR_CHILD_PROCESS_STDIO_MAXBUFFER.
  try {
    await execAsync(
      `${process.execPath} -e "process.stdout.write('x'.repeat(10))"`,
      { maxBuffer: 5 } // intentionally tiny — 5 bytes
    );
  } catch (err) {
    console.log("maxBuffer exceeded:", err.code || err.message);
  }

  // timeout: child is killed with SIGTERM after N ms.
  try {
    await execAsync(
      `${process.execPath} -e "setTimeout(()=>{},5000)"`,
      { timeout: 200 }
    );
  } catch (err) {
    console.log("timeout hit — signal:", err.signal, "killed:", err.killed);
  }
};

// ---------------------------------------------------------------------------
// Demo 4: execSync() — synchronous, blocks the event loop
// ---------------------------------------------------------------------------
const demo4 = () => {
  console.log("\n--- Demo 4: execSync() (synchronous) ---");
  console.log("NOTE: execSync() blocks the event loop. Only use in scripts or startup.");

  const output = execSync(
    `${process.execPath} -e "process.stdout.write('hello from execSync')"`,
    { encoding: "utf8" }
  );
  console.log("stdout:", output);
};

// ---------------------------------------------------------------------------
// Demo 5: ⚠️  ANTIPATTERN — shell injection with exec()
// ---------------------------------------------------------------------------
const demo5 = () => {
  console.log("\n--- Demo 5: ⚠️  ANTIPATTERN — shell injection ---");
  console.log(
    "exec() passes the command through a shell.\n" +
    "Concatenating user input into the command string allows shell injection.\n"
  );

  // Imagine `userInput` comes from a CLI argument or HTTP request.
  const safeInput = "someFile.txt";
  // ANTIPATTERN: exec(`cat ${userInput}`) — attacker passes "; rm -rf /"
  console.log("ANTIPATTERN: exec(`cat ${safeInput}`) — never interpolate untrusted input");
  console.log("FIX: use execFile('cat', [userInput]) — args are never parsed by a shell");
  console.log("     or: use fs.readFile() directly — no subprocess at all");
};

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
const main = async () => {
  console.log("=== exec / execFile / execSync Demo ===\n");
  console.log("API comparison:");
  console.log("  exec()     — shell, async, buffered output");
  console.log("  execFile() — no shell, async, buffered output (preferred for binaries)");
  console.log("  execSync() — no shell by default, synchronous, blocks event loop\n");

  await demo1();
  await demo2();
  await demo3();
  demo4();
  demo5();

  console.log("\n=== Demo Complete ===");
};

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
