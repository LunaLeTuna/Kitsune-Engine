void Vector3Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    float tx = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ty = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float tz = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float vedy = sqrtf(tx*tx+ty*ty+tz*tz);

    args.Holder()->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,tx/vedy));
    args.Holder()->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,ty/vedy));
    args.Holder()->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,tz/vedy));

    args.GetReturnValue().Set(args.Holder());
}

v8::Persistent<v8::ObjectTemplate> vec3_templ;

v8::Local<v8::Object> quick_create_vector3(glm::vec3 vvinput){

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));
        local->Set(isolate, "z", v8::Number::New(isolate,0));
        local->Set(isolate, "Normalize", v8::FunctionTemplate::New(isolate, Vector3Normalize));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate, vvinput.x));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate, vvinput.y));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate, vvinput.z));

    return vec3_obj;
}

v8::Local<v8::Object> v8_vector3(const v8::FunctionCallbackInfo<v8::Value>& args){

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));
        local->Set(isolate, "z", v8::Number::New(isolate,0));
        local->Set(isolate, "Normalize", v8::FunctionTemplate::New(isolate, Vector3Normalize));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber() &&
        !args[2].IsEmpty() && args[2]->IsNumber()) {
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), args[0]);
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), args[1]);
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), args[2]);
    }

    return vec3_obj;
}

void Vector3Constructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);


    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber() &&
        !args[2].IsEmpty() && args[2]->IsNumber()) {
        args.GetReturnValue().Set(v8_vector3(args));
        return;
    }
    args.GetReturnValue().Set(v8_vector3(args));
}