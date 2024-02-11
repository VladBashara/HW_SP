function printSHIT(x) {
    console.log("SHIT", x);
}

const { exit, argv } = require('process');
const child = require('child_process');
const cluster = require('cluster');
const fs = require('fs');
const filename = argv[2];
const N = argv[3];
let promise = new Promise(function(resolve, reject) {
    fs.readFile("./array", 'utf-8', (err, data) => {
        console.log(data);
        child.execSync('sleep 1');
        console.log("Lol");
        // exit(0);
        resolve('done');
        })
});
promise.finally( function() { console.log("CONSOLE"); } ).finally( function() { console.log("CONSOLE"); } );
child.execSync('sleep 2');
console.log("EXIT");