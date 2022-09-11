#include <cstddef>
#define GL_GLEXT_PROTOTYPES
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
 
 
#include <assert.h>
#include <fcntl.h>
#include <libplatform/libplatform.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <pthread.h>

#include <btBulletDynamicsCommon.h>

#include <include/v8.h>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

btDefaultCollisionConfiguration* collisionConfiguration;

btCollisionDispatcher* dispatcher;

btBroadphaseInterface* overlappingPairCache;

btSequentialImpulseConstraintSolver* solver;

btDiscreteDynamicsWorld* dynamicWorld;

string get_file(string location){

    string apock;
    ifstream fev;
    fev.exceptions (ifstream::failbit | ifstream::badbit);
    fev.open(location);
    stringstream fstre;
    fstre << fev.rdbuf();
    fev.close();

    apock = fstre.str();

    apock[apock.size()]='\0';

    //cout << apock;

    return apock;
}

#include "./KE_MOD/KE_Objects.hpp"

cameras* main_cam;

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;


class plane {
public:
    glm::vec3 normal = { 1.0f, 0.0f, 0.0f };
    float distance = 0; 

    bool isInFrontOfPlane(glm::vec3 position) {
        // Get the dot product of the normal and the position vector
        const float dot = glm::dot(normal, position);

        // If the dot product is greater than the distance, then the position is in front of the plane
        return (dot > distance);
    }  
    //asked gpt to "write a c++ function detect if a givin position in in front of a plain. The plain is givin by a class with a position and rotation."
};

class frustum {
public:
  plane farFace;
  plane nearFace;
  plane topFace;
  plane bottomFace;
  plane leftFace;
  plane rightFace;
};

frustum craftFrustum(/*cameras* cam*/){
    frustum based;
    
    based.topFace.normal = {0.0f, 1.0f, 0.0f};
    based.topFace.distance = -5.0f;

    based.bottomFace.normal = {0.0f, -1.0f, 0.0f};
    based.bottomFace.distance = -5.0f;

    based.leftFace.normal = {-1.0f, 0.0f, 0.0f};
    based.leftFace.distance = -5.0f;

    based.rightFace.normal = {1.0f, 0.0f, 0.0f};
    based.rightFace.distance = -5.0f;

    based.nearFace.normal = {0.0f, 0.0f, 1.0f};
    based.nearFace.distance = -5.0f;

    based.farFace.normal = {0.0f, 0.0f, -1.0f};
    based.farFace.distance = -100.0f;

    return based;
}

frustum nya = craftFrustum();

bool check_cull(cameras* cam, prop* thprop){
    if(thprop->skipCull) return 1;

    //I really really really don't want to figure out how to move a frustrm around space
    //so instead of moving it, i'm going to move objects around it
    glm::vec3 globpos =  thprop->position-cam->position;

    // if(!nya.topFace.isInFrontOfPlane(thprop->position)) return 0;
    // if(!nya.bottomFace.isInFrontOfPlane(thprop->position)) return 0;

    if(!nya.leftFace.isInFrontOfPlane(globpos)) return 0;
    if(!nya.rightFace.isInFrontOfPlane(globpos)) return 0;

    if(!nya.nearFace.isInFrontOfPlane(globpos)) return 0;
    if(!nya.farFace.isInFrontOfPlane(globpos)) return 0;

    return 1;
}

std::unique_ptr<v8::Platform> platform;
v8::Isolate::CreateParams create_params;
v8::Isolate* isolate;
v8::Local<v8::String> source;
v8::Local<v8::Context> context;
v8::Local<v8::Script> script;
v8::Local<v8::Value> resultz;

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(isolate);
        if (first) {
        first = false;
        } else {
        printf(" ");
        }
        v8::String::Utf8Value str(isolate, args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}

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
    }
}

v8::Persistent<v8::ObjectTemplate> vec2_templ;

