var maincamera = getCamByName("TheCharacterCamera");
var mainchar = getByName("TheCharacterController");
var portal_geo = new Prop();
var portal_mes = new Model("portal_doors/base_portal.obj");
var portal_tex = new Texture("");
var portal_cam = new Camera();


// var meowmedaodiskdas = new Prop();
// mod_prop_texture(meowmedaodiskdas._KE_Prop, 0, 0)
// mod_prop_shader(meowmedaodiskdas._KE_Prop, 2)


var house = new Prop();
var house_mesh = new Model("portal_doors/base_frame.obj");
house.model = house_mesh
house.position = new Vector3(0,0.501,0);
mod_prop_shader(house._KE_Prop, 0)

var door = new Prop();
var door_mesh = new Model("portal_doors/base_door.obj");
door.model = door_mesh
door.position = new Vector3(0,0.501,1.8);
mod_prop_shader(door._KE_Prop, 0)

function modor(vvv) {
    door.position = new Vector3(0,0.501,vvv);
}


portal_geo.texture = portal_tex;
tepter(portal_tex.ID, 0)
portal_cam.buffer_write2 = 1;
portal_cam.disabled = true

//portal_geo.scale = new Vector3(0.2,4,3)
portal_geo.position = new Vector3(0,0.501,0)
mod_prop_shader(portal_geo._KE_Prop, 6)
mod_prop_model(portal_geo._KE_Prop, portal_mes.ID)
mod_prop_texture(portal_geo._KE_Prop, 0, portal_tex.ID)
portal_geo.backfacing = false
portal_geo.trans = 0.9

var is_door_open = false;

function loop(delta){
    
    let mrow = maincamera.position;
    portal_cam.position = new Vector3(mrow.x,mrow.y,mrow.z-60)

    // let asaf = mainchar.position;
    // meowmedaodiskdas.position = new Vector3(asaf.position.x,asaf.position.y,asaf.position.z)

    var rcamrot = maincamera.rotation;
    portal_cam.rotation = new Vector3(rcamrot.x,rcamrot.y,rcamrot.z)

    
    //portal_cam.lookat(new Vector3(asaf.x-cameras.position.x,asaf.y-cameras.position.y,asaf.z-cameras.position.z))
}

addEventListener("tick",loop)

var camverticalpan = 0;
var cam_pan_speed = 0.04
var cam_pan_done = false;


addEventListener("keypress", (keyevt) => {
    if(is_door_open) return

    if(keyevt.which == 18 && keyevt.how == "released"){

        var pa1 = mainchar.position;
        var pa2 = portal_geo.position;
        let distenc = Math.sqrt((Math.abs(pa1.x-pa2.x)+Math.abs(pa1.y-pa2.y)+Math.abs(pa1.z-pa2.z)))
        //console.log(distenc);
        if(distenc < 3.5){
            is_door_open=true

            //load portal
            create_world(1)
            load_map("burgor.kbf",1)
            portal_cam.world = 1
            portal_cam.disabled = false

            console.log("open sesamee");
            //LOCK_MOVEMENT=true
            //playera.velocityOnlySideways(new Vector2(-3.5,0));
            var bloop = 0;
            cam_pan_done = false;
            addEventListener("tick",(delta)=> {
                bloop+=0.9*delta
                if(bloop > 700) {
                    kill_current_listener()
                    //playera.position = meowmedaodiskdas.position
                    //mod_prop_copy_phys(playera._KE_Prop)
                    //LOCK_MOVEMENT=false;
                    console.log("awawawa!"+bloop);
                }else{

                    door.rotation = new Vector3(0,bloop/360,0)
                }
            })
            
        }
    }
})