function f(x) {
    // if (x == true) return new Promise(resolve => { console.log("OK"); resolve(0); });
    if (x == true) return Promise.resolve(0);
    console.log("Run");
    return Promise.reject(1);
    // return new Promise(reject => { console.log("Not OK"); reject(1); });
}

// f(false).then((status) => { console.log("resolve", status); }, (status) => { console.log("reject", status); }).catch(() => {console.log("ERROR FCK ")});
const lib = require('node:process');
async function main() {
    try {
        let a = await f(false);
    } catch (err) {
        console.log("ERRER");
    }
    let a = await f(false);
    process.on("unhandledrejection", (reason, promise) => { console.log("Exit"); });
}

main();