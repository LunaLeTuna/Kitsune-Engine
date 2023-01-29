#pragma once

void AddInputEventButton( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);

    if (args[1]->IsFunction() && args[0]->IsString()) {

        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);

        int id = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        int dex = find_inputevent((char*)cstr, (char*)("button"+id));
        if(dex != -1) {
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
            event_calls[dex].func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(isolate, callback));
        }else{
            //so we can't find a box with the lable, so we need a new one :3
            inputevent_feld newt; //make new box

            newt.call_on = (char*)cstr; //lable new box

            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]); //grab the function out of the arguments
            newt.func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(isolate, callback)); //plop the function in to the box we made, after converting it to persistent

            entvcount++;
            event_calls.push_back(newt); //then put the box up in to storage for later
        }
    }

}

void ButtonMenuElementConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> MenuElement_templ;

    int awaeex = 0;

    if(element_available.empty()){
        awaeex = screen_elements.size();
    }else{
        // cout << element_available.back() <<  endl;
        awaeex = element_available.back();
    }

    element elpt;

    elpt.has_text = 1;
    elpt.text.tfont = &base_font;
    elpt.text.shaderz = &text_base_shader;
    elpt.text.position = glm::vec2(25.0f, 25.0f);
    elpt.text.scale = 0.3f;
    elpt.text.color = glm::vec3(1.0, 1.0f, 1.0f);
    elpt.text.text = "";

    elpt.has_image = 1;
    elpt.image.shaderz = &image_base_shader;
    elpt.image.position = glm::vec2(20.0f, 20.0f);
    elpt.image.scale = glm::vec2(100.0f, 100.0f);
    elpt.image.imbase = &tex;

    elpt.is_button = 1;
    const char* lvat = "button"+awaeex;
    elpt.pressed_list = one_box_please("pressed", (char *)lvat);
    elpt.hover_list = one_box_please("hover", (char *)lvat);
    elpt.unhover_list = one_box_please("unhover", (char *)lvat);

    if(element_available.empty()){
        screen_elements.push_back(elpt);
    }else{
        // cout << element_available.back() <<  endl;
        screen_elements[element_available.back()] = elpt;
        element_available.pop_back();
        element_available_size--;
    }

    cout << "nya >w< butt: " << awaeex << endl;


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awaeex));

    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getelvec2, Setelvec2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getimscal2, Setimscal2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "Tscale").ToLocalChecked(), Getelts, Setelts, v8::Integer::New(isolate,awaeex));

    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text").ToLocalChecked(), Geteltext, Seteltext, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "font").ToLocalChecked(), Getelfont, Setelfont, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "color").ToLocalChecked(), Getelcolor, Setelcolor, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "alignX").ToLocalChecked(), GetelalignX, SetelalignX, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "alignY").ToLocalChecked(), GetelalignY, SetelalignY, v8::Integer::New(isolate,awaeex));

    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Setimagetexture, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture1").ToLocalChecked(), Gettexture1, Setimagetexture1, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "shader").ToLocalChecked(), Getimageshader, Setimageshader, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "flipped_x").ToLocalChecked(), Getimflipx, Setimflipx, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "flipped_y").ToLocalChecked(), Getimflipy, Setimflipy, v8::Integer::New(isolate,awaeex));

    local->Set(isolate, "addEventListener", v8::FunctionTemplate::New(isolate, AddInputEventButton));

    local->Set(isolate, "Delete", v8::FunctionTemplate::New(isolate, elDelete));
    awaeex++;

    MenuElement_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> MenuElement_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, MenuElement_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(MenuElement_obj);
}