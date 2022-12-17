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

void setBool(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    bool* ax = new bool(args[1]->BooleanValue(isolate));

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    cshaders[MID].audit_attribute(cstr, kickin, 5);
}
// ------------------------------------------------------------------------
void setInt(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    int* ax = new int(args[1]->NumberValue(isolate->GetCurrentContext()).FromJust());

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    cshaders[MID].audit_attribute(cstr, kickin, 0);
}
// ------------------------------------------------------------------------
void setFloat(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float* ax = new float(args[1]->NumberValue(isolate->GetCurrentContext()).FromJust());

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    cshaders[MID].audit_attribute(cstr, kickin, 1);
}
// ------------------------------------------------------------------------
void setVec2(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ax = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ay = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(new glm::vec2(ax, ay));
    cshaders[MID].audit_attribute(cstr, kickin, 2);
}
// ------------------------------------------------------------------------
void setVec3(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ax = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ay = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float az = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(new glm::vec3(ax, ay, az));
    cshaders[MID].audit_attribute(cstr, kickin, 3);
}
// ------------------------------------------------------------------------


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
    local->Set(isolate, "setBool", v8::FunctionTemplate::New(isolate, setBool));
    local->Set(isolate, "setInt", v8::FunctionTemplate::New(isolate, setInt));
    local->Set(isolate, "setFloat", v8::FunctionTemplate::New(isolate, setFloat));
    local->Set(isolate, "setVec2", v8::FunctionTemplate::New(isolate, setVec2));
    local->Set(isolate, "setVec3", v8::FunctionTemplate::New(isolate, setVec3));
    shadpogs++;
    shader_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> shader_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, shader_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(shader_obj);
}