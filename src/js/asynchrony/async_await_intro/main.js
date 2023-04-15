async function fun1() {
  console.log("hello from fun1");
  return 1;
}

async function fun2() {
  // await makes the program execute the next lines after the function call
  const result = await fun1();
  console.log("hello from fun2: " + result);
}

function main() {
  console.log("Begin main.");
  fun2();
  console.log("End main.");
}

main();
