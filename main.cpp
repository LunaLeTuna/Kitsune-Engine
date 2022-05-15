#define GL_GLEXT_PROTOTYPES
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

#include <include/v8.h>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow* window;

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

class cameras{
    public:
    string name;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
};

cameras* main_cam;

class shader{
    public:
    int success;

    string name;
    unsigned int vertex;
    unsigned int fragment;
    unsigned int program; //id or what

    void craft(string location){

        char infoLog[621];

        string va = get_file(location+".vert");

        const char *vav = va.c_str();

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vav, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        string ay = get_file(location+".frag");

        const char *aya = ay.c_str();

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &aya, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

            program = glCreateProgram();
            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            glLinkProgram(program);

            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(program, 621, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            glDeleteShader(vertex);
            glDeleteShader(fragment);
    }

        void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

};

class texture{
    public:
    int success;

    string name;

    unsigned int texture_id; //id or what
    int width, height, nrChannels;
    bool flipped;

    void load(char *location) {

        // load and create a texture
        // -------------------------
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        stbi_set_flip_vertically_on_load(flipped);
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char *data = stbi_load(location, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    void call(){
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

};

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class model{
    public:
    string name;
    unsigned int VBO, VAO;
    int rcount;
    vector<Vertex> vertices;

    void cube() {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void load_obj(string code){


        vector<glm::fvec3> verts;
        vector<glm::fvec2> texcords;
        vector<glm::fvec3> normals;

        vector<GLint> vf;
        vector<GLint> tf;
        vector<GLint> nf;

        string mode = "owo";
        int load = 1;

        glm::vec3 temp_vec3;
        glm::vec2 temp_vec2;
        GLint temp_glint = 0;

        stringstream ss;
        stringstream src(code);
        string line;
        string prefix;
        while(getline(src,line)){

            ss.clear();
            ss.str(line);
            ss >> prefix;

            if(prefix == "#"){

            }else if(prefix == "o"){

            }else if(prefix == "s"){

            }else if(prefix == "use_mtl"){

            }else if(prefix == "v"){ //vertexs
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                verts.push_back(temp_vec3);
            }else if(prefix == "vt"){ //
                ss >> temp_vec2.x >> temp_vec2.y;
                texcords.push_back(temp_vec2);
            }else if(prefix == "vn"){
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                normals.push_back(temp_vec3);
            }else if(prefix == "f"){
                int counter = 0;
                while(ss >> temp_glint){
                    if(counter == 0)
                        vf.push_back(temp_glint);
                    else if(counter == 1)
                        tf.push_back(temp_glint);
                    else if(counter == 2)
                        nf.push_back(temp_glint);

                    if(ss.peek() == '/'){
                        ++counter;
                        ss.ignore(1,'/');
                    }else if(ss.peek() == ' '){
                        ++counter;
                        ss.ignore(1, ' ');
                    }

                    //Reset the counter
                    if(counter > 2)
                        counter = 0;
                }
            }else{

            }

            //cout << line << endl;
            //cout << "Nr of vertices: " << vf.size() << endl;
        }

        vertices.resize(vf.size(), Vertex());
        rcount = vertices.size();

        for(size_t i = 0; i < rcount; ++i) {
            vertices[i].position = verts[vf[i]-1];
            vertices[i].texcoord = texcords[tf[i]-1];
            vertices[i].normal = normals[nf[i]-1];
            //cout << i << "  x: " << vertices[i].position.x << "  y: " << vertices[i].position.y <<"  z: " << vertices[i].position.z << endl;
            //cout << i << "  tx: " << vertices[i].texcoord.x << "  ty: " << vertices[i].texcoord.y << endl;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        //cout << "  vau: " << VAO << endl;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // texture coord attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    }

    void del() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

class prop{
    public:
    string name;
    model* models;
    texture* textures;
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 rotation = glm::vec3( 0.0f,  0.0f,  0.0f);
    float angle = 0.0f;

    void use(shader* shader_program){
        glBindVertexArray(models->VAO);

        glActiveTexture(GL_TEXTURE0);
        textures->call();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        shader_program->setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, models->rcount);
    }
};

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
        first = false;
        } else {
        printf(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}

void Win_title(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
        first = false;
        } else {
        glfwSetWindowTitle(window, "Kitsune Engine");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        glfwSetWindowTitle(window, cstr);
    }
    fflush(stdout);
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion()).ToLocalChecked());
}

void Vector3Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    float tx = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    float ty = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    float tz = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    float vedy = sqrtf(tx*tx+ty*ty+tz*tz);

    args.Holder()->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,tx/vedy));
    args.Holder()->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,ty/vedy));
    args.Holder()->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,tz/vedy));

    args.GetReturnValue().Set(args.Holder());
}

