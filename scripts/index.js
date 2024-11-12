console.log('Hello World from a JS code string! awa')

var moew = create_menu_text()
mod_menu_text_text(moew, `KITSUNE ENGINE DEV BUILD`)
mod_menu_pos(moew, new Vector2(-0.99, 0.93))

var tvhowto = create_menu_text()
mod_menu_text_text(tvhowto, `\nopen console with the \` key [unpins cursor from window]`)
mod_menu_pos(tvhowto, new Vector2(-0.99, -0.87))

var xv1 = new Vector3(1,0,2)
var xv2 = new Vector3(2,0,1)

xv1.addTo(xv2)

console.log(xv1);

// addEventListener("keypress", (keyevt) => {
//     //console.log(JSON.stringify(keyevt));

//     if(keyevt.which == 26&&keyevt.how=="pressed"){
//         create_world(1)
//         load_map("burgor.kbf",1)
//         portal_cam.world = 1
//         //need to containerize lights to worlds
//     }
// })