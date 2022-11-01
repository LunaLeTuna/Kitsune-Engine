#pragma once

int shadpogs = 0; //size
vector<shader> cshaders;

void SetShaderFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    cout << "setted was the model " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        cshaders[MID].craft(cstr);
    }
}

void ShaderConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> shader_templ;

    shader shadercl;

    shadercl.name = "shader_"+to_string(shadpogs);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        shadercl.craft(cstr);
        //cout << "wa " << cstr << endl;
        //cout << "setted was the model " << modecl.VBO << endl;
    }

    cshaders.push_back(shadercl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, shadpogs));
    local->Set(isolate, "SetFandV", v8::FunctionTemplate::New(isolate, SetShaderFile));
    shadpogs++;
    shader_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> shader_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, shader_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(shader_obj);
}