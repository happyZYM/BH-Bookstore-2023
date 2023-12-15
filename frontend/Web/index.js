const express = require('express');
const { createServer } = require('node:http');
const { join } = require('node:path');
const { Server } = require('socket.io');

const app = express();
const server = createServer(app);
const io = new Server(server);
class Queue {
  constructor() {
    this.items = [];
  }

  // 添加元素到队列的尾部
  enqueue(element) {
    this.items.push(element);
  }

  // 从队列的头部移除元素并返回移除的元素
  dequeue() {
    if (this.isEmpty()) {
      return null;
    }
    return this.items.shift();
  }

  // 返回队列的头部元素
  front() {
    if (this.isEmpty()) {
      return null;
    }
    return this.items[0];
  }

  // 检查队列是否为空
  isEmpty() {
    return this.items.length === 0;
  }

  // 返回队列的大小
  size() {
    return this.items.length;
  }

  // 清空队列
  clear() {
    this.items = [];
  }
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

const message_map=new Map();
const { spawn } = require('child_process');
const backend=spawn('/workspaces/BH-Bookstore-2023/build/code',['-s','-c','/tmp/conf/']);

const AsyncLock = require('async-lock');
const lock = new AsyncLock();

function SendRequest(data) {
  console.log("SendRequest called");
  return new Promise((resolve, reject) => {
    lock.acquire('myLock', (done) => {
      console.log("SendRequest acquired lock");
      console.log("sending data to backend: "+data)
      backend.stdin.write(data+'\n', (error) => {
        done();
        if (error) {
          reject(error);
        } else {
          resolve();
        }
      });
    });
  });
}

function GetResult(session_token,operation_token) {
  console.log("GetResult called");
  while(true)
  {
    if(message_map.has(session_token))
    {
      if(message_map.get(session_token).has(operation_token))
      {
        const ret=message_map.get(session_token).get(operation_token);
        message_map.get(session_token).delete(operation_token);
        return ret;
      }
    }
    sleep(100);
  }
}
let buffer = '';
const response_queue=new Queue();
backend.stdout.on('data', (data) => {
  console.log(`stdout: ${data}`);
  buffer += data.toString();
  while (buffer.includes('\n')) {
      const lineEnd = buffer.indexOf('\n');
      const line = buffer.slice(0, lineEnd);
      buffer = buffer.slice(lineEnd + 1);
      console.log(`C++ Program Output: ${line}`);
      response_queue.enqueue(line);
  }
});
// SendRequest("#OpenSession InnerTest")
app.get('/', (req, res) => {
  res.sendFile(join(__dirname, 'index.html'));
});

io.on('connection', async (socket) => {
  console.log('a user connected');
  socket.on('disconnect', () => {
    console.log('user disconnected');
  });
  socket.on('request', async (msg) => {
    console.log('message: ' + msg);
    const substrings = msg.trim().split('\n')[0].split(' ');
    const head=substrings[0];
    if(head[1]=='S')
    {
      backend.stdin.write("#ShutDownSystem\n");
      sleep(1000);
      process.exit(0);
    }
    const session_token=substrings[1];
    if(head[1]=='O')
    {
      SendRequest(msg);
      ret=await GetResult(session_token,"Init");
      console.log("ret: "+ret);
      socket.emit('response', ret);
    }
    else{
      const operation_token=substrings[2];
      const outhentication_key=substrings[3];
      const command=msg.trim().split('\n')[1];
      console.log('head: ' + head);
      console.log('session_token: ' + session_token);
      console.log('operation_token: ' + operation_token);
      console.log('outhentication_key: ' + outhentication_key);
      console.log('command: ' + command);
      console.log("sending request to backend")
      SendRequest(msg);
      if(!message_map.has(session_token)) message_map.set(session_token, new Map());
      ret=await GetResult(session_token,operation_token);
      console.log("ret: "+ret);
      socket.emit('response', ret);
    }
  });
});

server.listen(3000, () => {
  console.log('server running at http://localhost:3000');
});