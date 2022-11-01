#pragma once

void TextMenuElementConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> MenuElement_templ;

    element elpt;

    elpt.has_text = 1;
    elpt.text.tfont = &base_font;
    elpt.text.shaderz = &text_base_shader;
    elpt.text.position = glm::vec2(25.0f, 25.0f);
    elpt.text.scale = 0.3f;
    elpt.text.color = glm::vec3(1.0, 1.0f, 1.0f);
    elpt.text.text = "";

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
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getelvec2, Setelvec2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getelts, Setelts, v8::Integer::New(isolate,awaeex));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "font").ToLocalChecked(), Getelfont, Setelfont, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text").ToLocalChecked(), Geteltext, Seteltext, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "font").ToLocalChecked(), Getelfont, Setelfont, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "color").ToLocalChecked(), Getelcolor, Setelcolor, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "alignX").ToLocalChecked(), GetelalignX, SetelalignX, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "alignY").ToLocalChecked(), GetelalignY, SetelalignY, v8::Integer::New(isolate,awaeex));
    awaeex++;

    MenuElement_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> MenuElement_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, MenuElement_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(MenuElement_obj);
}