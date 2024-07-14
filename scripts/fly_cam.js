console.log('cube test')

var proxp = new Prop();
var cameras = new Camera();

SetMainCam(cameras)

var playera = getByName("TheCharacterController");
var maincamera = getCamByName("TheCharacterCamera");
maincamera.disabled = true;

mod_prop_texture(proxp._KE_Prop, 0, 0)


// function pressed(event){
//     //awawawa("meowmeowmeowmeomwoemwomeowmo")
//     console.log(event.which);
// }

// addEventListener("keypress",pressed)

var awadwa = 0;
var camverticalpan = 9;
//at some point this could be made to be hot loaded :3
function loop(delta){
    
    let asaf = playera.position;
    proxp.position =  new Vector3(playera.position.x,playera.position.y,playera.position.z);

    cameras.position = new Vector3(asaf.x+11,asaf.y+camverticalpan,asaf.z);

    cameras.lookat(new Vector3(asaf.x-cameras.position.x,asaf.y-cameras.position.y,asaf.z-cameras.position.z))

    awadwa+=0.01*delta

    var forword = 0;
    var side = 0;
    

    if(button["17"]) forword = -5;
    else if(button["31"]) forword = 5;

    if(button["30"]) side = 5;
    else if(button["32"]) side = -5;
    
    if(button["103"]) camverticalpan += 0.01;
    else if(button["108"]) camverticalpan += -0.01;

    forword*=delta;
    side*=delta;

    

    if(button["57"]) playera.velocity = new Vector3(forword,1.0,side);
    else playera.velocityOnlySideways(new Vector2(forword,side));

    if(forword || side) {
        if(side >= -0.1)
        proxp.lookat(new Vector3(-forword,0,side));
        else
        proxp.lookat(new Vector3(forword,0,side));
    }
}

addEventListener("tick",loop)


var button = {};

addEventListener("keypress", (keyevt) => {
    if(keyevt.how == "pressed")
        button[keyevt.which] = true
    else
        button[keyevt.which] = false
})