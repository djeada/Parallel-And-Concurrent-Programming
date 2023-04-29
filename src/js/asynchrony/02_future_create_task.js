const { performance } = require('perf_hooks');

function slowSquareSync(x) {
    console.log(`Starting slow square computation for ${x}`);
    const startTime = new Date();
    while (new Date() - startTime < 2000) {}
    const result = x * x;
    console.log(`Finished slow square computation for ${x}`);
    return result;
}

async function slowSquareAsync(x) {
    console.log(`Starting slow square computation for ${x}`);
    await new Promise((resolve) => setTimeout(resolve, 2000));
    const result = x * x;
    console.log(`Finished slow square computation for ${x}`);
    return result;
}

function synchronousExecution() {
    const start_time = performance.now();

    const result1 = slowSquareSync(3);
    const result2 = slowSquareSync(4);

    const elapsed_time = performance.now() - start_time;
    console.log(`\nSynchronous execution took ${elapsed_time / 1000} seconds.`);
    console.log(`Results: ${result1}, ${result2}`);
}

async function asynchronousExecution() {
    const start_time = performance.now();

    const promise1 = slowSquareAsync(3);
    const promise2 = slowSquareAsync(4);

    const results = await Promise.all([promise1, promise2]);

    const elapsed_time = performance.now() - start_time;
    console.log(`\nAsynchronous execution took ${elapsed_time / 1000} seconds.`);
    console.log(`Results: ${results}`);
}

synchronousExecution();
asynchronousExecution();
