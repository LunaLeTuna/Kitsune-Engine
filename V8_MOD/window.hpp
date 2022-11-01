#pragma once

void Win_title(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(isolate);
        if (first) {
        first = false;
        } else {
        glfwSetWindowTitle(window, "Kitsune Engine");
        }
        v8::String::Utf8Value str(isolate, args[i]);
        const char* cstr = ToCString(str);
        glfwSetWindowTitle(window, cstr);
    }
    fflush(stdout);
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, v8::V8::GetVersion()).ToLocalChecked());
}

static void GetscreenWidth(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

  info.GetReturnValue().Set(v8::Integer::New(isolate, SCR_WIDTH));
}


static void SetscreenWidth(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

//   part[valueb].rotation.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void GetscreenHeight(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

  info.GetReturnValue().Set(v8::Integer::New(isolate, SCR_HEIGHT));
}


static void SetscreenHeight(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

//   part[valueb].rotation.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

void WindowManager( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> WindowManager_templ;


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "innerWidth").ToLocalChecked(), GetscreenWidth, SetscreenWidth);
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "innerHeight").ToLocalChecked(), GetscreenHeight, SetscreenHeight);

    v8::Local<v8::Object> WindowManager_obj =
    v8::Local<v8::ObjectTemplate>::New(isolate, WindowManager_templ)
        ->NewInstance(isolate->GetCurrentContext())
        .ToLocalChecked();

    args.GetReturnValue().Set(WindowManager_obj);
}

void toggle_curser_pin(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::HandleScope handle_scope(isolate);
    bool c = args[0]->BooleanValue(isolate);

    if(c){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    fflush(stdout);
}