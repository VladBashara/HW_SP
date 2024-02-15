function calc() {
}

function div(M, N) {
    return (M - (M % N))/N;
}

function write_numbers_to_file(path, data_lst) {
    fs.writeFileSync(path, '');
    for (let i = 0; i < data_lst.length-1; i++) {
            fs.appendFileSync(path, data_lst[i] + ' ');
    }
    fs.appendFileSync(path, data_lst[data_lst.length-1]);
}

function call_subprocesses(N, delay) {
    let child_proc = Array(N);
    for (let i = 0; i < N; i++) {
        child_proc[i] = child.fork("./testchild.js", [i+1, delay]);
        // console.log("PROC " + child_proc[i].pid);
    }
    return child_proc;
}

async function wait_subprocesses(N, child_proc) {
    // return Promise.resolve("99");
//     Promise.all(child_proc.map(x => {console.log("PRINTITN");
//     x.on("exit", () => {console.log("SUPER"); Promise.resolve("resolved");});
// }
//     )).then((z) => {console.log("return Promise.resolve('') ", z); return Promise.resolve('21');},
//             () => {return Promise.resolve('22'); throw new Error("ERROR 32");}
//         ).catch(() => {console.log("ERROR 33");}).finally(() => {return Promise.resolve('end')});
    // return child_proc.map( x => {console.log("PRINTITN ", x.pid);
    // x.on("exit", () => {console.log("SUPER"); return Promise.resolve("resolved");})});
    // let smth = await child_proc.map(async function(x) {console.log("PRINTITN ", x.pid);
    // let counter = 0;
    // let param = await x.on("exit", () => {console.log("PRINTITN2 "); counter = 1;});
    // if (counter == 1) { console.log("SUPER"); return Promise.resolve("resolved"); }
    // });
    // console.log("log ", smth);
    // return smth;
    let counter = 0;
    x.on("exit", () => {console.log("PRINTITN2 "); counter++;});
    if (counter== N) { return Promise.resolve(1);}
}

// function wait_subprocesses(N, child_proc) {
//     let counter = 0;
//     console.log("\nHERE\n");
//     for (let i = 0; i<N; i++) {
//         child_proc[i].on("exit", (code, signal) => {
//             if (code == 0) {
//                 counter++
//             } else { return Promise.reject('bad'); }
//             if (counter == N) {
//                 return Promise.resolve('good');
//             }
//         })
//     }
//     // return Promise.reject('bad');
// }
// Promise.all(child_proc.map(x => {x.on("exit", (code) => {
//     if (code == 0) { resolve(''); } else { reject(''); }
// })})).then().catch();
function summation(N) {
    let folder = 'summed_arrays/';
    let result = 0;
    for (let i = 0; i < N; i++) {
        let buf = fs.readFileSync(folder + (i+1), 'utf-8');
        result += Number(buf);
    }
    console.log("Result = " + result + "\n");
}

const { exit, argv } = require('process');
const child = require('child_process');
const cluster = require('cluster');
const fs = require('fs');
const { resolve } = require('path');
const { rejects } = require('assert');
const filename = argv[2];
let N = argv[3];
// let promise = new Promise(function(resolve, reject) {
//     fs.readFile("./array", 'utf-8', (err, data) => {
//         console.log(data);
//         child.execSync('sleep 1');
//         console.log("Lol");
//         // exit(0);
//         resolve('done');
//         })
// });
// promise.finally( function() { console.log("CONSOLE"); } ).finally( function() { console.log("CONSOLE"); } );
console.log();
const data = fs.readFileSync("./array", 'utf-8');
let data_lst = data.split(' ');
// console.log("data: " + data);

if (data_lst[data_lst.length-1] == '') {
    data_lst.splice(data_lst.length-1, 1);
}
let M = data_lst.length;

if (N > M/2) {
    N = div(M , 2);
    console.log("Warning: N = M//2\n");
}
let M_prcs = div(M , N);
let M_last_prcs = M - (N-1)*div(M , N);

console.log("M - data: " + M);
console.log("M_prcs: " + M_prcs);
console.log("M_last_prcs: " + M_last_prcs);
console.log("N - subproccesses: " + N);
// console.log("data_lst.length: ", data_lst.length);

// fs.writeFileSync(`./splitted_arrays/${N}`, 'kek');
// console.log(data_lst);
// console.log(data_lst.slice(0, 2));
let i = 0;
for (i; i < N-1; i++) {
    write_numbers_to_file(`./splitted_arrays/${i+1}`, data_lst.slice(i*M_prcs, (i+1)*M_prcs));
}
write_numbers_to_file(`./splitted_arrays/${N}`, data_lst.slice(i*M_prcs, data_lst.length));

let child_proc = call_subprocesses(N, 1);
// let ggg = wait_subprocesses(N, child_proc);
// ggg.then(res => {console.log("res ", res)});
// let smth = await child_proc.map(x => {console.log("PRINTITN ", x.pid);
// x.on("exit", () => {console.log("SUPER"); return Promise.resolve("resolved");});
// // return Promise.resolve("resolved");
// });
// let smth = wait_subprocesses(N, child_proc);
// let smth = child_proc.map( x => {console.log("PRINTITN ", x.pid);
// let counter = 0;
// x.on("exit", () => {counter = 1;})
// if (counter == 1) { console.log("SUPER"); return Promise.resolve("resolved"); }
// });
// let smth = wait_subprocesses(N, child_proc);
// console.log("asdasd ", smth);
// let smth = [
//     new Promise(resolve => setTimeout(() => resolve(1), 3000))
//   ];
let counter = 0;
child_proc.forEach(x => {
    x.on("exit", () => {
    counter++;
    if (counter == N) {
        summation(N);
    }});
})
// Promise.all(smth).then(smth => {console.log("smth ", smth)});
// Promise.all(smth).then(() => {console.log("YEEAH");}).catch((error) => {console.log("ERROR ", error)});