const { exit, argv } = require('process');
const fs = require('fs');
const file_num = argv[2];
const delay = argv[3];
console.log("PID", process.pid);
const data = fs.readFileSync("./splitted_arrays/" + file_num, 'utf-8'); // exit(1)
let data_lst = data.split(' ');
let result = 0;
data_lst.forEach((num) => { result += Number(num); });
fs.writeFileSync("./summed_arrays/" + file_num, String(result)); // exit(1)
setTimeout(() => {
    exit(0); 
}, 10000);