/*
This program demonstrates the concept of orphan processes in Node.js. An orphan process
is a child process whose parent process has terminated before the child process. In most
operating systems, including Linux, when the parent process terminates, its orphaned
children are reparented to the init or systemd process (PID 1).

In this example, we create a child process that outlives its parent process, becoming
an orphan. After the parent process exits, you can check the process list (using
commands like 'ps' or 'top') to see that the child process is still running and has
been reparented to the init or systemd process (PID 1).
*/

const { fork } = require('child_process');

if (process.argv[2] === 'child') {
    // Child process
    console.log(`Child process (PID: ${process.pid}) started.`);
    setTimeout(() => {
        console.log(`Child process (PID: ${process.pid}) finished.`);
    }, 10000);
} else {
    // Parent process
    console.log(`Parent process (PID: ${process.pid}) started.`);
    const child = fork(__filename, ['child']);
    console.log(`Parent process (PID: ${process.pid}) is exiting, leaving the child process orphaned.`);
    process.exit();
}