v8::Local<v8::Object> quick_create_vector2(glm::vec2 vvinput){

    if (vec2_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));;
        vec2_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec2_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec2_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate, vvinput.x));
    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate, vvinput.y));

    return vec2_obj;
}

v8::Local<v8::Object> v8_vector2(const v8::FunctionCallbackInfo<v8::Value>& args){

    if (vec2_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Number::New(isolate,0));
        local->Set(isolate, "y", v8::Number::New(isolate,0));
        vec2_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec2_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec2_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber()) {
        vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), args[0]);
        vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), args[1]);
    }

    return vec2_obj;
}

void Vector2Constructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);


    if(!args[0].IsEmpty() && args[0]->IsNumber() &&
        !args[1].IsEmpty() && args[1]->IsNumber()) {
        args.GetReturnValue().Set(v8_vector2(args));
    }
}

glm::vec3 foo = glm::vec3( 0.0f,  0.0f,  0.0f);

int away = 0; //size
vector<texture> ctextures;

void SetTextureFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        ctextures[MID].load((char*)cstr);
    }
}

void TextureConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> texture_templ;

    texture texturecl;

    texturecl.name = "texture_"+to_string(away);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        texturecl.load((char*)cstr);
    }

    ctextures.push_back(texturecl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, away));
    local->Set(isolate, "GetFromFile", v8::FunctionTemplate::New(isolate, SetTextureFile));
    away++;
    texture_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> texture_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, texture_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(texture_obj);
}

int awaz = 0; //size
vector<model> cmodels;

void SetModelOBJ(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    cout << "setted was the model " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        cmodels[MID].load_obj(get_file(cstr));
        cout << "fom model " << cmodels[MID].VBO << "\ndn "<< cstr << endl;
    }
}

/*
 * this is the models
 * TODO:
 * multi vertex poly to 3 vertex poly inator
 * giv'm bones
 * allow models to be genorated and modified through Script
 */

void ModelConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> model_templ;

    model modecl;

    modecl.name = "moodel "+to_string(awaz);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        modecl.load_obj(get_file(cstr));
        //cout << "wa " << cstr << endl;
        //cout << "setted was the model " << modecl.VBO << endl;
    }

    cmodels.push_back(modecl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awaz));
    local->Set(isolate, "GetFromOBJ", v8::FunctionTemplate::New(isolate, SetModelOBJ));
    awaz++;
    model_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> model_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, model_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(model_obj);
}

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
        shadercl.craft(get_file(cstr));
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

    if(part[valueb].has_physbody){
        btTransform transform;
        transform.setIdentity();

        transform.setOrigin(btVector3(part[valueb].position.x,part[valueb].position.y,part[valueb].position.z));

        part[valueb].phys_counterpart->setWorldTransform(transform);
        part[valueb].phys_counterpart->getMotionState()->setWorldTransform(transform);
    }
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
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.x));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.y));
    vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.z));


  info.GetReturnValue().Set(vec3_obj);
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
}


static void Setspecular(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].speculars = &ctextures[dock];
}

static void Getmodel(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

//     if (vec3_templ.IsEmpty()) {
//         v8::EscapableHandleScope inner(isolate);
//         v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
//         local->Set(isolate, "x", v8::Integer::New(isolate,0));
//         local->Set(isolate, "y", v8::Integer::New(isolate,0));
//         local->Set(isolate, "z", v8::Integer::New(isolate,0));
//         vec3_templ.Reset(isolate, inner.Escape(local));
//     }
//
//     v8::Local<v8::Object> vec3_obj =
//       v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
//           ->NewInstance(isolate->GetCurrentContext())
//           .ToLocalChecked();
//
//     int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
//     int valueb = valuea;
//     vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.x));
//     vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.y));
//     vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.z));
//
//
//   info.GetReturnValue().Set(vec3_obj);
}


