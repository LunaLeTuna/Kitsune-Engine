#pragma once

vector<prop> props;

model cubez;
texture tex;
texture spec;

int awax = 0; //size
vector<prop> part;

void setPBool(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    bool* ax = new bool(args[1]->BooleanValue(isolate));

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    part[MID].audit_shader_override_insert_attribute(cstr, kickin, 5);
}
// ------------------------------------------------------------------------
void setPInt(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    int* ax = new int(args[1]->NumberValue(isolate->GetCurrentContext()).FromJust());

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    part[MID].audit_shader_override_insert_attribute(cstr, kickin, 0);
}
// ------------------------------------------------------------------------
void setPFloat(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float* ax = new float(args[1]->NumberValue(isolate->GetCurrentContext()).FromJust());

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(ax);
    part[MID].audit_shader_override_insert_attribute(cstr, kickin, 1);
}
// ------------------------------------------------------------------------
void setPVec2(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ax = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ay = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(new glm::vec2(ax, ay));
    part[MID].audit_shader_override_insert_attribute(cstr, kickin, 2);
}
// ------------------------------------------------------------------------
void setPVec3(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ax = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float ay = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float az = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);
    
    void* kickin = (void*)(new glm::vec3(ax, ay, az));
    part[MID].audit_shader_override_insert_attribute(cstr, kickin, 3);
}
// ------------------------------------------------------------------------

static void Getvec3p(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.x));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.y));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.z));


  info.GetReturnValue().Set(vec3_obj);
}


static void Setvec3p(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    part[valueb].position.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].position.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].position.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

#ifdef Include_physics
    if(part[valueb].has_physbody){
        btTransform transform;
        transform.setIdentity();

        transform.setOrigin(btVector3(part[valueb].position.x,part[valueb].position.y,part[valueb].position.z));

        part[valueb].phys_counterpart->setWorldTransform(transform);
        part[valueb].phys_counterpart->getMotionState()->setWorldTransform(transform);
    }
#endif
}

static void Getvec3sc(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].scale.x));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].scale.y));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].scale.z));


  info.GetReturnValue().Set(vec3_obj);
}


static void Setvec3sc(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  part[valueb].scale.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].scale.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].scale.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void Getvec3r(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

     int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;


  info.GetReturnValue().Set(quick_create_vector3(part[valueb].rotation));
}


static void Setvec3r(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    part[valueb].rotation.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].rotation.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].rotation.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

}

static void Gettexture(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);
    //TODO: make return texture
}


static void Settexture(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].imbase0 = &ctextures[dock];
}

static void Getspecular(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    //TODO: make return texture
}


static void Setspecular(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].imbase1 = &ctextures[dock];
}

static void Getshader(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);
    //TODO: make return texture
}


static void Setshader(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].default_shader = 0;
    part[valueb].shaders = &cshaders[dock];
}

static void Getmodel(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    //TODO: make return model
}


static void Setmodel(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int valueb = info.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].models = &cmodels[dock];
}

void depthdraw(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::HandleScope handle_scope(isolate);
    
    float valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    if(!args[0].IsEmpty() && args[0]->IsBoolean()){
        part[valueb].ignore_depth_test = args[0]->BooleanValue(isolate);
    }

}

#ifdef Include_physics
vector<btRigidBody*> bodies;

btRigidBody* create_physbox(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float mass, int id, int group){
    btTransform L;
    L.setIdentity();
    L.setOrigin(btVector3(position.x,position.y,position.z));
    L.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
    btBoxShape* box = new btBoxShape(btVector3(size.x,size.y,size.z));
    btMotionState* motionL = new btDefaultMotionState(L);
    btRigidBody::btRigidBodyConstructionInfo infoL(mass,motionL,box);
    btRigidBody* bodyL = new btRigidBody(infoL);
    bodyL->setUserIndex(id);
    dynamicWorld->addRigidBody(bodyL, group, group);
    bodies.push_back(bodyL);
    return bodyL;
}

void create_physbody(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::HandleScope handle_scope(isolate);
    
    float valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    int ColType = 0;
    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        if(strcmp(cstr, "box")==0)
            ColType = 0;
        // else if(strcmp(cstr, "sphere")==0)
        //     ColType = 1;
    }

    float xp, yp, zp;
    if(!args[1].IsEmpty() && args[1]->IsObject()){
        xp = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        yp = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        zp = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    }else{
        xp=part[valueb].scale.x;
        yp=part[valueb].scale.y;
        zp=part[valueb].scale.z;
    }

    //put collision offset here

    int groupID = -1;
    if(!args[3].IsEmpty() && args[3]->IsNumber()){
        groupID = args[3]->NumberValue(isolate->GetCurrentContext()).FromJust();
    }

    switch (ColType)
    {
    case 0:
        part[valueb].phys_counterpart = create_physbox(part[valueb].position, part[valueb].rotation, glm::vec3(xp,yp,zp), part[valueb].mass, valueb, groupID);
        break;
    
    // case 1:
    //     part[valueb].phys_counterpart = create_physsphere(part[valueb].position, glm::vec3(xp,yp,zp), part[valueb].mass, valueb, -1);
    //     break;
    
    default:
        break;
    }
    
    part[valueb].has_physbody = 1;

    fflush(stdout);
}

void add_force(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::HandleScope handle_scope(isolate);

    float valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    if(part[valueb].has_physbody){
        
        float xp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        float yp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        float zp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        part[valueb].phys_counterpart->forceActivationState(ACTIVE_TAG);
        part[valueb].phys_counterpart->applyCentralImpulse(btVector3(xp,yp,zp));
        
    }

    fflush(stdout);
}
#endif


