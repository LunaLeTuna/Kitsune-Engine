#pragma once

class prop_attribute{
    public:
    string name;
    void* data;
    uint8_t type;
};

class prop{
    public:
    //render
    bool allow_render = 1;
    bool default_shader = 1;
    bool ignore_depth_test = 0;
    string name;
    shader* shaders;
    model* models;

    texture* imbase0;
    texture* imbase1;

    GLuint text0, text1;
    bool textset = false;

    bool has_shader_overide = 0;
    vector<shader_attribute> shader_overide_attributes;

    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 scale = glm::vec3( 1.0f,  1.0f,  1.0f);
    glm::vec3 rotation = glm::vec3( 0.0f,  0.0f,  0.0f);
    float angle = 0.0f;
    
    //cull
    bool skipCull = 0;
    float CullSize = 1;
    
#ifdef Include_physics
    //phys
    float mass = 0.0f;
    bool has_physbody = 0;
    btRigidBody* phys_counterpart;
#endif

void load_shader_prop_override_insert_attribute(shader* shader_program){
        for (auto& attribute : shader_overide_attributes){
            switch(attribute.type) {
                case 0:
                {
                    int* back = static_cast<int*>(attribute.data);
                    shader_program->setInt(attribute.name, *back);
                    break;
                }
                case 1:
                {
                    float* back = static_cast<float*>(attribute.data);
                    shader_program->setFloat(attribute.name, *back);
                    break;
                }
                case 2:
                {
                    glm::vec2* back = static_cast<glm::vec2*>(attribute.data);
                    shader_program->setVec2(attribute.name, back->x, back->y);
                    break;
                }
                case 3:
                {
                    glm::vec3* back = static_cast<glm::vec3*>(attribute.data);
                    shader_program->setVec3(attribute.name, back->x, back->y, back->z);
                    break;
                }
                case 5:
                {
                    bool* back = static_cast<bool*>(attribute.data);
                    shader_program->setBool(attribute.name, *back);
                    break;
                }
                default:
                    cout << "unknown attribute type in " << name << endl;
            }
        }
    }

    void audit_shader_override_insert_attribute(string name, void* data, uint8_t type){
        has_shader_overide = 1;
        for (auto& attribute : shader_overide_attributes){
            if(attribute.name.compare(name) == 0){
                attribute.data = data;
                attribute.type = type;
                return;
            }
        }
        shader_attribute temp;
        temp.name = name;
        temp.data = data;
        temp.type = type;
        shader_overide_attributes.push_back(temp);
    }

    void use(shader* shader_program){

        glBindVertexArray(models->VAO);

        if(!textset){
            glGenTextures(1, &text0);
            glBindTexture(GL_TEXTURE_2D, text0);
            glGenTextures(1, &text1);
            glBindTexture(GL_TEXTURE_2D, text1); 
        }

        glUniform1i(text0, 0);
        glUniform1i(text1,  1);

        glActiveTexture(GL_TEXTURE0);
        shader_program->setInt("text0", 0);
        imbase0->call();

        glActiveTexture(GL_TEXTURE1);
        shader_program->setInt("text1", 1);
        imbase1->call();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        shader_program->setMat4("model", model);

        if(has_shader_overide){
            load_shader_prop_override_insert_attribute(shader_program);
        }

        if(ignore_depth_test){
            glDepthFunc(GL_ALWAYS);
        }else{
            glDepthFunc(GL_LESS);
        }

        glDrawArrays(GL_TRIANGLES, 0, models->rcount);
    }
};