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
    }
    return child_proc;
}

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
const fs = require('fs');
const filename = argv[2];
let N = argv[3];
const delay = argv[4];
console.log("PID", process.pid);

console.log();
let data = "somedata";
try {
    data = fs.readFileSync(filename, 'utf-8');
} catch (err) {
    console.log("Can`t open ", filename);
    exit(1);
}
let data_lst = data.split(' ');


if (data_lst[data_lst.length-1] == '') {
    data_lst.splice(data_lst.length-1, 1);
}
let M = data_lst.length;

if (M < 2) {
    console.log("ERROR: M < 2");
    exit(1);
}

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

let i = 0;
for (i; i < N-1; i++) {
    write_numbers_to_file(`./splitted_arrays/${i+1}`, data_lst.slice(i*M_prcs, (i+1)*M_prcs));
}
write_numbers_to_file(`./splitted_arrays/${N}`, data_lst.slice(i*M_prcs, data_lst.length));

let child_proc = call_subprocesses(N, delay);

let counter = 0;
child_proc.forEach(x => {
    x.on("exit", () => {
    counter++;
    if (counter == N) {
        setTimeout(()=>{summation(N);}, 10000);
    }});
})