v8::Persistent<v8::ObjectTemplate> vec3_templ;
void Vector3Constructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(args.GetIsolate());

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
      v8::Local<v8::ObjectTemplate>::New(args.GetIsolate(), vec3_templ)
          ->NewInstance(args.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

        if(!args[0].IsEmpty() && args[0]->IsNumber() &&
           !args[1].IsEmpty() && args[1]->IsNumber() &&
           !args[2].IsEmpty() && args[2]->IsNumber()) {
            vec3_obj->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "x").ToLocalChecked(), args[0]);
            vec3_obj->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "y").ToLocalChecked(), args[1]);
            vec3_obj->Set(args.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(args.GetIsolate(), "z").ToLocalChecked(), args[2]);
        }

        args.GetReturnValue().Set(vec3_obj);
}

glm::vec3 foo = glm::vec3( 0.0f,  0.0f,  0.0f);

int away = 0; //size
vector<texture> ctextures;

void SetTextureFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    int MID = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        const char* cstr = ToCString(str);
        ctextures[MID].load((char*)cstr);
    }
}

void TextureConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Persistent<v8::ObjectTemplate> texture_templ;

    texture texturecl;

    texturecl.name = "texture_"+to_string(away);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
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
      v8::Local<v8::ObjectTemplate>::New(args.GetIsolate(), texture_templ)
          ->NewInstance(args.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(texture_obj);
}

int awaz = 0; //size
vector<model> cmodels;

void SetModelOBJ(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    int MID = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    cout << "setted was the model " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
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
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Persistent<v8::ObjectTemplate> model_templ;

    model modecl;

    modecl.name = "moodel "+to_string(awaz);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
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
      v8::Local<v8::ObjectTemplate>::New(args.GetIsolate(), model_templ)
          ->NewInstance(args.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(model_obj);
}

vector<prop> props;

model cubez;
texture tex;

int awax = 0; //size
vector<prop> part;

static void Getvec3p(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(info.GetIsolate(), vec3_templ)
          ->NewInstance(info.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.x));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.y));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].position.z));


  info.GetReturnValue().Set(vec3_obj);
}


static void Setvec3p(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  part[valueb].position.x = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].position.y = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].position.z = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();
}

static void Getvec3r(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(info.GetIsolate(), vec3_templ)
          ->NewInstance(info.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.x));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.y));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.z));


  info.GetReturnValue().Set(vec3_obj);
}


static void Setvec3r(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  part[valueb].rotation.x = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].rotation.y = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].rotation.z = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();
}

static void Gettexture(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());
}


static void Settexture(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].textures = &ctextures[dock];
}

static void Getmodel(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());

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
//       v8::Local<v8::ObjectTemplate>::New(info.GetIsolate(), vec3_templ)
//           ->NewInstance(info.GetIsolate()->GetCurrentContext())
//           .ToLocalChecked();
//
//     int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
//     int valueb = valuea;
//     vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.x));
//     vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.y));
//     vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,part[valueb].rotation.z));
//
//
//   info.GetReturnValue().Set(vec3_obj);
}


static void Setmodel(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  int dock = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  part[valueb].models = &cmodels[dock];
}

/*
 * prop is just a frick ton of pointers to other resources
 * so we arn't wasting a load of resources giving every individual prop it's own model
 * yeah, you pretty much get it
 */

void PropConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Persistent<v8::ObjectTemplate> prop_templ;

    prop zpart;

    zpart.name = "woot"+to_string(awax);

    zpart.models = &cubez;
    zpart.textures = &tex;

    part.push_back(zpart);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), Getvec3p, Setvec3p, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "rotation").ToLocalChecked(), Getvec3r, Setvec3r, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "texture").ToLocalChecked(), Gettexture, Settexture, v8::Integer::New(isolate,awax));
    local->SetAccessor(v8::String::NewFromUtf8(isolate, "model").ToLocalChecked(), Getmodel, Setmodel, v8::Integer::New(isolate,awax));
    awax++;
    prop_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> prop_obj =
      v8::Local<v8::ObjectTemplate>::New(args.GetIsolate(), prop_templ)
          ->NewInstance(args.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(prop_obj);
}

// class cameras{
//     public:
//     glm::mat4 view = glm::mat4(1.0f);
//     glm::mat4 projection = glm::mat4(1.0f);
//     glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
//     glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
//     glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
// };

int awac = 0; //size
vector<cameras> cams;

static void Getvec3pc(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(info.GetIsolate(), vec3_templ)
          ->NewInstance(info.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].position.x));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].position.y));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].position.z));


  info.GetReturnValue().Set(vec3_obj);
}


static void Setvec3pc(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
  int valueb = valuea;

  cams[valueb].position.x = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  cams[valueb].position.y = -1*value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

  cams[valueb].position.z = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, cams[valueb].front.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, cams[valueb].position);
    cams[valueb].view = model;
}

static void Getvec3f(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {

    v8::HandleScope handle_scope(info.GetIsolate());

    if (vec3_templ.IsEmpty()) {
        v8::EscapableHandleScope inner(isolate);
        v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
        local->Set(isolate, "x", v8::Integer::New(isolate,0));
        local->Set(isolate, "y", v8::Integer::New(isolate,0));
        local->Set(isolate, "z", v8::Integer::New(isolate,0));
        vec3_templ.Reset(isolate, inner.Escape(local));
    }

    v8::Local<v8::Object> vec3_obj =
      v8::Local<v8::ObjectTemplate>::New(info.GetIsolate(), vec3_templ)
          ->NewInstance(info.GetIsolate()->GetCurrentContext())
          .ToLocalChecked();

    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "x").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].front.x));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "y").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].front.y));
    vec3_obj->Set(info.GetIsolate()->GetCurrentContext(), v8::String::NewFromUtf8(info.GetIsolate(), "z").ToLocalChecked(), v8::Number::New(isolate,cams[valueb].front.z));


  info.GetReturnValue().Set(vec3_obj);
}

double pi = 2 * acos(0.0);

static void Setvec3f(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
    int64_t valuea = static_cast<int64_t>(info.Data().As<v8::Integer>()->Value());
    int valueb = valuea;

    cams[valueb].front.x = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust()*(pi/180);

    cams[valueb].front.y = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust()*(pi/180);

    cams[valueb].front.z = value->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(info.GetIsolate()->GetCurrentContext()).FromJust()*(pi/180);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, cams[valueb].front.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cams[valueb].front.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, cams[valueb].position);
    cams[valueb].view = model;
}

void CamLookAt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    if(!args[0].IsEmpty()){
        int valueb = args.Holder()->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();


        float rxx = args[0]->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

        float ryy = args[0]->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

        float rzz = args[0]->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

        cams[valueb].view = glm::lookAt(cams[valueb].position, glm::vec3(rxx, ryy, rzz), cams[valueb].up);
    }
}

void CameraConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Persistent<v8::ObjectTemplate> camera_templ;

    cameras cpart;

    cpart.name = "woot"+to_string(awac);

    cpart.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

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
    v8::Local<v8::ObjectTemplate>::New(args.GetIsolate(), camera_templ)
        ->NewInstance(args.GetIsolate()->GetCurrentContext())
        .ToLocalChecked();

    args.GetReturnValue().Set(camera_obj);
}

void set_main_cam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    int dock = args[0]->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(args.GetIsolate()->GetCurrentContext()).FromJust();

    main_cam = &cams[dock];
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

