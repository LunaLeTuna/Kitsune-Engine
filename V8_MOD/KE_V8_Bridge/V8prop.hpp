#pragma once

vector<prop> props;

model cubez;
texture tex;
texture spec;

int awax = 0; //size
vector<prop> part;

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

  part[valueb].textures = &ctextures[dock];
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

  part[valueb].speculars = &ctextures[dock];
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

#ifdef Include_physics
vector<btRigidBody*> bodies;

btRigidBody* create_physbox(glm::vec3 position, glm::vec3 size, float mass){
    btTransform L;
    L.setIdentity();
    L.setOrigin(btVector3(position.x,position.y,position.z));
    btBoxShape* box = new btBoxShape(btVector3(size.x,size.y,size.z));
    btMotionState* motionL = new btDefaultMotionState(L);
    btRigidBody::btRigidBodyConstructionInfo infoL(mass,motionL,box);
    btRigidBody* bodyL = new btRigidBody(infoL);
    dynamicWorld->addRigidBody(bodyL);
    bodies.push_back(bodyL);
    return bodyL;
}

void create_physbody(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::HandleScope handle_scope(isolate);

    float xp, yp, zp;
    if(args[0]->IsObject()){
        xp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        yp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        zp = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
    }else{
        xp=1;
        yp=1;
        zp=1;
    }
    
    float valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].phys_counterpart = create_physbox(part[valueb].position, glm::vec3(xp,yp,zp), part[valueb].mass);
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

void PropConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> prop_templ;

    prop zpart;

    zpart.name = "woot"+to_string(awax);

    zpart.models = &cubez;
    zpart.textures = &tex;
    zpart.speculars = &spec;

    part.push_back(zpart);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3p, Setvec3p, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "scale").ToLocalChecked(), Getvec3sc, Setvec3sc, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), Getvec3r, Setvec3r, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Settexture, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "specular").ToLocalChecked(), Getspecular, Setspecular, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "shader").ToLocalChecked(), Getshader, Setshader, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "model").ToLocalChecked(), Getmodel, Setmodel, v8::Integer::New(isolate,awax));
#ifdef Include_physics
    local->Set(v8::String::NewFromUtf8(isolate, "create_physbody").ToLocalChecked(), v8::FunctionTemplate::New(isolate, create_physbody));
    local->Set(v8::String::NewFromUtf8(isolate, "AddForce").ToLocalChecked(), v8::FunctionTemplate::New(isolate, add_force));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "LinearVelocity").ToLocalChecked(), Getpvl, Setpvl, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "mass").ToLocalChecked(), Getpmass, Setpmass, v8::Integer::New(isolate,awax));
#endif

    awax++;
    prop_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> prop_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, prop_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(prop_obj);
}