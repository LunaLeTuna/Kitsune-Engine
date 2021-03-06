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

class cameras{
    public:
    string name;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::quat rotation;
    glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
};

cameras* main_cam;

class pointLight{
    public:
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

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
    bool allow_render = 1;
    string name;
    model* models;
    texture* textures;
    texture* speculars;
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 scale = glm::vec3( 1.0f,  1.0f,  1.0f);
    glm::vec3 rotation = glm::vec3( 0.0f,  0.0f,  0.0f);
    float angle = 0.0f;

    float mass = 0.0f;
    bool has_physbody = 0;
    btRigidBody* phys_counterpart;

    void use(shader* shader_program){
        glBindVertexArray(models->VAO);

        glActiveTexture(GL_TEXTURE0);
        textures->call();
        glActiveTexture(GL_TEXTURE1);
        speculars->call();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
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

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

shader text_base_shader;

class font{
public:
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
    string file_name;

    void craft(string fsn){

        if(fsn!=""){
            file_name = fsn;
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
        glUseProgram(text_base_shader.program);
        glUniformMatrix4fv(glGetUniformLocation(text_base_shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // FreeType
        // --------
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }

        // find path to font
        std::string font_name = file_name;
        if (font_name.empty())
        {
            std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
            return;
        }

        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return;
        }
        else {
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 48);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texturex;
                glGenTextures(1, &texturex);
                glBindTexture(GL_TEXTURE_2D, texturex);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texturex,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);


        // configure VAO/VBO for texture quads
        // -----------------------------------
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

font base_font;

class text_element{
public:
    font* tfont;
    shader* shaderz;
    string text;
    glm::vec3 color;
    float line_spaceing = 30;

    glm::vec2 position;
    float scale;

    bool overflow = -1; //-1 means off, -2 cuts off size, 0 and up cut off string

    void Render(){
        // activate corresponding render state

        int current_line = 0;

        float tx = position.x;
        float ty = position.y;

        glUseProgram(shaderz->program);
        glUniform3f(glGetUniformLocation(shaderz->program, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(tfont->VAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = tfont->Characters[*c];

            if(*c == '\n'){
                tx=position.x;
                current_line++;
                ty=position.y-(line_spaceing*current_line);
                continue;
            }

            float xpos = tx + ch.Bearing.x * scale;
            float ypos = ty - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, tfont->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            tx += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

class element{
public:
    bool has_text = 0;
    bool has_background = 0;
    glm::vec2 position;
    glm::vec2 size;

    text_element text;

    bool allow_render = 1;

    void set_text(string le_filler){
        has_text = 1;
        text.text = le_filler;
    }

    void render(){
//         if(has_background == 1){
//
//         }
        if(has_text == 1){
            text.Render();
        }
    }

};

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

    float valueb = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    part[valueb].phys_counterpart = create_physbox(part[valueb].position, glm::vec3(1,1,1), part[valueb].mass);
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
        glm::decompose(trass, scale, rotation, translation, skew,perspective);

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
    pogger.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

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

    text_base_shader.craft("./engine_dependent/text_fonts/shader/base");

    base_font.craft("./fonts/ComicSans.ttf");

    shader waffle;
    waffle.craft("./shaders/sample");

    cubez.load_obj(get_file("./engine_dependent/ellie_def/pig.obj"));

    tex.flipped = false;
    tex.load("./engine_dependent/ellie_def/PiggoTexture.png");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    //physics - bullet
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

    //js - v8
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

        main_cam->projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //dynamicWorld->stepSimulation(1.f / 60.f ,10);

        glEnable(GL_DEPTH_TEST);

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
            i.use(&waffle);
        }

        for (element& s : screen_elements) {
            if(s.allow_render)
                s.render();
        }


        //passes image to glfw I thing
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
