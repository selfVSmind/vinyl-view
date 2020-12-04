const electron = require('electron');
const url = require('url');
const path = require('path');

let mainWindow;

electron.app.on('ready', function() {
  mainWindow = new electron.BrowserWindow({
    webPreferences: {
      contextIsolation: false,
      preload: path.join(electron.app.getAppPath(), 'preload.js')
    }
  });

  let name = require("./package.json").name;
  let version = require("./package.json").version;

  mainWindow.maximize();
  mainWindow.setTitle(name + " " + version);

  mainWindow.loadURL(url.format({
  	pathname: path.join(__dirname, 'html/index.html'),
  	protocol: 'file',
  	slashes: true
  }));
});
