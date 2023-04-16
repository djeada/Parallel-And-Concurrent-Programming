const axios = require('axios');
const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

const urls = [
  'https://www.example.com',
  'https://www.example.org',
  'https://www.example.net',
  'https://www.example.edu',
];

async function fetchUrl(url) {
  const response = await axios.get(url);
  const contentLength = response.data.length;
  return { url, contentLength };
}

async function nonConcurrentFetch() {
  const nonConcurrentResults = {};
  const startTime = Date.now();

  for (const url of urls) {
    const { url: fetchedUrl, contentLength } = await fetchUrl(url);
    nonConcurrentResults[fetchedUrl] = contentLength;
    console.log(`Non-concurrent - URL: ${fetchedUrl}, Content Length: ${contentLength}`);
  }

  console.log(`Non-concurrent results: ${JSON.stringify(nonConcurrentResults)}`);
  console.log(`Non-concurrent time taken: ${(Date.now() - startTime) / 1000} seconds`);
}

if (isMainThread) {
  (async () => {
    // Non-concurrent fetching
    await nonConcurrentFetch();

    // Concurrent fetching
    const startTime = Date.now();
    const workers = [];

    for (const url of urls) {
      const worker = new Worker(__filename, { workerData: url });
      worker.on('message', ({ url, contentLength }) => {
        console.log(`Concurrent - URL: ${url}, Content Length: ${contentLength}`);
      });
      workers.push(worker);
    }

    for (const worker of workers) {
      await new Promise((resolve) => {
        worker.on('exit', () => {
          resolve();
        });
      });
    }

    console.log(`Concurrent time taken: ${(Date.now() - startTime) / 1000} seconds`);
  })();
} else {
  (async () => {
    const { url, contentLength } = await fetchUrl(workerData);
    parentPort.postMessage({ url, contentLength });
  })();
}
