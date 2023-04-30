const http = require("http");
const https = require("https");

const URLS = [
  "http://httpbin.org/get",
  "https://api.github.com",
  "https://www.example.com",
];

function fetchUrl(url) {
  return new Promise((resolve, reject) => {
    const protocol = url.startsWith("https") ? https : http;
    protocol
      .get(url, (res) => {
        let data = "";
        res.on("data", (chunk) => {
          data += chunk;
        });
        res.on("end", () => {
          resolve(data);
        });
      })
      .on("error", (err) => {
        reject(err);
      });
  });
}

async function fetchSync(url) {
  const response = await fetchUrl(url);
  console.log(`Synchronously fetched content from ${url}`);
  return response;
}

async function fetchAllSync(urls) {
  const contents = [];
  for (const url of urls) {
    contents.push(await fetchSync(url));
  }
  return contents;
}

async function fetchAsync(url) {
  return fetchSync(url);
}

async function fetchAllAsync(urls) {
  const promises = urls.map((url) => fetchAsync(url));
  const contents = await Promise.all(promises);
  return contents;
}

async function main() {
  const startTimeSync = Date.now();
  const contentsSync = await fetchAllSync(URLS);
  const elapsedTimeSync = (Date.now() - startTimeSync) / 1000;
  console.log(`\nSynchronous execution took ${elapsedTimeSync} seconds.`);

  const startTimeAsync = Date.now();
  const contentsAsync = await fetchAllAsync(URLS);
  const elapsedTimeAsync = (Date.now() - startTimeAsync) / 1000;
  console.log(`\nAsynchronous execution took ${elapsedTimeAsync} seconds.`);
}

main();
