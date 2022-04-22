const {Worker} = require("worker_threads");

const worker_script_path = "./worker_script.js";

const worker = new Worker(worker_script_path, {
    workerData: "worker thread data"
});

worker.on("message", (message) => {
    console.log(`Received a message from thread with id: ${ worker.threadId}`)
    console.log(message);
    worker.terminate();
});


worker.on("error", (err) => {
    console.log(err);
    worker.terminate();
});


function main() {
    console.log(`main thread process id: ${process.pid}`);
    worker.postMessage("message from main thread");
}

main();
