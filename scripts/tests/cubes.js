console.log('cube test')

var awa = new Prop();

var aaa = 0;

// function pressed(event){
//     //awawawa("meowmeowmeowmeomwoemwomeowmo")
//     console.log(event.which);
// }

// addEventListener("keypress",pressed)


//at some point this could be made to be hot loaded :3
function loop(delta){
    //awawawa("meowmeowmeowmeomwoemwomeowmo")
    //console.log("meow");

    aaa+=(0.001)%(Math.PI*2);

    awa.position = new Vector3(Math.sin(aaa)*20,4,Math.cos(aaa)*20);
}

addEventListener("tick",loop)