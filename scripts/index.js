console.log('Hello World from a JS code string! awa')

var moew = create_menu_text()
mod_menu_text_text(moew, `KITSUNE ENGINE DEV BUILD`)
mod_menu_pos(moew, new Vector2(-0.99, 0.93))

var tvhowto = create_menu_text()
mod_menu_text_text(tvhowto, `\nopen console with the \` key [unpins cursor from window]`)
mod_menu_pos(tvhowto, new Vector2(-0.99, -0.87))

//console.log(get_file("README.md"));

let wtfisthisprop = new Prop();

addEventListener("keypress", (keyevt) => {
    //console.log(JSON.stringify(keyevt));

    if(keyevt.which == 26&&keyevt.how=="pressed")
        wtfisthisprop.destroy()
})