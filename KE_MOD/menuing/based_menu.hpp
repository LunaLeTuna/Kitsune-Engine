#pragma once

class element{
public:
    bool has_text = 0;
    bool has_background = 0;
    glm::vec2 position;
    glm::vec2 size;

    text_element text;

    bool allow_render = 1;

    void set_text(string le_filler){
        has_text = 1;
        text.text = le_filler;
    }

    void render(){
//         if(has_background == 1){
//
//         }
        if(has_text == 1){
            text.Render();
        }
    }

};