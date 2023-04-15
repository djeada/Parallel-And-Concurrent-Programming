let counter = 1;

function foo(multiplier) {
  for (let i = 0; i < 10; i++) {
    setTimeout(() => {
      const oldValue = counter;
      const newValue = oldValue * multiplier;
      console.log(`The counter gets multiplied by ${multiplier}`);
      counter = newValue;
    }, i * 10 + Math.random() * 100);
  }
}

for (let i = 0; i < 3; i++) {
  foo(i + 1);
}

// Wait for all tasks to finish, then print the final value of counter
setTimeout(() => {
  console.log(`The final value of counter is: ${counter}`);
}, 2000);