static void Setmodel(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int valueb = info.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  int dock = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

  part[valueb].models = &cmodels[dock];
}

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

    if(part[valueb].has_physbody){
    //getLinearVelocity
        btVector3 lva = part[valueb].phys_counterpart->getLinearVelocity();

        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,lva.getX()));
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,lva.getY()));
        vec3_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "z").ToLocalChecked(), v8::Number::New(isolate,lva.getZ()));

    }


  info.GetReturnValue().Set(vec3_obj);
}


static void Setpvl(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    if(part[valueb].has_physbody){
        int xp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        int yp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        int zp = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

        part[valueb].phys_counterpart->forceActivationState(ACTIVE_TAG);
        part[valueb].phys_counterpart->setLinearVelocity(btVector3(xp,yp,zp));
    }
}

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
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "model").ToLocalChecked(), Getmodel, Setmodel, v8::Integer::New(isolate,awax));
    local->Set(v8::String::NewFromUtf8(isolate, "create_physbody").ToLocalChecked(), v8::FunctionTemplate::New(isolate, create_physbody));
    local->Set(v8::String::NewFromUtf8(isolate, "AddForce").ToLocalChecked(), v8::FunctionTemplate::New(isolate, add_force));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "LinearVelocity").ToLocalChecked(), Getpvl, Setpvl, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "mass").ToLocalChecked(), Getpmass, Setpmass, v8::Integer::New(isolate,awax));

    awax++;
    prop_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> prop_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, prop_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(prop_obj);
}

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
    model = glm::rotate(model, cams[valueb].front.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, 1.0f));
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

double pi = 2 * acos(0.0);

static void Setvec3f(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    cams[valueb].front.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()*(pi/180);

    cams[valueb].front.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()*(pi/180);

    cams[valueb].front.z = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust()*(pi/180);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, cams[valueb].front.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-cams[valueb].position.x, -cams[valueb].position.y, -cams[valueb].position.z));
    cams[valueb].view = model;
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
        model = glm::rotate(model, cams[valueb].front.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, 1.0f));
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

int fontpogs = 0; //size
vector<font> cfonts;

void GetFontFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    cout << "setted was the font " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        cfonts[MID].craft(cstr);
    }
}

void FontConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> font_templ;

    font fontcl;

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        fontcl.craft(get_file(cstr));
        //cout << "wa " << cstr << endl;
        //cout << "setted was the model " << modecl.VBO << endl;
    }

    cfonts.push_back(fontcl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, fontpogs));
    local->Set(isolate, "GetFontFile", v8::FunctionTemplate::New(isolate, GetFontFile));
    fontpogs++;
    font_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> font_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, font_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(font_obj);
}

int awaeex = 0; //size
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

    screen_elements[valueb].text.position.x = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    screen_elements[valueb].text.position.y = value->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();
}

static void Getelts(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(isolate);

    if (vec2_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec2_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, vec3_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].scale.x));
    vec2_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].scale.y));


  info.GetReturnValue().Set(vec2_obj);
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

    info.GetReturnValue().Set(quick_create_vector3(cams[valueb].position));
}

void MenuElementConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> MenuElement_templ;

    element elpt;

    elpt.text.tfont = &base_font;
    elpt.text.shaderz = &text_base_shader;
    elpt.text.position = glm::vec2(25.0f, 25.0f);
    elpt.text.scale = 0.3f;
    elpt.text.color = glm::vec3(1.0, 1.0f, 1.0f);
    elpt.text.text = "";

    screen_elements.push_back(elpt);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getelvec2, Setelvec2, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text_size").ToLocalChecked(), Getelts, Setelts, v8::Integer::New(isolate,awaeex));
//     local->SetAccessor(v8::String::NewFromUtf8(isolate, "font").ToLocalChecked(), Getelfont, Setelfont, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text").ToLocalChecked(), Geteltext, Seteltext, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text_font").ToLocalChecked(), Getelfont, Setelfont, v8::Integer::New(isolate,awaeex));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "text_color").ToLocalChecked(), Getelcolor, Setelcolor, v8::Integer::New(isolate,awaeex));

    MenuElement_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> MenuElement_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, MenuElement_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(MenuElement_obj);
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

