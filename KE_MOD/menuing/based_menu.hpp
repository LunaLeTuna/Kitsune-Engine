#pragma once

class element{
public:
    bool not_used = 0;

    glm::vec2 position;
    glm::vec2 size;

    bool has_image = 1;
    image_element image;

    bool has_text = 0;
    text_element text;

    bool allow_render = 1;

    void set_text(string le_filler){
        has_text = 1;
        text.text = le_filler;
    }

    void render(){
        if(has_text == 1){
            text.Render();
        }
        if(has_image == 1){
            image.Render();
        }
    }

};