var maincamera = getCamByName("TheCharacterCamera");
var portal_geo = new Prop();
var portal_tex = new Texture("");
var portal_cam = new Camera();
portal_geo.texture = portal_tex;
tepter(1, 0)
portal_cam.buffer_write2 = 1;

portal_geo.scale = new Vector3(0.2,4,3)
portal_geo.position = new Vector3(0,4,0)
mod_prop_shader(portal_geo._KE_Prop, 4)
mod_prop_model(portal_geo._KE_Prop, 1)
mod_prop_texture(portal_geo._KE_Prop, 0, 1)

function loop(delta){
    
    let mrow = cameras.position;
    portal_cam.position = new Vector3(mrow.x-10,mrow.y,mrow.z-60)

    let asaf = playera.position;
    portal_cam.lookat(new Vector3(asaf.x-cameras.position.x,asaf.y-cameras.position.y,asaf.z-cameras.position.z))
}

addEventListener("tick",loop)