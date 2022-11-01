#pragma once

int NR_POINT_LIGHTS = 0; //size
vector<pointLight> pointLights;

static void Setvec3pls(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  pointLights[valueb].position.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  pointLights[valueb].position.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  pointLights[valueb].position.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void Getvec3pls(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

     int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;


  info.GetReturnValue().Set(quick_create_vector3(pointLights[valueb].position));
}

void PointLightConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> point_light_templ;

    pointLight zlight;

    pointLights.push_back(zlight);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3pls, Setvec3pls, v8::Integer::New(isolate,NR_POINT_LIGHTS));
    NR_POINT_LIGHTS++;

    
    point_light_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> point_light_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, point_light_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(point_light_obj);
}