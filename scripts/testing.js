console.log('Hello World from a JS code string!')

var myid = "";
var myBody = getByName("TheCharacterController");

var awa = new Prop();



class player {
    constructor(username, id, leprop) {
        this.username = username;
        this.id = id;
        this.position = new Vector3(0,5,0);
        this.body = leprop;
    }
}


var players = {};




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
    nya = {"type":"pig_update","pos":new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20)}

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
    if(delta.type == "join"){ //serverside, when player joins

        var newBody = new Prop();

        players[delta.id] = new player(delta.id, delta.id, newBody);

        console.log(`player ${delta.id} has joined!`);

        emit({"to":delta.id,"data":{"type":"preparing new player", "your id": delta.id, "current players": players}});
        emit({"type":"new player!","id":delta.id});
    }
    
    if(delta.type == "update"){
        console.log(delta.data.type, delta.fromid);

        //player updates
        if(delta.fromid == myid){return}

        if(delta.data.type == "player pos update"){
            players[delta.fromid].body.position = delta.data.data.newpos;
        }

        //server only things
        if(!delta.ishost){return}

        //give new player the scoope
        if(delta.data.type == "preparing new player"){
            myid = delta.data["your id"];

            for( const newplayer in delta.data["current players"]) {
                var newBody = new Prop();

                players[newplayer.id] = new player(newplayer.id, newplayer.id, newBody);
            }

            return
        }

        //clientside, when player joins
        if(delta.data.type == "new player!"){
            if(delta.ishost || delta.data.id == myid){return}

            var newBody = new Prop();

            players[delta.id] = new player(delta.id, delta.id, newBody);

            console.log(`player ${delta.data.id} has joined!`);
            return
        }

        if(delta.data.type == "pig_update"){
            awa.position = delta.data.pos;
            return
        }
        
    }
}

addEventListener("client_update",client_update)


//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log(JSON.stringify(myBody.position));
    emit({"type": "player pos update", "data":{"newpos":myBody.position}})

    // console.log(JSON.stringify({pos:awa.position}))
}

addEventListener("tick",loop);


"awa"