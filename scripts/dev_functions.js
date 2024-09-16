function texture_viewer(){

    var textureflipthrough = 0;

    var cawa = create_menu_image()
    create_menu_image_texture(cawa, textureflipthrough)
    mod_menu_pos(cawa, new Vector2(0.4, -0.5))
    mod_menu_scale(cawa, new Vector2(0.4, 0.4))

    var textureinfo = create_menu_text()
    mod_menu_text_text(textureinfo, `texture: 0`)
    mod_menu_pos(textureinfo, new Vector2(-0.99, -0.77))

    //var tvhowto = create_menu_text()
    mod_menu_text_text(tvhowto, `use "[" and "]" to navigate`)
    mod_menu_pos(tvhowto, new Vector2(-0.99, -0.87))


    addEventListener("keypress", (keyevt) => {
        //console.log(JSON.stringify(keyevt));

        if(keyevt.which == 26&&keyevt.how=="pressed")
            textureflipthrough--
        else if(keyevt.which == 27&&keyevt.how=="pressed")
            textureflipthrough++
        
        if(textureflipthrough <0) textureflipthrough=0
        create_menu_image_texture(cawa, textureflipthrough)
        mod_menu_text_text(textureinfo, `texture: ${textureflipthrough}`)
    })

}