#pragma once

#ifndef V8_Objs
#define V8_Objs

#include "./print.hpp"
#include "./window.hpp"
#include "./V8cmd.hpp"
#include "./require.hpp"

#include "./vector3.hpp"
#include "./vector2.hpp"

#include "./KE_V8_Bridge/KE_Bridge.hpp"

#include "./V8rayCast.hpp"

v8::Local<v8::Context> load_wrap_functions(v8::Isolate* isolate) {
    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

    //Vector3s :3
    global->Set(isolate, "Vector3", v8::FunctionTemplate::New(isolate, Vector3Constructor));

    //Vector2s ,':|c
    global->Set(isolate, "Vector2", v8::FunctionTemplate::New(isolate, Vector2Constructor));

    //textures
    global->Set(isolate, "Texture", v8::FunctionTemplate::New(isolate, TextureConstructor));

    //models
    global->Set(isolate, "Model", v8::FunctionTemplate::New(isolate, ModelConstructor));

    //shaders
    global->Set(isolate, "Shader", v8::FunctionTemplate::New(isolate, ShaderConstructor));

    //props
    global->Set(isolate, "Prop", v8::FunctionTemplate::New(isolate, PropConstructor));

#ifdef Include_physics
    //raycast
    global->Set(isolate, "RayCast", v8::FunctionTemplate::New(isolate, RayConstruct));
#endif

    //Cameras
    global->Set(isolate, "Camera", v8::FunctionTemplate::New(isolate, CameraConstructor));
    global->Set(isolate, "SetMainCam", v8::FunctionTemplate::New(isolate, set_main_cam));

    //Point Lights
    global->Set(isolate, "PointLight", v8::FunctionTemplate::New(isolate, PointLightConstructor));

    //fonts
    global->Set(isolate, "Font", v8::FunctionTemplate::New(isolate, FontConstructor));

    //Menu Elements
    global->Set(isolate, "TextElement", v8::FunctionTemplate::New(isolate, TextMenuElementConstructor));
    global->Set(isolate, "ImageElement", v8::FunctionTemplate::New(isolate, ImageMenuElementConstructor));

    //input
    v8::Local<v8::ObjectTemplate> input_b = v8::ObjectTemplate::New(isolate);
    input_b->Set(isolate, "addEventListener", v8::FunctionTemplate::New(isolate, AddInputEvent));
    global->Set(isolate, "Input", input_b);

#ifdef Include_console_interaction
    global->Set(isolate, "cmd", v8::FunctionTemplate::New(isolate, run_cmd));
#endif

    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));
    global->Set(isolate, "Win_title", v8::FunctionTemplate::New(isolate, Win_title));
    global->Set(isolate, "version", v8::FunctionTemplate::New(isolate, Version));

    global->SetAccessor(v8::String::NewFromUtf8(isolate, "innerWidth").ToLocalChecked(), GetscreenWidth, SetscreenWidth);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "innerHeight").ToLocalChecked(), GetscreenHeight, SetscreenHeight);
    global->Set(isolate, "CursorPin", v8::FunctionTemplate::New(isolate, toggle_curser_pin));

    return v8::Context::New(isolate, NULL, global);
}

#endif