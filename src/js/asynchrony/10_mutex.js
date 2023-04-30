/*
This script demonstrates the use of async-lock (a.k.a. async mutex) to protect
access to shared resources in an asynchronous environment. In this example, we
simulate a bank account with two async functions attempting to transfer money concurrently.
The use of an async mutex (lock) is crucial to prevent race conditions that could lead
to incorrect account balances.
*/

const AsyncLock = require('async-lock');
const lock = new AsyncLock();

class BankAccount {
  constructor(balance) {
    this.balance = balance;
  }

  async transfer(amount) {
    await lock.acquire('balance', async () => {
      console.log(`Transferring ${amount}...`);
      await new Promise((resolve) => setTimeout(resolve, 100)); // Simulate some processing time
      this.balance += amount;
      console.log(`Transfer of ${amount} complete. New balance: ${this.balance}`);
    });
  }
}

async function transferMoney(account, amounts) {
  for (const amount of amounts) {
    await account.transfer(amount);
  }
}

async function main() {
  const account = new BankAccount(100);

  const transfer1 = transferMoney(account, [50, -20, 30]);
  const transfer2 = transferMoney(account, [-10, 60, -10]);

  await Promise.all([transfer1, transfer2]);

  console.log(`Final account balance: ${account.balance}`);
}

main();
