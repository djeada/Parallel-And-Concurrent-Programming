/*
This script demonstrates the use of Promise and callbacks in asynchronous programming. It compares the
asynchronous approach to a synchronous approach for computing square of numbers using slowSquare function.

In the asynchronous approach, we create two tasks for computing squares and then create two Promise objects.
We add a callback function 'squareCallback' to the Promise objects that will be called when the promise is resolved.

The main function waits for the results of the slowSquare function and resolves each promise, which in turn
triggers the callback functions. This demonstrates how Promise objects can be used for more advanced control flow and
how they can help decouple parts of your code.
*/

function slowSquare(x) {
  return new Promise((resolve) => {
    console.log(`Starting slow square computation for ${x}`);
    setTimeout(() => {
      console.log(`Finished slow square computation for ${x}`);
      resolve(x * x);
    }, 1000);
  });
}

function squareCallback(result) {
  console.log(`Square callback called with result: ${result}`);
}

async function main() {
  const startTime = Date.now();

  const promise1 = slowSquare(3);
  const promise2 = slowSquare(4);

  const result1 = await promise1;
  const result2 = await promise2;

  squareCallback(result1);
  squareCallback(result2);

  const endTime = Date.now();
  const duration = endTime - startTime;
  console.log(`Asynchronous approach time: ${duration}ms`);
}

main();
