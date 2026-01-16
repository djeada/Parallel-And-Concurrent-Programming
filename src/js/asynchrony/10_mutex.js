/*
 * Mutex (Mutual Exclusion) Pattern for Async Operations
 * 
 * This script demonstrates the use of an async mutex to protect access to shared 
 * resources in an asynchronous environment. A mutex ensures only one task can 
 * access the critical section at a time.
 * 
 * Example: Bank account transfers where balance updates must be atomic.
 * 
 * Key concepts:
 * - Mutex implementation using Promises
 * - Critical section protection
 * - try/finally pattern for guaranteed release
 * - FIFO queue for fair access
 */

"use strict";

class Mutex {
  constructor() {
    this.locked = false;
    this.queue = [];
  }

  async acquire() {
    if (!this.locked) {
      this.locked = true;
      return;
    }

    return new Promise((resolve) => {
      this.queue.push(resolve);
    });
  }

  release() {
    if (this.queue.length > 0) {
      const resolve = this.queue.shift();
      resolve();
    } else {
      this.locked = false;
    }
  }

  get isLocked() {
    return this.locked;
  }

  get queueLength() {
    return this.queue.length;
  }
}

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class BankAccount {
  constructor(initialBalance, mutex) {
    this.balance = initialBalance;
    this.mutex = mutex;
    this.transactionLog = [];
  }

  async transfer(amount, description = "") {
    await this.mutex.acquire();
    
    try {
      const prevBalance = this.balance;
      console.log(`  [${description}] Starting transfer of ${amount}...`);
      
      // Simulate processing time (e.g., database operations)
      await sleep(100);
      
      this.balance += amount;
      
      this.transactionLog.push({
        amount,
        description,
        prevBalance,
        newBalance: this.balance,
        timestamp: new Date().toISOString(),
      });
      
      console.log(
        `  [${description}] Transfer complete: ${prevBalance} -> ${this.balance}`
      );
    } finally {
      this.mutex.release();
    }
  }

  getStatement() {
    return this.transactionLog;
  }
}

const transferSeries = async (account, transfers) => {
  for (const { amount, description } of transfers) {
    await account.transfer(amount, description);
  }
};

const main = async () => {
  console.log("=== Mutex Demo: Bank Account Transfers ===\n");

  const mutex = new Mutex();
  const account = new BankAccount(100, mutex);

  console.log(`Initial balance: $${account.balance}`);
  console.log("Starting concurrent transfers...\n");

  // Two series of transfers running concurrently
  const series1 = transferSeries(account, [
    { amount: 50, description: "Deposit-1" },
    { amount: -20, description: "Withdrawal-1" },
    { amount: 30, description: "Deposit-2" },
  ]);

  const series2 = transferSeries(account, [
    { amount: -10, description: "Fee-1" },
    { amount: 60, description: "Deposit-3" },
    { amount: -10, description: "Fee-2" },
  ]);

  await Promise.all([series1, series2]);

  console.log(`\n=== Final Results ===`);
  console.log(`Final balance: $${account.balance}`);
  console.log(`Expected: $${100 + 50 - 20 + 30 - 10 + 60 - 10} (all transactions applied)`);

  console.log("\nTransaction Log:");
  account.getStatement().forEach((tx, i) => {
    console.log(
      `  ${i + 1}. [${tx.description}] ${tx.amount >= 0 ? "+" : ""}${tx.amount} ` +
      `(${tx.prevBalance} -> ${tx.newBalance})`
    );
  });
};

main().catch(console.error);
