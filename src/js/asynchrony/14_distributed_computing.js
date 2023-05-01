const net = require('net');
const readline = require('readline');
const { ArgumentParser } = require('argparse');

class TaskServer {
  constructor(host, port) {
    this.host = host;
    this.port = port;
  }

  dispatchTasks() {
    const server = net.createServer((socket) => {
      setInterval(() => {
        const task = Math.floor(Math.random() * 100) + 1;
        console.log(`Dispatching task: ${task}`);
        socket.write(`task ${task}\n`);
      }, 1000);
    });

    server.listen(this.port, this.host);
    console.log(`Task server listening on ${this.host}:${this.port}`);
  }
}

class Worker {
  async performTask(host, port) {
    const client = net.createConnection({ host, port }, async () => {
      const rl = readline.createInterface({ input: client });

      rl.on('line', async (line) => {
        const task = parseInt(line.split(' ')[1], 10);
        console.log(`Performing task: ${task}`);
        await sleep(Math.random() * 1500 + 500);
        const result = task * 2;
        console.log(`Completed task: ${task}, result: ${result}`);
      });
    });
  }
}

class Client {
  async receiveResults(host, port) {
    const client = net.createConnection({ host, port }, async () => {
      const rl = readline.createInterface({ input: client });

      rl.on('line', (line) => {
        const result = parseInt(line.split(' ')[1], 10);
        console.log(`Client received result: ${result}`);
      });
    });
  }
}

async function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function main(role) {
  const host = 'localhost';
  const port = 8888;

  if (role === 'server') {
    const taskServer = new TaskServer(host, port);
    taskServer.dispatchTasks();
  } else if (role === 'worker') {
    const worker = new Worker();
    await worker.performTask(host, port);
  } else if (role === 'client') {
    const client = new Client();
    await client.receiveResults(host, port);
  }
}

const parser = new ArgumentParser();
parser.add_argument('role', { choices: ['server', 'worker', 'client'] });
const args = parser.parse_args();

main(args.role);
