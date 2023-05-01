/*
This program demonstrates the concept of zombie processes in Node.js. A zombie process is
a child process that has terminated but has not been reaped by its parent process. The
operating system keeps an entry in the process table for the terminated child process,
holding information about its termination status until the parent process collects this
information.

In this example, we create a child process that terminates but isn't reaped by its
parent, becoming a zombie. After the child process exits, you can check the process
list (using commands like 'ps' or 'top') to see that the child process is in a 'zombie'
state.

WARNING: This example intentionally creates a zombie process. You should manually kill
the parent process after running the program to avoid having a zombie process lingering
on your system.
*/

const { spawn } = require("child_process");

if (process.argv[2] === "child") {
  // Child process
  console.log(`Child process (PID: ${process.pid}) started.`);
  setTimeout(() => {
    console.log(`Child process (PID: ${process.pid}) finished.`);
    process.exit(0);
  }, 2000);
} else {
  // Parent process
  const child = spawn(process.execPath, [__filename, "child"], {
    detached: true,
    stdio: "inherit",
  });

  console.log(`Parent process (PID: ${process.pid}) started.`);
  console.log(
    `Parent process (PID: ${process.pid}) is NOT waiting for the child process to terminate.`
  );
  console.log(
    `Child process (PID: ${child.pid}) will become a zombie after termination.`
  );

  setTimeout(() => {
    console.log(`Parent process (PID: ${process.pid}) finished.`);
  }, 10000);
}
