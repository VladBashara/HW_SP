// let promise = new Promise(function(resolve, reject) {
//     console.log("Make Promise");
//     setTimeout(() => { reject(new Error("adads")); }, 10);
//     setTimeout(() => { console.log("promise.status: " + this.status); }, 10000);
//     setTimeout(() => { console.log("promise.result: " + this.result); }, 1000);
// });
// promise.then(
//     function() { console.log("Make error "); }
// ).catch(function() { console.log("End"); });

// let res = Promise.allSettled([
//     new Promise(function(resolve, reject) { resolve('100'); }),
//     new Promise(function(resolve, reject) { reject(new Error('400')); }),
// ]);

// res.then((results) => { results.forEach(result => { console.log("result.status " + result.status);
//                                                     if (result.status == "fulfilled") {
//                                                         console.log("result.value " + result.value);
//                                                     } else {
//                                                         console.log("result.reason " + result.reason);
//                                                     } }); });

// let promise = Promise.resolve();
// promise.then(() => {
//     console.log("First then"); setTimeout(()=>{}, 100); 
// });
// promise.then(setTimeout(()=>{console.log("Second then")}, 0));
// promise.then(console.log("Fourth then"));
// promise.then(() => {
//     console.log("Third then"); setTimeout(()=>{}, 0); 
// });
// console.log("EXIT");

// const lib = require("process");
// function f() {
//     return Promise.reject();
// }

// f().then(() => {console.log("NONONOON");})
// process.on("uncaughtException", () => {console.log("KEK");});


function wait_subprocesses() {
    return Promise.resolve("99");
}

let ggg = wait_subprocesses();
ggg.then(res => {console.log("res ", res)});