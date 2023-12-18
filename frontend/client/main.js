const { app, BrowserWindow } = require('electron');

function createWindow() {
  const win = new BrowserWindow({
    fullscreen: true, // 设置默认全屏
    frame: false, // 隐藏导航栏
  });

  win.loadURL('http://bookstore.zymsite.ink/');

  // 监听按键事件
  win.webContents.on('before-input-event', (event, input) => {
    if (input.key === 'Escape') {
      app.quit(); // 按下Esc键时退出程序
    }
  });
}

app.whenReady().then(createWindow);