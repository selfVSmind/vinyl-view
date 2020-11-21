const testAddon = require('./build/Release/testaddon.node');
console.log('addon',testAddon);
console.log(testAddon.recordWavFile(5));

module.exports = testAddon;
