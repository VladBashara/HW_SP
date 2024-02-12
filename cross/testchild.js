const { exit, argv } = require('process');
const cluster = require('cluster');
const fs = require('fs');
console.log("Child PID: " + process.pid);
console.log(argv[2], argv[3]);
const file_num = argv[2];
const delay = argv[3];
console.log("./splitted_arrays/" + file_num);
const data = fs.readFileSync("./splitted_arrays/" + file_num, 'utf-8'); // exit(1)
let data_lst = data.split(' ');
let result = 0;
data_lst.forEach((num) => { result += Number(num); });
fs.writeFileSync("./summed_arrays/" + file_num, String(result)); // exit(1)
exit(0);