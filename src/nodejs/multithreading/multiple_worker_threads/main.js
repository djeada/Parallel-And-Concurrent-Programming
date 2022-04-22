const {Worker} = require("worker_threads");

const worker_script_path = "./worker_script.js";


function spawn_worker(worker_script_path) {
    const worker = new Worker(worker_script_path);
    worker.on("message", (message) => {
        console.log(`Received a message from thread with id: ${worker.threadId}`)
        console.log(message);
        worker.terminate();
    });
    worker.on("error", (error) => {
        console.log(error);
        worker.terminate();
    });
    worker.on("exit", (code) => {
        console.log(`Worker ${worker.threadId} exited with code: ${code}`);
    });
    return worker;
}


function spawn_n_workers(n, worker_script_path) {
    const workers = [];
    for (let i = 0; i < n; i++) {
        workers.push(spawn_worker(worker_script_path));
    }
    return workers;
}

function main() {
    const workers = spawn_n_workers(10, worker_script_path);
    workers.forEach((worker) => {
        worker.postMessage("Hello from main thread");
    });
}

main();
