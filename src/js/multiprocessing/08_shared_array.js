const { fork } = require('child_process');

if (process.argv[2] === 'child') {
  process.on('message', (message) => {
    if (message.action === 'increment') {
      process.send({ action: 'increment' });
    } else if (message.action === 'decrement') {
      process.send({ action: 'decrement' });
    }
  });
} else {
  const child1 = fork(__filename, ['child']);
  const child2 = fork(__filename, ['child']);

  let sharedArray = [0, 0, 0];

  child1.on('message', (message) => {
    if (message.action === 'increment') {
      sharedArray = sharedArray.map((value) => value + 1);
      console.log(`Shared array incremented by child1: ${sharedArray}`);
      setTimeout(() => {
        child1.send({ action: 'increment' });
      }, 1000);
    }
  });

  child2.on('message', (message) => {
    if (message.action === 'decrement') {
      sharedArray = sharedArray.map((value) => value - 1);
      console.log(`Shared array decremented by child2: ${sharedArray}`);
      setTimeout(() => {
        child2.send({ action: 'decrement' });
      }, 1000);
    }
  });

  child1.send({ action: 'increment' });
  child2.send({ action: 'decrement' });
}
