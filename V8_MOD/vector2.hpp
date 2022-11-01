v8::Persistent<v8::ObjectTemplate> vec2_templ;

v8::Local<v8::Object> quick_create_vector2(glm::vec2 vvinput){

    if (vec2_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));;
        vec2_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec2_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec2_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate, vvinput.x));
    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate, vvinput.y));

    return vec2_obj;
}

v8::Local<v8::Object> v8_vector2(const v8::FunctionCallbackInfo<v8::Value>& args){

    if (vec2_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));
        vec2_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec2_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec2_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber()) {
        vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), args[0]);
        vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), args[1]);
    }

    return vec2_obj;
}

void Vector2Constructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);


    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber()) {
        args.GetReturnValue().Set(v8_vector2(args));
    }
}