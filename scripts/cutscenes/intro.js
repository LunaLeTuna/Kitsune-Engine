console.log('lift scene is a go!')


var lift = [];

for(let at = 0; at <= 7; at++) {
    var nowprop = getByName(`Platform${at}`);
    var awa = nowprop.position;
    lift.push({
        prop:nowprop,
        origin_pos:awa
    })
}

var lwhere = -200;

// function pressed(event){
//     //awawawa("meowmeowmeowmeomwoemwomeowmo")
//     console.log(event.which);
// }

// addEventListener("keypress",pressed)

var is_done = false;
var door_is_done = true;


//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");

    if(lift[0].origin_pos.y+lwhere >= 0.5 && !is_done){
        lwhere-=0.25*delta;

        lift.forEach(element => {
            element.prop.position = new Vector3(element.origin_pos.x,element.origin_pos.y+lwhere,element.origin_pos.z);
        });
    }else if(!is_done){
        lift[1].prop.position= new Vector3(99,99,99);
        lwhere=0.6500002;
        is_done=true;
        door_is_done=false
    }

    if(lwhere >= 0.0 && !door_is_done){
        lwhere-=0.05*delta;
        lift[6].prop.scale = new Vector3(lwhere,1,1);
        lift[7].prop.scale = new Vector3(lwhere,1,1);
    }else if(!door_is_done){
        door_is_done=true
    }
    
}

addEventListener("tick",loop)


"awa"