/*
 * how does this event thing work you may ask?
 *
 * here we have a box, each one is filled with functions
 * we add functions to these boxes by doing "Input.addEventListener("keydown", buttonpushedpog)"
 *  ____________        ____________
 * |            |      |            |
 * |            |      |            |
 * |    awa     |      |    pog     |
 * |            |      |            |
 * |____________|      |____________|
 *
 * this is for triggering a lot of functions depending which box you choose
 */

class inputevent_feld{
public:
    char* call_on;
    vector<v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> func;
};

int entvcount = -1;
vector<inputevent_feld> event_calls;

int find_inputevent(char* name) { //TODO: rip phonebook in half || hash it
    if(entvcount != -1) {
        for(int i = 0; i <= entvcount; i++){
            if(strcmp(event_calls[i].call_on, name) == 0){
                return i;
            }
        }
        return -1;
    }else{
        return -1;
    }
}

int one_box_please(char* name) { //but hold the functions
    if(find_inputevent(name) == -1) {
        inputevent_feld newt; //make new box

        newt.call_on = name; //lable new box

        entvcount++;
        event_calls.push_back(newt); //then put the box up in to storage for later
        return entvcount;
    }else{
        return -1;
    }

}

void AddInputEvent( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);

    if (args[1]->IsFunction() && args[0]->IsString()) {

        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);

        int dex = find_inputevent((char*)cstr);
        if(dex != -1) {
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
            event_calls[dex].func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(isolate, callback));
        }else{
            //so we can't find a box with the lable, so we need a new one :3
            inputevent_feld newt; //make new box

            newt.call_on = (char*)cstr; //lable new box

            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]); //grab the function out of the arguments
            newt.func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(isolate, callback)); //plop the function in to the box we made, after converting it to persistent

            entvcount++;
            event_calls.push_back(newt); //then put the box up in to storage for later
        }
    }

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

void run_cmd(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::HandleScope handle_scope(isolate);

    v8::String::Utf8Value str(isolate, args[0]);

    string c = ToCString(str);

    FILE *fp,*outputfile;
    char var[40];

    string last = "";

    fp = popen(ToCString(str), "r");
    while (fgets(var, sizeof(var), fp) != NULL)
        {
        last+=var;
        }
    pclose(fp);

    const char *cv = last.c_str();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, cv).ToLocalChecked());

    fflush(stdout);
}

void RayConstruct( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);

    float xp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float yp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float zp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float xp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float yp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float zp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    btCollisionWorld::ClosestRayResultCallback res(btVector3(xp1,yp1,zp1), btVector3(xp2,yp2,zp2));

    dynamicWorld->rayTest(btVector3(xp1,yp1,zp1), btVector3(xp2,yp2,zp2), res);
    v8::Local<v8::Object> Raycast_obj = v8::Object::New(isolate);

    if(res.hasHit()){
//         printf("Collision at: <%.2f, %.2f, %.2f>\n", res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ());

        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "hasHit").ToLocalChecked(), v8::Boolean::New(isolate, 1));

        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), quick_create_vector3(glm::vec3(res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ())));
    }else{
        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "hasHit").ToLocalChecked(), v8::Boolean::New(isolate, 0));
    }

    args.GetReturnValue().Set(Raycast_obj);
}

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

    //raycast
    global->Set(isolate, "RayCast", v8::FunctionTemplate::New(isolate, RayConstruct));

    //Cameras
    global->Set(isolate, "Camera", v8::FunctionTemplate::New(isolate, CameraConstructor));
    global->Set(isolate, "SetMainCam", v8::FunctionTemplate::New(isolate, set_main_cam));

    //fonts
    global->Set(isolate, "Font", v8::FunctionTemplate::New(isolate, FontConstructor));

    //Menu Elements
    global->Set(isolate, "element", v8::FunctionTemplate::New(isolate, MenuElementConstructor));

    //input
    v8::Local<v8::ObjectTemplate> input_b = v8::ObjectTemplate::New(isolate);
    input_b->Set(isolate, "addEventListener", v8::FunctionTemplate::New(isolate, AddInputEvent));
    global->Set(isolate, "Input", input_b);

    global->Set(isolate, "cmd", v8::FunctionTemplate::New(isolate, run_cmd));

    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));
    global->Set(isolate, "Win_title", v8::FunctionTemplate::New(isolate, Win_title));
    global->Set(isolate, "version", v8::FunctionTemplate::New(isolate, Version));

    global->SetAccessor(v8::String::NewFromUtf8(isolate, "innerWidth").ToLocalChecked(), GetscreenWidth, SetscreenWidth);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "innerHeight").ToLocalChecked(), GetscreenHeight, SetscreenHeight);
    global->Set(isolate, "CursorPin", v8::FunctionTemplate::New(isolate, toggle_curser_pin));

    return v8::Context::New(isolate, NULL, global);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

