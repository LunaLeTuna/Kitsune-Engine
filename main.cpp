#include <cstddef>
#define GL_GLEXT_PROTOTYPES
#define STB_IMAGE_IMPLEMENTATION

#define Include_physics
#define Include_console_interaction

#include <stdio.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.hpp"
 
 
#include <assert.h>
#include <fcntl.h>
#include <libplatform/libplatform.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include <map>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#ifdef Include_physics
#include <btBulletDynamicsCommon.h>
#endif

#include <v8.h>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float xpos_mouse = 0;
float ypos_mouse = 0;

GLFWwindow* window;

#ifdef Include_physics
btDefaultCollisionConfiguration* collisionConfiguration;

btCollisionDispatcher* dispatcher;

btBroadphaseInterface* overlappingPairCache;

btSequentialImpulseConstraintSolver* solver;

btDiscreteDynamicsWorld* dynamicWorld;
#endif

string get_file_date(string location){
    char time[50];
    struct stat attrib;
    
    stat(location.c_str(), &attrib);
    strftime(time, 50, "aaaa-%m-%d %H:%M:%S", localtime(&attrib.st_mtime));
    return time;
}

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

string ask_user_file(){ //needs winder implementation
    char filename[1024];
    FILE *f = popen("zenity --file-selection", "r");
    fgets(filename, 1024, f);
    return filename;
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
    char* locked;
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

int find_inputevent(char* name, char* locked) { //TODO: rip phonebook in half || hash it
    if(entvcount != -1) {
        for(int i = 0; i <= entvcount; i++){
            if(strcmp(event_calls[i].call_on, name) == 0 && strcmp(event_calls[i].locked, locked) == 0){
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

int one_box_please(char* name, char* locked) { //but hold the functions
    if(find_inputevent(name, locked) == -1) {
        inputevent_feld newt; //make new box

        newt.call_on = name; //lable new box
        newt.locked = locked;

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
    based.topFace.distance = -70.0f;

    based.bottomFace.normal = {0.0f, -1.0f, 0.0f};
    based.bottomFace.distance = -70.0f;

    based.leftFace.normal = {-1.0f, 0.0f, 0.0f};
    based.leftFace.distance = -70.0f;

    based.rightFace.normal = {1.0f, 0.0f, 0.0f};
    based.rightFace.distance = -70.0f;

    based.nearFace.normal = {0.0f, 0.0f, 1.0f};
    based.nearFace.distance = -70.0f;

    based.farFace.normal = {0.0f, 0.0f, -1.0f};
    based.farFace.distance = -70.0f;

    return based;
}


glm::vec3 RotateAroundPoint(glm::vec3 point, glm::vec3 axis, float radians){
  glm::vec3 rotatedPoint;
  float x = point.x;
  float y = point.y;
  float z = point.z;
  float a = axis.x;
  float b = axis.y;
  float c = axis.z;
  rotatedPoint.x = (a*(a*x+b*y+c*z)*(1-cos(radians))+x*cos(radians)+(-c*y+b*z)*sin(radians));
  rotatedPoint.y = (b*(a*x+b*y+c*z)*(1-cos(radians))+y*cos(radians)+(c*x-a*z)*sin(radians));
  rotatedPoint.z = (c*(a*x+b*y+c*z)*(1-cos(radians))+z*cos(radians)+(-b*x+a*y)*sin(radians));
  return rotatedPoint;
}

frustum nya = craftFrustum();

bool check_cull(cameras* cam, prop* thprop){
    if(thprop->skipCull) return 1;

    //I really really really don't want to figure out how to move a frustrm around space
    //so instead of moving it, i'm going to move objects around it
    glm::vec3 globpos =  thprop->position-cam->position;

    // globpos = RotateAroundPoint(globpos, glm::vec3(1.0f, 0.0f, 0.0f), cam->front.x);

    // if(!nya.topFace.isInFrontOfPlane(thprop->position)) return 0;
    // if(!nya.bottomFace.isInFrontOfPlane(thprop->position)) return 0;

    if(!nya.leftFace.isInFrontOfPlane(globpos)) return 0;
    if(!nya.rightFace.isInFrontOfPlane(globpos)) return 0;

    if(!nya.nearFace.isInFrontOfPlane(globpos)) return 0;
    if(!nya.farFace.isInFrontOfPlane(globpos)) return 0;

    //cout << thprop->name << -cam->front.x << " " << cam->front.y << " " << cam->front.z << endl;

    return 1;
}

#include "./V8_MOD/V8_Objects.hpp"
#include "./management_scripts/management.hpp"

glm::vec3 foo = glm::vec3( 0.0f,  0.0f,  0.0f);

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
int key_event_id = one_box_please("key", "key"); //just dosn't care
int keypress_event_id = one_box_please("keypress", "key"); //GLFW_PRESS
int keyrelease_event_id = one_box_please("keyrelease", "key"); //GLFW_RELEASE
int keyhold_event_id = one_box_please("keyhold", "key"); //GLFW_REPEAT
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[key_event_id].func) {
        v8::TryCatch trycatch(isolate);
        v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
    }
    if(action == GLFW_PRESS){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keypress_event_id].func) {
            v8::TryCatch trycatch(isolate);
            v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
            v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
        }
    }
    if(action == GLFW_RELEASE){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keyrelease_event_id].func) {
            v8::TryCatch trycatch(isolate);
            v8::Local<v8::Value> buttonkey = v8::Integer::New(isolate, key);
            v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonkey);
        }
    }
    if(action == GLFW_REPEAT){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[keyhold_event_id].func) {
            v8::TryCatch trycatch(isolate);
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

int mousemove_event_id = one_box_please("mousemove", "mouse");
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    xpos_mouse = xpos;
    ypos_mouse = ypos;

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

int mouse_event_id = one_box_please("mouse", "mouse"); //just dosn't care
int mousepress_event_id = one_box_please("mousepress", "mouse"); //GLFW_PRESS
int mouserelease_event_id = one_box_please("mouserelease", "mouse"); //GLFW_RELEASE
int mousehold_event_id = one_box_please("mousehold", "mouse"); //GLFW_REPEAT
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{

    for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[mouse_event_id].func) {
        v8::TryCatch trycatch(isolate);
        v8::Local<v8::Value> buttonmouse = v8::Integer::New(isolate, button);
        v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonmouse);
    }
    
    if(action == GLFW_PRESS){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[mousepress_event_id].func) {
            v8::TryCatch trycatch(isolate);
            
            v8::Local<v8::Value> buttonmouse = v8::Integer::New(isolate, button);
            v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonmouse);
        }
        for (element& s : screen_elements) {
            if(!s.not_used)
            if(s.is_button)
            if(s.allow_render)
                s.pressed_button();
        }
    }

    if(action == GLFW_RELEASE){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[mouserelease_event_id].func) {
            v8::TryCatch trycatch(isolate);
            v8::Local<v8::Value> buttonmouse = v8::Integer::New(isolate, button);
            v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonmouse);
        }
    }
    if(action == GLFW_REPEAT){
        for (v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>& entfs : event_calls[mousehold_event_id].func) {
            v8::TryCatch trycatch(isolate);
            v8::Local<v8::Value> buttonmouse = v8::Integer::New(isolate, button);
            v8::Local<v8::Function>::New(isolate, entfs)->Call(context, context->Global(), 1, &buttonmouse);
        }
    }
}

