const { fork } = require('child_process');

function random(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

class Queue {
  constructor() {
    this.queue = [];
  }

  push(item) {
    this.queue.push(item);
  }

  pop() {
    if (this.queue.length === 0) return null;
    return this.queue.shift();
  }

  isEmpty() {
    return this.queue.length === 0;
  }
}

if (process.argv[2] === 'child') {
  const queue = new Queue();

  process.on('message', (message) => {
    if (message.type === 'produce') {
      const item = random(1, 10);
      console.log(`Child: Produced ${item}`);
      queue.push(item);
    } else if (message.type === 'consume') {
      if (!queue.isEmpty()) {
        const item = queue.pop();
        console.log(`Child: Sending ${item} to parent process`);
        process.send({ type: 'item', value: item });
      }
    }
  });
} else {
  const childProcess = fork(__filename, ['child'], { stdio: 'inherit' });

  for (let i = 0; i < 5; i++) {
    childProcess.send({ type: 'produce' });
  }

  setTimeout(() => {
    for (let i = 0; i < 5; i++) {
      childProcess.send({ type: 'consume' });
    }
  }, 1000);

  let itemsReceived = 0;
  childProcess.on('message', (message) => {
    if (message.type === 'item') {
      console.log(`Parent: Received ${message.value} from child process`);
      itemsReceived++;
      if (itemsReceived === 5) {
        childProcess.kill();
      }
    }
  });
}
