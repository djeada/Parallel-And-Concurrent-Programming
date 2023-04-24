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

  let sharedValue = 0;

  child1.on('message', (message) => {
    if (message.action === 'increment') {
      sharedValue++;
      console.log(`Shared value incremented by child1: ${sharedValue}`);
      setTimeout(() => {
        child1.send({ action: 'increment' });
      }, 1000);
    }
  });

  child2.on('message', (message) => {
    if (message.action === 'decrement') {
      sharedValue--;
      console.log(`Shared value decremented by child2: ${sharedValue}`);
      setTimeout(() => {
        child2.send({ action: 'decrement' });
      }, 1000);
    }
  });

  child1.send({ action: 'increment' });
  child2.send({ action: 'decrement' });
}