cameras pogger;

int main(int argc, char* argv[]) {

    pogger.projection = glm::perspective(glm::radians(pogger.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, pogger.Cnear, pogger.Cfar);

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

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_click_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //setup defalt assets
    
    image_base_shader.craft("./engine_dependent/menu_image/shader/base");
    text_base_shader.craft("./engine_dependent/text_fonts/shader/base");

    base_font.craft("./engine_dependent/fonts/Hack-Regular.ttf");

    shader waffle;
    waffle.craft("./engine_dependent/shaders/sample");
    cout << "meow" << endl;

    cubez.load_obj(get_file("./engine_dependent/ellie_def/pig.obj"));

    tex.flipped = false;
    tex.load("./engine_dependent/ellie_def/PiggoTexture.png");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


#ifdef Include_physics
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
    body->setFriction(1.0f);
    dynamicWorld->addRigidBody(body);
    bodies.push_back(body);
#endif

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

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int fbt;
    glGenTextures(1, &fbt);
    glBindTexture(GL_TEXTURE_2D, fbt);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbt, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    texture buffy;
    buffy.texture_id = fbt;
    element screen;
    screen.image.shaderz = &image_base_shader;
    screen.image.imbase = &buffy;
    screen.image.flipx = 1;
    screen.has_image = 1;

    while (!glfwWindowShouldClose(window)) {
        hotload();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input controlays
        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        main_cam->reproject();
        
#ifdef Include_physics
        dynamicWorld->stepSimulation(1.f / 60.f ,10);
#endif

        shader* current_shader = &waffle;

        glUseProgram(current_shader->program);

        current_shader->setVec3("viewPos", main_cam->position);
        current_shader->setInt("material.diffuse", 0);
        current_shader->setInt("material.specular", 1);
        current_shader->setFloat("material.shininess", 32.0f);

        // directional light
        current_shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        current_shader->setVec3("dirLight.ambient", 1.0f, 1.0f, 1.0f);
        current_shader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        current_shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        current_shader->setInt("NR_POINT_LIGHTS", NR_POINT_LIGHTS);

        for(int liwack = 0; liwack < NR_POINT_LIGHTS; liwack++){
            string a = "pointLights[" + std::to_string(liwack);
            string po = a+"].position";
            glUniform3fv(glGetUniformLocation(current_shader->program, po.c_str()), 1, &pointLights[liwack].position[0]);
            string am = a+"].ambient";
            glUniform3fv(glGetUniformLocation(current_shader->program, am.c_str()), 1, &pointLights[liwack].ambient[0]);
            string di = a+"].diffuse";
            glUniform3fv(glGetUniformLocation(current_shader->program, di.c_str()), 1, &pointLights[liwack].diffuse[0]);
            string sp = a+"].specular";
            glUniform3fv(glGetUniformLocation(current_shader->program, sp.c_str()), 1, &pointLights[liwack].specular[0]);
            string co = a+"].constant";
            glUniform1f(glGetUniformLocation(current_shader->program, co.c_str()), pointLights[liwack].constant);
            string li = a+"].linear";
            glUniform1f(glGetUniformLocation(current_shader->program, li.c_str()), pointLights[liwack].linear);
            string qu = a+"].quadratic";
            glUniform1f(glGetUniformLocation(current_shader->program, qu.c_str()), pointLights[liwack].quadratic);
        }


        current_shader->setMat4("projection", main_cam->projection);
        current_shader->setMat4("view", main_cam->view);

        unsigned int last_shader = current_shader->program;

        for (prop& i : part) {
#ifdef Include_physics

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
#endif

            if(i.default_shader) {
                i.shaders = &waffle;
                i.default_shader = 0;
            }

            if(i.shaders->program != last_shader) {
                
                last_shader = i.shaders->program;
                current_shader = i.shaders;
                glUseProgram(current_shader->program);

                current_shader->setVec3("viewPos", main_cam->position);
                current_shader->setInt("material.diffuse", 0);
                current_shader->setInt("material.specular", 1);
                current_shader->setFloat("material.shininess", 32.0f);

                // directional light
                current_shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
                current_shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
                current_shader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
                current_shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

                current_shader->setInt("NR_POINT_LIGHTS", NR_POINT_LIGHTS);

                for(int liwack = 0; liwack < NR_POINT_LIGHTS; liwack++){
                    string a = "pointLights[" + std::to_string(liwack);
                    string po = a+"].position";
                    glUniform3fv(glGetUniformLocation(current_shader->program, po.c_str()), 1, &pointLights[liwack].position[0]);
                    string am = a+"].ambient";
                    glUniform3fv(glGetUniformLocation(current_shader->program, am.c_str()), 1, &pointLights[liwack].ambient[0]);
                    string di = a+"].diffuse";
                    glUniform3fv(glGetUniformLocation(current_shader->program, di.c_str()), 1, &pointLights[liwack].diffuse[0]);
                    string sp = a+"].specular";
                    glUniform3fv(glGetUniformLocation(current_shader->program, sp.c_str()), 1, &pointLights[liwack].specular[0]);
                    string co = a+"].constant";
                    glUniform1f(glGetUniformLocation(current_shader->program, co.c_str()), pointLights[liwack].constant);
                    string li = a+"].linear";
                    glUniform1f(glGetUniformLocation(current_shader->program, li.c_str()), pointLights[liwack].linear);
                    string qu = a+"].quadratic";
                    glUniform1f(glGetUniformLocation(current_shader->program, qu.c_str()), pointLights[liwack].quadratic);
                }


                current_shader->setMat4("projection", main_cam->projection);
                current_shader->setMat4("view", main_cam->view);

                glActiveTexture(GL_TEXTURE2);
                current_shader->setInt("ScreenBuffer", 3);
                glBindTexture(GL_TEXTURE_2D, rbo);
            }

            current_shader->load_attribute();

            //to render
            if(i.models != NULL)
            if(i.allow_render)
            // if(check_cull(main_cam, &i))
            i.use(current_shader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, fbt);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbt, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

        screen.image.position = glm::vec2(SCR_WIDTH/2, SCR_HEIGHT/2);
        screen.image.scale = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
        screen.render();

        for (element& s : screen_elements) {
            if(!s.not_used)
            if(s.allow_render)
                s.render();
        }


        //passes image to glfw I think
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#ifdef Include_physics
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
    #endif

    //     collisionShapes.clear();

    for (int xa = 0; xa < awaz; xa++) {
        cmodels[xa].del();
    }
    glDeleteProgram(waffle.program);
    // glDeleteFramebuffers(1, &fbo);

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