static void Getpvl(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    #ifdef Include_physics
    if(part[valueb].has_physbody){
    //getLinearVelocity
        btVector3 lva = part[valueb].phys_counterpart->getLinearVelocity();

        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,lva.getX()));
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,lva.getY()));
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,lva.getZ()));

    }
    #endif


  info.GetReturnValue().Set(vec3_obj);
}


static void Setpvl(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    #ifdef Include_physics
    if(part[valueb].has_physbody){
        int xp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        int yp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        int zp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        part[valueb].phys_counterpart->forceActivationState(ACTIVE_TAG);
        part[valueb].phys_counterpart->setLinearVelocity(btVector3(xp,yp,zp));
    }
    #endif
}

#ifdef Include_physics
static void Setpmass(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  part[valueb].mass = value->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void Getpmass(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    info.GetReturnValue().Set(v8::Integer::New(isolate,part[valueb].mass));
}
#endif

/*
 * prop is just a frick ton of pointers to other resources
 * so we arn't wasting a load of resources giving every individual prop it's own model
 * yeah, you pretty much get it
 */

v8::Local<v8::Object> v8_prop(int propID){

    v8::Persistent<v8::ObjectTemplate> prop_templ;

    if (prop_templ.IsEmpty()) {
    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "_id", v8::Integer::New(isolate, propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3p, Setvec3p, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getvec3sc, Setvec3sc, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), Getvec3r, Setvec3r, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Settexture, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "specular").ToLocalChecked(), Getspecular, Setspecular, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "shader").ToLocalChecked(), Getshader, Setshader, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "model").ToLocalChecked(), Getmodel, Setmodel, v8::Integer::New(isolate,propID));
        local->Set(v8::String::NewFromUtf8(isolate, "depthdraw").ToLocalChecked(), v8::FunctionTemplate::New(isolate, depthdraw));
        local->Set(v8::String::NewFromUtf8(isolate, "setBool").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPBool));
        local->Set(v8::String::NewFromUtf8(isolate, "setInt").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPInt));
        local->Set(v8::String::NewFromUtf8(isolate, "setFloat").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPFloat));
        local->Set(v8::String::NewFromUtf8(isolate, "setVec2").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPVec2));
        local->Set(v8::String::NewFromUtf8(isolate, "setVec3").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPVec3));
#ifdef Include_physics
        local->Set(v8::String::NewFromUtf8(isolate, "create_physbody").ToLocalChecked(), v8::FunctionTemplate::New(isolate, create_physbody));
        local->Set(v8::String::NewFromUtf8(isolate, "AddForce").ToLocalChecked(), v8::FunctionTemplate::New(isolate, add_force));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "LinearVelocity").ToLocalChecked(), Getpvl, Setpvl, v8::Integer::New(isolate,propID));
        local->SetAccessor(v8::String::NewFromUtf8(isolate, "mass").ToLocalChecked(), Getpmass, Setpmass, v8::Integer::New(isolate,propID));
#endif

        prop_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> prop_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, prop_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    return prop_obj;
}

void PropConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);

    prop zpart;

    zpart.name = "woot"+to_string(awax);

    zpart.models = &cubez;
    zpart.imbase0 = &tex;
    zpart.imbase1 = &spec;

    part.push_back(zpart);
    
    args.GetReturnValue().Set(v8_prop(awax));
    awax++;
}

// void PropConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

//     v8::HandleScope handle_scope(isolate);
//     v8::Persistent<v8::ObjectTemplate> prop_templ;

//     prop zpart;

//     zpart.name = "woot"+to_string(awax);

//     zpart.models = &cubez;
//     zpart.textures = &tex;
//     zpart.speculars = &spec;

//     part.push_back(zpart);


//     v8::EscapableHandleScope inner(isolate);
//     v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
//     local->Set(isolate, "_id", v8::Integer::New(isolate, awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3p, Setvec3p, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getvec3sc, Setvec3sc, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), Getvec3r, Setvec3r, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Settexture, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "specular").ToLocalChecked(), Getspecular, Setspecular, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "shader").ToLocalChecked(), Getshader, Setshader, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "model").ToLocalChecked(), Getmodel, Setmodel, v8::Integer::New(isolate,awax));
//     local->Set(v8::String::NewFromUtf8(isolate, "setBool").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPBool));
//     local->Set(v8::String::NewFromUtf8(isolate, "setInt").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPInt));
//     local->Set(v8::String::NewFromUtf8(isolate, "setFloat").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPFloat));
//     local->Set(v8::String::NewFromUtf8(isolate, "setVec2").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPVec2));
//     local->Set(v8::String::NewFromUtf8(isolate, "setVec3").ToLocalChecked(), v8::FunctionTemplate::New(isolate, setPVec3));
// #ifdef Include_physics
//     local->Set(v8::String::NewFromUtf8(isolate, "create_physbody").ToLocalChecked(), v8::FunctionTemplate::New(isolate, create_physbody));
//     local->Set(v8::String::NewFromUtf8(isolate, "AddForce").ToLocalChecked(), v8::FunctionTemplate::New(isolate, add_force));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "LinearVelocity").ToLocalChecked(), Getpvl, Setpvl, v8::Integer::New(isolate,awax));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "mass").ToLocalChecked(), Getpmass, Setpmass, v8::Integer::New(isolate,awax));
// #endif

//     awax++;
//     prop_templ.Reset(isolate, inner.Escape(local));

//         v8::Local<v8::Object> prop_obj =
//       v8::Local<v8::ObjectTemplate>::New(isolate, prop_templ)
//           ->NewInstance(isolate->GetCurrentContext())
//           .ToLocalChecked();

//         args.GetReturnValue().Set(prop_obj);
// }