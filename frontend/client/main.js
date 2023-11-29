const { app, BrowserWindow } = require('electron');

function createWindow() {
  const win = new BrowserWindow();
  win.loadURL('http://localhost:4000/');
}

app.whenReady().then(createWindow);