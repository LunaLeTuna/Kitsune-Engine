#pragma once

int awac = 0; //size
vector<cameras> cams;

static void Getvec3pc(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;


  info.GetReturnValue().Set(quick_create_vector3(cams[valueb].position));
}


static void Setvec3pc(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  cams[valueb].position.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  cams[valueb].position.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  cams[valueb].position.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, cams[valueb].front.x, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::translate(model, glm::vec3(-cams[valueb].position.x, -cams[valueb].position.y, -cams[valueb].position.z));
    cams[valueb].view = model;
}

static void Getvec3f(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

  v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;


  info.GetReturnValue().Set(quick_create_vector3(cams[valueb].front));
}

double pi = M_PI;

static void Setvec3f(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    glm::vec3 tvec;

    tvec.x = (value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()/180)*pi;

    tvec.y = (value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()/180)*pi;

    tvec.z = (value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()/180)*pi;


    cams[valueb].set_rotation(tvec);

}

void CamLookAt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    if(!args[0].IsEmpty()){
        int valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();


        float rxx = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        float ryy = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        float rzz = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();


//         glm::vec3 f = glm::normalize(glm::vec3(rxx, ryy, rzz) - cams[valueb].position);
//         glm::vec3 u = glm::normalize(cams[valueb].up);
//         glm::vec3 s = glm::normalize(glm::cross(f, u));
//         u = glm::cross(s, f);
//
//         glm::mat4 trass = {
//             { s.x, u.x, -f.x, 0 },
//             { s.y, u.y, -f.y, 0 },
//             { s.z, u.z, -f.z, 0 },
//             { -glm::dot(s, cams[valueb].position), -glm::dot(u, cams[valueb].position), glm::dot(f, cams[valueb].position), 1 }
//         };

        glm::mat4 trass = glm::lookAt(cams[valueb].position, glm::vec3(rxx, ryy, rzz), cams[valueb].up);

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(trass, scale, rotation, translation, skew, perspective);

        rotation = glm::conjugate(rotation);

        cams[valueb].front  = glm::eulerAngles(rotation);// * 3.14159f / 180.f;
        cams[valueb].front = glm::vec3(-cams[valueb].front.x,-cams[valueb].front.y,cams[valueb].front.z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, cams[valueb].front.x, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::translate(model, glm::vec3(-cams[valueb].position.x, -cams[valueb].position.y, -cams[valueb].position.z));
        cams[valueb].view = model;
    }
}

void CameraConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> camera_templ;

    cameras cpart;

    cpart.name = "c"+to_string(awac);

    cpart.projection = glm::perspective(glm::radians(cpart.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, cpart.near, cpart.far);

    cams.push_back(cpart);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awac));
    local->Set(isolate, "LookAt", v8::FunctionTemplate::New(isolate, CamLookAt));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3pc, Setvec3pc, v8::Integer::New(isolate,awac));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), Getvec3f, Setvec3f, v8::Integer::New(isolate,awac));
    awac++;
    camera_templ.Reset(isolate, inner.Escape(local));

    v8::Local<v8::Object> camera_obj =
    v8::Local<v8::ObjectTemplate>::New(isolate, camera_templ)
        ->NewInstance(isolate->GetCurrentContext())
        .ToLocalChecked();

    args.GetReturnValue().Set(camera_obj);
}

void set_main_cam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int dock = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    main_cam = &cams[dock];
}