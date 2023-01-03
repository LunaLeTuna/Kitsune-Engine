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

    bool is_button = 0;
    int pressed_list = -1;
    int hover_list = -1;
    int unhover_list = -1;

    void set_text(string le_filler){
        has_text = 1;
        text.text = le_filler;
    }

    int pressed_button(){
        if(!is_button) return 0;
        if((xpos_mouse >= image.position.x-image.scale.x/2 && xpos_mouse <= image.position.x+image.scale.x/2)&&(ypos_mouse >= image.position.y-image.scale.y/2 && ypos_mouse <= image.position.y+image.scale.y/2)){
                for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[pressed_list].func) {
                    v8::TryCatch trycatch(isolate);
                    v8::Local<v8::Value> a = v8::Integer::New(isolate, 0);
                    v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &a);
                }
                return 1;
            }else{
                return 0;
            }
    }

    void render(){
        if(is_button == 1){
            if((xpos_mouse >= image.position.x-image.scale.x/2 && xpos_mouse <= image.position.x+image.scale.x/2)&&(ypos_mouse >= image.position.y-image.scale.y/2 && ypos_mouse <= image.position.y+image.scale.y/2)){
                for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[hover_list].func) {
                    v8::TryCatch trycatch(isolate);
                    v8::Local<v8::Value> a = v8::Integer::New(isolate, 0);
                    v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &a);
                }
            }else{
                for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[unhover_list].func) {
                    v8::TryCatch trycatch(isolate);
                    v8::Local<v8::Value> a = v8::Integer::New(isolate, 0);
                    v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &a);
                }
            }

        }
        if(has_image == 1){
            image.Render();
        }
        if(has_text == 1){
            text.Render();
        }
    }

};