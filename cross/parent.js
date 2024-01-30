console.log("Hello guys");
let fs = require('fs');
fs.appendFile('new_file', '', function (err) {
    if (err) throw err;
    console.log('Saved!');
  }); 