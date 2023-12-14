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

var nya = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);

// multiplayer loop
function server_loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");
    nya = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);

    aaa+=(0.001)%(Math.PI*2);
    dispatchEvent("client_update", nya)
    serverEmit(nya)
    // awa.position = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);

    // aaa+=(0.1*delta)%(Math.PI*2);

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("server_tick",server_loop)



//at some point this could be made to be hot loaded :3
function client_update(delta){
    if(delta.type == "join"){ //for server
        console.log(JSON.stringify(delta))
        emit({"to":delta.id,"data":"meow"})
        emit({"type":"new player!","id":delta.id})
    }
    
    if(delta.type == "update"){
        if(delta.data.type == "new player!"){
            console.log(`player ${delta.data.id} has joined!`);
            return
        }
        awa.position = delta.data
    }
}

addEventListener("client_update",client_update)


//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("tick",loop)


"awa"