const { spawn } = require("child_process");
const { Readable } = require("stream");

function random(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

if (process.argv[2] === "child") {
  const readable = new Readable({
    read(size) {},
  });

  for (let i = 0; i < 5; i++) {
    const item = random(1, 10);
    setTimeout(() => {
      readable.push(`${item}\n`);
    }, random(500, 1500));
  }

  setTimeout(() => {
    readable.push(null);
  }, 3000);

  readable.pipe(process.stdout);
} else {
  const childProcess = spawn(process.argv[0], [__filename, "child"], {
    stdio: [null, "pipe", null],
  });

  childProcess.stdout.setEncoding("utf8");
  childProcess.stdout.on("data", (chunk) => {
    console.log(`Parent: Received ${chunk.trim()} from child process`);
  });

  childProcess.on("exit", () => {
    console.log("Child process finished");
  });
}