class keycalled{
public:
    int key;
    int action;
};

//TODO: write a key GOOD keymanager
//basicly to allow to detect multable keypresses
//add mouse buttons stuff too
int key_event_id = one_box_please("key"); //just dosn't care
int keypress_event_id = one_box_please("keypress"); //GLFW_PRESS
int keyrelease_event_id = one_box_please("keyrelease"); //GLFW_RELEASE
int keyhold_event_id = one_box_please("keyhold"); //GLFW_REPEAT
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[key_event_id].func) {
        v8::TryCatch trycatch(isolate);
        v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
    }
    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keypress_event_id].func) {
        v8::TryCatch trycatch(isolate);
        if(action == GLFW_PRESS){
        v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
        }
    }
    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keyrelease_event_id].func) {
        v8::TryCatch trycatch(isolate);
        if(action == GLFW_RELEASE){
        v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
        }
    }
    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keyhold_event_id].func) {
        v8::TryCatch trycatch(isolate);
        if(action == GLFW_REPEAT){
        v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
        }
    }
}

void windowSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    glUseProgram(text_base_shader.program);
    glUniformMatrix4fv(glGetUniformLocation(text_base_shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

v8::Persistent<v8::ObjectTemplate> mouse_templ;

int mousemove_event_id = one_box_please("mousemove");
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[mousemove_event_id].func) {
        v8::TryCatch trycatch(isolate);

        v8::HandleScope handle_scope(isolate);

    if (mouse_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "xpos", v8::Integer::New(isolate,0));
        local->Set(isolate, "ypos", v8::Integer::New(isolate,0));
        mouse_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> mouse_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, mouse_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

    mouse_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "xpos").ToLocalChecked(), v8::Number::New(isolate, xpos));
    mouse_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "ypos").ToLocalChecked(), v8::Number::New(isolate, ypos));

    v8::Local<v8::Value> wack = v8::Local<v8::Value>::Cast(mouse_obj);

    v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &wack);
    }
}

cameras pogger;

