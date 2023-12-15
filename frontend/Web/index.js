const express = require('express');
const { createServer } = require('node:http');
const { join } = require('node:path');
const { Server } = require('socket.io');

const app = express();
const server = createServer(app);
const io = new Server(server);

const message_map=new Map();
const { spawn } = require('child_process');
const backend=spawn(join(__dirname, '../../build/code'),['-s','-c',process.argv.slice(2)[0]]);

const AsyncLock = require('async-lock');
const lock = new AsyncLock();

async function SendRequest(data) {
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

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function GetResult(session_token,operation_token) {
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
    await sleep(100);
  }
}

let buffer = '';
let to_be_collect=0;
let res_block="";
backend.stdout.on('data', (data) => {
  console.log(`stdout: ${data}`);
  buffer += data.toString();
  while (buffer.includes('\n')) {
      const lineEnd = buffer.indexOf('\n');
      const line = buffer.slice(0, lineEnd);
      buffer = buffer.slice(lineEnd + 1);
      console.log(`C++ Program Output: ${line}`);
      if(to_be_collect>0)
      {
        res_block+=line+'\n';
        to_be_collect--;
        if(to_be_collect==0)
        {
          const substrings = res_block.trim().split('\n')[0].split(' ');
          const session_token=substrings[0];
          const operation_token=substrings[1];
          console.log("session_token: "+session_token);
          console.log("operation_token: "+operation_token);
          console.log("ret: "+res_block);
          if(!message_map.has(session_token)) message_map.set(session_token, new Map());
          message_map.get(session_token).set(operation_token,res_block);
          res_block="";
        }
      }
      else{
        res_block=line+'\n';
        to_be_collect=parseInt(line.split(' ')[2]);
        if(to_be_collect<=0)
        {
          const substrings = res_block.trim().split('\n')[0].split(' ');
          const session_token=substrings[0];
          const operation_token=substrings[1];
          console.log("session_token: "+session_token);
          console.log("operation_token: "+operation_token);
          console.log("ret: "+res_block);
          if(!message_map.has(session_token)) message_map.set(session_token, new Map());
          message_map.get(session_token).set(operation_token,res_block);
          res_block="";
        }
      }
  }
});
// SendRequest("#OpenSession InnerTest")
app.get('/', (req, res) => {
  res.sendFile(join(__dirname, 'index.html'));
});
app.get('/communication.js', (req, res) => {
  res.sendFile(join(__dirname, 'communication.js'));
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

backend.on('exit', (code, signal) => {
  if (code !== null) {
    console.log(`子进程退出，退出码: ${code}`);
  } else if (signal !== null) {
    console.log(`子进程被信号中断，信号: ${signal}`);
  } else {
    console.log('子进程退出');
  }
  process.exit(0);
});

server.listen(3000, () => {
  console.log('server running at http://localhost:3000');
});