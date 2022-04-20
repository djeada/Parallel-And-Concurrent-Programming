const fs = require('fs');
const {Worker} = require("worker_threads");

const worker_script_path = "./worker_script.js";

const worker_script = ` 
const {parentPort} = require("worker_threads");

parentPort.on("message", (message) => {
    parentPort.postMessage(\`worker thread process id: ${process.pid}\`);
});
`;

// write worker script to file
fs.writeFileSync(worker_script_path, worker_script);

const worker = new Worker(worker_script_path, {
    workerData: "worker thread data"
});

worker.on("message", (message) => {
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
