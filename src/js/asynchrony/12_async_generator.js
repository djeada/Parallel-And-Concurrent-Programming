async function* asyncGenerator() {
    const delay = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

    for (let i = 0; i < 5; i++) {
        await delay(Math.random() * 1000 + 500);
        yield i;
    }
}

(async () => {
    for await (const value of asyncGenerator()) {
        console.log("Received value:", value);
    }
})();
