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

  const mainMenu = electron.Menu.buildFromTemplate(mainMenuTemplate);
  electron.Menu.setApplicationMenu(mainMenu);

  mainWindow.loadURL(url.format({
  	pathname: path.join(__dirname, 'html/index.html'),
  	protocol: 'file',
  	slashes: true
  }));
});

const mainMenuTemplate = [
  {
    label:'File',
    submenu: [
      {
        label: 'Set Capture Device',
        click() {
          const { exec } = require("child_process");
          exec("pavucontrol --tab=4");
        }
      }
    ]
  },
  {
    label: 'Console',
    accelerator: 'Ctrl+Shift+I',
    click(item, focusedWindow) {
      focusedWindow.toggleDevTools();
    },
  }
];
