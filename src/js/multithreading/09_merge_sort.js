const { Worker, isMainThread, parentPort } = require("worker_threads");
const os = require("os");
const numCPUs = os.cpus().length;

function mergeSort(arr) {
  if (arr.length <= 1) {
    return arr;
  }

  const mid = Math.floor(arr.length / 2);
  const left = arr.slice(0, mid);
  const right = arr.slice(mid);

  return merge(mergeSort(left), mergeSort(right));
}

function merge(left, right) {
  const result = [];

  while (left.length && right.length) {
    if (left[0] < right[0]) {
      result.push(left.shift());
    } else {
      result.push(right.shift());
    }
  }

  return [...result, ...left, ...right];
}

async function threadedMergeSort(arr) {
  const chunkSize = Math.ceil(arr.length / numCPUs);
  const chunks = [];

  for (let i = 0; i < arr.length; i += chunkSize) {
    chunks.push(arr.slice(i, i + chunkSize));
  }

  const workers = chunks.map(async (chunk) => {
    return new Promise((resolve) => {
      const worker = new Worker(__filename);
      worker.postMessage(chunk);
      worker.on("message", resolve);
      worker.on("error", (error) => {
        console.error("Worker error:", error);
      });
    });
  });

  const sortedChunks = await Promise.all(workers);

  return sortedChunks.reduce((acc, chunk) => merge(acc, chunk), []);
}

if (isMainThread) {
  const arr = Array.from({ length: 10000 }, () =>
    Math.floor(Math.random() * 10000)
  );

  const start = Date.now();
  const sortedArr = mergeSort(arr);
  const end = Date.now();
  console.log(`Non-threaded merge sort time: ${(end - start) / 1000} seconds`);

  const startThreaded = Date.now();
  threadedMergeSort(arr.slice()).then((sortedArrThreaded) => {
    const endThreaded = Date.now();
    console.log(
      `Threaded merge sort time: ${
        (endThreaded - startThreaded) / 1000
      } seconds`
    );
  });
} else {
  parentPort.once("message", (arr) => {
    const sortedArr = mergeSort(arr);
    parentPort.postMessage(sortedArr);
  });
}
