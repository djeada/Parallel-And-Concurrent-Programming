const spawn = require("child_process").spawn;

const worker_script_path = "./worker_script.js";

function spawn_new_process(worker_script_path, args) {
  const worker = spawn(worker_script_path, args);
  worker.stdout.on("data", (data) => {
    console.log(`stdout: ${data}`);
    worker.kill();
  });

  worker.stderr.on("data", (data) => {
    console.log(`stderr: ${data}`);
    worker.kill();
  });

  worker.on("close", (code) => {
    console.log(`child process with id ${worker.pid} exited with code ${code}`);
  });
}

function spawn_n_processes(worker_script_path, args, n) {
  for (let i = 0; i < n; i++) {
    spawn_new_process(worker_script_path, args);
  }
}

function main() {
  const n = 10;
  const args = ["--n", n.toString()];
  spawn_n_processes(worker_script_path, args, n);
}

main();
