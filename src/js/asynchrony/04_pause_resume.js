/*
This script demonstrates how to pause and resume function execution using
setInterval, clearInterval, and readline in Node.js. It consists of two main parts:
1. A long-running function that periodically prints its progress.
2. A button handler that listens for user input to toggle pause and resume.

The script shows how to use Node.js setInterval and clearInterval to
effectively pause and resume the execution of the long-running function.
*/

const readline = require('readline');

let paused = false;
let intervalId;
let step = 0;

const longFunction = () => {
  console.log(`Executing step ${step}`);
  step += 1;
};

const togglePause = () => {
  paused = !paused;
  if (paused) {
    clearInterval(intervalId);
    console.log('Function paused');
  } else {
    intervalId = setInterval(longFunction, 1000);
    console.log('Function resumed');
  }
};

const main = () => {
  intervalId = setInterval(longFunction, 1000);

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on('line', () => {
    togglePause();
  });
};

main();
