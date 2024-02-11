console.log("Hello guys");
const fs = require('fs');
const { exit } = require('process');
fs.appendFile('new_file2', '', function (err) {
    if (err) throw err;
    console.log('Saved!');
  }); 
  exit(0);