int find_inputevent(char* name) { //TODO: rip phonebook in half
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
    v8::HandleScope handle_scope(args.GetIsolate());

    if (args[1]->IsFunction() && args[0]->IsString()) {

        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        const char* cstr = ToCString(str);

        int dex = find_inputevent((char*)cstr);
        if(dex != -1) {
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
            event_calls[dex].func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(args.GetIsolate(), callback));
        }else{
            //so we can't find a box with the lable, so we need a new one :3
            inputevent_feld newt; //make new box

            newt.call_on = (char*)cstr; //lable new box

            v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]); //grab the function out of the arguments
            newt.func.push_back(v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>(args.GetIsolate(), callback)); //plop the function in to the box we made, after converting it to persistent

            entvcount++;
            event_calls.push_back(newt); //then put the box up in to storage for later
        }
    }

}

// I hated everything from my birth name to my looks, for years before even went to actual school
// feeling like, if I say anything my parents would not like it. now i'm 16, my mom has said she would love me for who I am.
// finally tell her, to be shutdown "you know your a boy right?" "you've never had a problem before"

v8::Local<v8::Context> load_wrap_functions(v8::Isolate* isolate) {
    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

    //Vector3s :3
    global->Set(isolate, "Vector3", v8::FunctionTemplate::New(isolate, Vector3Constructor));

    //textures
    global->Set(isolate, "Texture", v8::FunctionTemplate::New(isolate, TextureConstructor));

    //models
    global->Set(isolate, "Model", v8::FunctionTemplate::New(isolate, ModelConstructor));

    //props
    global->Set(isolate, "Prop", v8::FunctionTemplate::New(isolate, PropConstructor));

    //Cameras
    global->Set(isolate, "Camera", v8::FunctionTemplate::New(isolate, CameraConstructor));
    global->Set(isolate, "SetMainCam", v8::FunctionTemplate::New(isolate, set_main_cam));

    //input
    v8::Local<v8::ObjectTemplate> input_b = v8::ObjectTemplate::New(isolate);
    input_b->Set(isolate, "addEventListener", v8::FunctionTemplate::New(isolate, AddInputEvent));
    global->Set(isolate, "Input", input_b);

    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));
    global->Set(isolate, "Win_title", v8::FunctionTemplate::New(isolate, Win_title));
    global->Set(isolate, "version", v8::FunctionTemplate::New(isolate, Version));

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

cameras pogger;



int main(int argc, char* argv[]) {
    pogger.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    main_cam = &pogger;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    shader waffle;
    waffle.craft("./shaders/sample");

    cubez.load_obj(get_file("./engine_dependent/ellie_def/pig.obj"));

    tex.flipped = false;
    tex.load("./engine_dependent/ellie_def/PiggoTexture.png");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    //js
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
    v8::Local<v8::Value> foo_value = context->Global()->Get(context, v8::String::NewFromUtf8(isolate, "init").ToLocalChecked()).ToLocalChecked();
    if (foo_value->IsFunction()) {
        // argument (string)
        v8::Local<v8::Value> targ = v8::Integer::New(isolate, 0);

        {
            v8::TryCatch trycatch(isolate);
            v8::Local<v8::Object> foo_object = foo_value.As<v8::Object>();
            v8::MaybeLocal<v8::Value> foo_result = v8::Function::Cast(*foo_object)->Call(context, context->Global(), 1, &targ);
        }
    }

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


//         pogger.view = glm::lookAt(pogger.position, pogger.position + pogger.front, pogger.up);


        glUseProgram(waffle.program);
        waffle.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        waffle.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        waffle.setVec3("lightPos", lightPos);
        waffle.setMat4("projection", main_cam->projection);
        waffle.setMat4("view", main_cam->view);


        for (prop& i : part) {
            i.use(&waffle);
        }


        //passes image to glfw I thing
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int xa = 0; xa < awaz; xa++) {
        cmodels[xa].del();
    }
    glDeleteProgram(waffle.program);

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    pogger.front = glm::normalize(front);
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
