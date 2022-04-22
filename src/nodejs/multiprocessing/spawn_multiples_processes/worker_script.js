const {parentPort} = require("worker_threads");

const args = process.argv.slice(2);
const n = parseInt(args[0]);

console.log(\`worker started with args: \${args}\`);

parentPort.on("message", (message) => {
    parentPort.postMessage(\`message received by ${process.pid}\`);
});