int main(int argc, char* argv[]) {
    pogger.projection = glm::perspective(glm::radians(pogger.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, pogger.near, pogger.far);

    main_cam = &pogger;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Kitsune Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLFWimage images[1];
    images[0].pixels = stbi_load("./engine_dependent/icon.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);

    glfwMakeContextCurrent(window);

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //setup defalt assets
    
    text_base_shader.craft("./engine_dependent/text_fonts/shader/base");

    base_font.craft("./fonts/ComicSans.ttf");

    shader waffle;
    waffle.craft("./shaders/sample");

    cubez.load_obj(get_file("./engine_dependent/ellie_def/pig.obj"));

    tex.flipped = false;
    tex.load("./engine_dependent/ellie_def/PiggoTexture.png");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    //physics - bullet phys world setup
    collisionConfiguration = new btDefaultCollisionConfiguration();

    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    overlappingPairCache = new btDbvtBroadphase();

    solver = new btSequentialImpulseConstraintSolver;

    dynamicWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicWorld->setGravity(btVector3(0,-10,0));

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0,0,0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0,1,0),0);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.f,motion,plane);
    btRigidBody* body = new btRigidBody(info);
    dynamicWorld->addRigidBody(body);
    bodies.push_back(body);

    //js - v8 env setup
    v8::V8::InitializeICUDefaultLocation("");
    v8::V8::InitializeExternalStartupData("");
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);

    v8::Isolate::Scope isolate_scope(isolate);

    v8::HandleScope handle_scope(isolate);

    context = load_wrap_functions(isolate);

    v8::Context::Scope context_scope(context);

    string raw_js = get_file("./index.js");
    source = v8::String::NewFromUtf8(isolate, raw_js.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    script = v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    resultz = script->Run(context).ToLocalChecked();



    //run script's init, at start
//     v8::Local<v8::Value> foo_value = context->Global()->Get(context, v8::String::NewFromUtf8(isolate, "init").ToLocalChecked()).ToLocalChecked();
//     if (foo_value->IsFunction()) {
//         // argument (string)
//         v8::Local<v8::Value> targ = v8::Integer::New(isolate, 0);
//
//         {
//             v8::TryCatch trycatch(isolate);
//             v8::Local<v8::Object> foo_object = foo_value.As<v8::Object>();
//             v8::MaybeLocal<v8::Value> foo_result = v8::Function::Cast(*foo_object)->Call(context, context->Global(), 1, &targ);
//         }
//     }

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input controlays
        processInput(window);

        //background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //run script's tick, every frame
        v8::Local<v8::Value> foo_value = context->Global()->Get(context, v8::String::NewFromUtf8(isolate, "tick").ToLocalChecked()).ToLocalChecked();
        if (foo_value->IsFunction()) {
            // argument (string)
            v8::Local<v8::Value> targ = v8::Integer::New(isolate, deltaTime);

            {
                v8::TryCatch trycatch(isolate);
                v8::Local<v8::Object> foo_object = foo_value.As<v8::Object>();
                v8::MaybeLocal<v8::Value> foo_result = v8::Function::Cast(*foo_object)->Call(context, context->Global(), 1, &targ);
            }
        }

        dynamicWorld->stepSimulation(1.f / 60.f ,10);

//         for(int v=dynamicWorld->getNumCollisionObjects()-1; v>=0;v--){
//             btCollisionObject* obj = dynamicWorld->getCollisionObjectArray()[v];
//             btRigidBody* body = btRigidBody::upcast(obj);
//             btTransform trans; // :3
//             if(body && body->getMotionState()){
//                 body->getMotionState()->getWorldTransform(trans);
//             }else{
//                 trans = obj->getWorldTransform();
//             }
//             printf("world pos object %d = %f, %f, %f\n", v, float(trans.getOrigin().getX()), float (trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
//         }


//         pogger.view = glm::lookAt(pogger.position, pogger.position + pogger.front, pogger.up);


        int NR_POINT_LIGHTS = 4; //size
        vector<pointLight> pointLights;

        glm::vec3 pointposes[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
        };

        for(int liwak = 0; liwak < NR_POINT_LIGHTS; liwak++){
            pointLight temp;
            temp.position = pointposes[liwak];
            pointLights.push_back(temp);
        }

        main_cam->projection = glm::perspective(glm::radians(main_cam->fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, main_cam->near, main_cam->far);

        //dynamicWorld->stepSimulation(1.f / 60.f ,10);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(waffle.program);

        waffle.setVec3("viewPos", main_cam->position);
        waffle.setInt("material.diffuse", 0);
        waffle.setInt("material.specular", 1);
        waffle.setFloat("material.shininess", 32.0f);

        // directional light
        waffle.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        waffle.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        waffle.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        waffle.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        waffle.setInt("NR_POINT_LIGHTS", NR_POINT_LIGHTS);

        for(int liwack = 0; liwack < NR_POINT_LIGHTS; liwack++){
            string a = "pointLights[" + std::to_string(liwack);
            string po = a+"].position";
            glUniform3fv(glGetUniformLocation(waffle.program, po.c_str()), 1, &pointLights[liwack].position[0]);
            string am = a+"].ambient";
            glUniform3fv(glGetUniformLocation(waffle.program, am.c_str()), 1, &pointLights[liwack].ambient[0]);
            string di = a+"].diffuse";
            glUniform3fv(glGetUniformLocation(waffle.program, di.c_str()), 1, &pointLights[liwack].diffuse[0]);
            string sp = a+"].specular";
            glUniform3fv(glGetUniformLocation(waffle.program, sp.c_str()), 1, &pointLights[liwack].specular[0]);
            string co = a+"].constant";
            glUniform1f(glGetUniformLocation(waffle.program, co.c_str()), pointLights[liwack].constant);
            string li = a+"].linear";
            glUniform1f(glGetUniformLocation(waffle.program, li.c_str()), pointLights[liwack].linear);
            string qu = a+"].quadratic";
            glUniform1f(glGetUniformLocation(waffle.program, qu.c_str()), pointLights[liwack].quadratic);
        }


        waffle.setMat4("projection", main_cam->projection);
        waffle.setMat4("view", main_cam->view);

        for (prop& i : part) {
            //parody prop physics counterpart
            if(i.has_physbody && i.mass != 0){
                btTransform trans; // :3
                if(i.phys_counterpart && i.phys_counterpart->getMotionState()){
                    i.phys_counterpart->getMotionState()->getWorldTransform(trans);

                    i.position = glm::vec3(trans.getOrigin().getX(),trans.getOrigin().getY(),trans.getOrigin().getZ());

                    btVector3 btaxis = i.phys_counterpart->getWorldTransform().getRotation().getAxis();
                    i.rotation = glm::vec3(btaxis.getX()/360,btaxis.getY()/360,btaxis.getZ()/360);
                }

            }

            //to render
            if(i.models != NULL)
            if(i.allow_render)
            if(check_cull(main_cam, &i))
            i.use(&waffle);
        }

        for (element& s : screen_elements) {
            if(s.allow_render)
                s.render();
        }


        //passes image to glfw I think
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for(int dc=dynamicWorld->getNumCollisionObjects()-1; dc>0; dc--){
        btCollisionObject* obj = dynamicWorld->getCollisionObjectArray()[dc];
        btRigidBody* body = btRigidBody::upcast(obj);
        if(body && body->getMotionState()){
            delete body->getMotionState();
        }
        dynamicWorld->removeCollisionObject(obj);
        delete obj;
    }

    //     for(int jp;jp<collisionShapes.size();jp++){
    //         btCollisionShape* shape = collisionShapes[jp];
    //         collisionShapes[jp] = 0;
    //         delete shape;
    //     }

    delete dynamicWorld;

    delete solver;

    delete overlappingPairCache;

    delete dispatcher;

    delete collisionConfiguration;

    //     collisionShapes.clear();

    for (int xa = 0; xa < awaz; xa++) {
        cmodels[xa].del();
    }
    glDeleteProgram(waffle.program);

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::DisposePlatform();
    delete create_params.array_buffer_allocator;

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

//     float cameraSpeed = static_cast<float>(2.5 * deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         pogger.position += cameraSpeed * pogger.front;
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         pogger.position -= cameraSpeed * pogger.front;
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         pogger.position -= glm::normalize(glm::cross(pogger.front, pogger.up)) * cameraSpeed;
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         pogger.position += glm::normalize(glm::cross(pogger.front, pogger.up)) * cameraSpeed;
}
