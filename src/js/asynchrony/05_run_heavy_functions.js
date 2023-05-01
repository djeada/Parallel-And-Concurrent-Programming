const readline = require("readline");

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function heavyComputation(i) {
  console.log(`Running heavy computation ${i}`);
  await sleep(10000); // Simulate heavy computation using sleep
  console.log(`Done running heavy computation ${i}`);
  return i;
}

async function runBlockingFunctions() {
  console.log("Start runBlockingFunctions");
  const computationPromises = Array.from({ length: 6 }, (_, i) =>
    heavyComputation(i)
  );
  console.log("Waiting for heavy computations");
  const results = await Promise.all(computationPromises);
  console.log(`Results: ${results}`);
  console.log("End runBlockingFunctions");
}

async function main() {
  console.log("START main");

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on("line", (input) => {
    console.log(`Received input: ${input}`);
  });

  await runBlockingFunctions();

  console.log("Heavy computations completed. You can continue to enter text.");
}

main();
