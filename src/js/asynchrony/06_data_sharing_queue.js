/*
This Node.js script demonstrates data sharing and communication between asynchronous tasks
using JavaScript Promises. In this example, we simulate a producer and consumer
scenario where one task produces data and the other consumes it using Promises for
communication and synchronization.
*/

const produceData = () => {
  const data = Math.floor(Math.random() * 100) + 1;
  console.log(`Produced data: ${data}`);
  return data;
};

const consumeData = (data) => {
  console.log(`Consumed data: ${data}`);
};

const main = async () => {
  const dataPromise = new Promise((resolve) => {
    const data = produceData();
    resolve(data);
  });

  const data = await dataPromise;
  consumeData(data);
};

main();
