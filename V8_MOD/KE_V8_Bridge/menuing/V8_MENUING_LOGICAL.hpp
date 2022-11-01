#pragma once

vector<element> screen_elements;

static void Getelvec2(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(quick_create_vector2(screen_elements[valueb].text.position));
}


static void Setelvec2(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    float letx = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    screen_elements[valueb].text.position.x = letx;
    screen_elements[valueb].image.position.x = letx;

    float lety = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    screen_elements[valueb].text.position.y = lety;
    screen_elements[valueb].image.position.y = lety;
}

static void Getimscal2(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(quick_create_vector2(screen_elements[valueb].image.scale));
}


static void Setimscal2(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    float letx = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    screen_elements[valueb].image.scale.x = letx;

    float lety = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    screen_elements[valueb].image.scale.y = lety;
}

static void Getelts(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;


    info.GetReturnValue().Set(v8::Number::New(isolate, screen_elements[valueb].text.scale));
}


static void Setelts(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  screen_elements[valueb].text.scale = value->NumberValue(isolate->GetCurrentContext()).FromJust();
}



static void Geteltext(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, screen_elements[valueb].text.text.c_str()).ToLocalChecked());
}


static void Seteltext(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  v8::String::Utf8Value str(isolate, value);
  screen_elements[valueb].set_text(ToCString(str));
}

static void Getelfont(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

}


static void Setelfont(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  screen_elements[valueb].text.tfont = &cfonts[dock];
}

static void Setelcolor(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  screen_elements[valueb].text.color.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  screen_elements[valueb].text.color.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  screen_elements[valueb].text.color.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void Getelcolor(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(quick_create_vector3(screen_elements[valueb].text.color));
}

static void Setimagetexture(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  screen_elements[valueb].image.imbase = &ctextures[dock];
}

static void Setimagetexture1(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    screen_elements[valueb].image.has2 = true;
    screen_elements[valueb].image.imbase2 = &ctextures[dock];
}

void SetimageshaderDefault(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int idthinh = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    screen_elements[idthinh].image.shaderz = &image_base_shader;
}

static void Setimageshader(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  screen_elements[valueb].image.shaderz = &cshaders[dock];
}

static void Getimageshader(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);
    //TODO: make return shader
}

static void Gettexture1(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);
    //TODO: make return shader
}

static void Setimflipx(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  bool dock = value->BooleanValue(isolate);

  screen_elements[valueb].image.flipx = dock;
}

static void Getimflipx(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(screen_elements[valueb].image.flipx);
}

static void Setimflipy(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  bool dock = value->BooleanValue(isolate);

  screen_elements[valueb].image.flipy = dock;
}

static void Getimflipy(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(screen_elements[valueb].image.flipy);
}

static void SetelalignX(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  v8::String::Utf8Value str(isolate, value);
  


  screen_elements[valueb].text.setalignmentX(ToCString(str));
}

static void GetelalignX(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, screen_elements[valueb].text.getalignmentX().c_str()).ToLocalChecked());
}

static void SetelalignY(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  v8::String::Utf8Value str(isolate, value);
  


  screen_elements[valueb].text.setalignmentY(ToCString(str));
}

static void GetelalignY(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, screen_elements[valueb].text.getalignmentY().c_str()).ToLocalChecked());
}

vector<int> element_available;
int element_available_size = 0;

void elDelete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int idthinh = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    screen_elements[idthinh].not_used = true;
    element_available.push_back(idthinh);
}