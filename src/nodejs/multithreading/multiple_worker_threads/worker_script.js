const {parentPort} = require("worker_threads");

parentPort.on("message", (message) => {
    parentPort.postMessage(`worker thread process id: ${process.pid}`);
});
