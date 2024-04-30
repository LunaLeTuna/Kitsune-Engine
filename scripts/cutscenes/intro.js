console.log('lift scene is a go!')


var lift = [];

//get all the props named Platform#
for(let at = 0; at <= 10; at++) {
    var nowprop = getByName(`Platform${at}`);
    var awa = nowprop.position;
    lift.push({
        prop:nowprop,
        origin_pos:awa
    })
}
var liflight = new Light();

var lwhere = 0;

// function pressed(event){
//     //awawawa("meowmeowmeowmeomwoemwomeowmo")
//     console.log(event.which);
// }

// addEventListener("keypress",pressed)

var is_done = false;
var door_is_done = false;

let ro = lift[10].prop.rotation.y

//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");

    delta = delta*0.1

    if(lift[0].origin_pos.y+lwhere >= 0.5 && !is_done){
        lwhere-=0.5*delta;

        lift.forEach(element => {
            element.prop.position = new Vector3(element.origin_pos.x,element.origin_pos.y+lwhere,element.origin_pos.z);
        });
        liflight.position = new Vector3(lift[0].origin_pos.x,lift[0].origin_pos.y+lwhere+10,lift[0].origin_pos.z);

        lift[9].prop.rotation = new Vector3((-lwhere*Math.PI/180.0)*15,ro,0);
        lift[10].prop.rotation = new Vector3((lwhere*Math.PI/180.0)*23,ro,0);
    }else if(!is_done){
        lift[1].prop.position= new Vector3(99,99,99);
        lwhere=1;
        is_done=true;
        door_is_done=false
    }

    if(lwhere >= 0.3 && !door_is_done){
        lwhere-=0.01*delta;
        lift[6].prop.scale = new Vector3(lwhere,1,1);
        lift[7].prop.scale = new Vector3(lwhere,1,1);
    }else if(!door_is_done){
        door_is_done=true
    }
    
    
}

addEventListener("tick",loop)


"awa"