console.log('Hello World from a JS code string!')

var awa = new Prop();

var whatrprop = getByName("thisisbad")

console.log(JSON.stringify(whatrprop.position))

let aaa = 0;

// for multiplayer
function server_start(){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    console.log("meow");

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("server_start",server_start)


// multiplayer loop
function server_loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");
    // awa.position = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);

    // aaa+=(0.1*delta)%(Math.PI*2);

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("server_tick",server_loop)



//at some point this could be made to be hot loaded :3
function client_update(delta){
    console.log(delta)
}

addEventListener("client_update",client_update)


//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");
    awa.position = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);

    aaa+=(0.1*delta)%(Math.PI*2);

    emit({"playerpoid":1})

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("tick",loop)


"awa"