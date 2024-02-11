const { exit, argv } = require('process');
const cluster = require('cluster');
console.log("Child PID: " + process.pid);
console.log(argv[2], argv[3]);
ReadFromFile(); // exit(1)
Sum(); // exit(1)
WriteToFile(); // exit(1)
exit(0);