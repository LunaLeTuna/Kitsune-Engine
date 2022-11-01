#pragma once

void ImageMenuElementConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> MenuElement_templ;

    element elpt;

    elpt.has_image = 1;
    elpt.image.shaderz = &image_base_shader;
    elpt.image.position = glm::vec2(20.0f, 20.0f);
    elpt.image.scale = glm::vec2(100.0f, 100.0f);
    elpt.image.imbase = &tex;

    int awaeex = 0;

    if(element_available.empty()){
        screen_elements.push_back(elpt);
        awaeex = screen_elements.size()-1;
    }else{
        // cout << element_available.back() <<  endl;
        awaeex = element_available.back();
        screen_elements[element_available.back()] = elpt;
        element_available.pop_back();
        element_available_size--;
    }


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getelvec2, Setelvec2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getimscal2, Setimscal2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Setimagetexture, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture1").ToLocalChecked(), Gettexture1, Setimagetexture1, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "shader").ToLocalChecked(), Getimageshader, Setimageshader, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "flipped_x").ToLocalChecked(), Getimflipx, Setimflipx, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "flipped_y").ToLocalChecked(), Getimflipy, Setimflipy, v8::Integer::New(isolate,awaeex));
    local->Set(isolate, "shaderDefault", v8::FunctionTemplate::New(isolate, SetimageshaderDefault));
    local->Set(isolate, "Delete", v8::FunctionTemplate::New(isolate, elDelete));
    awaeex++;

    MenuElement_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> MenuElement_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, MenuElement_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(MenuElement_obj);
}