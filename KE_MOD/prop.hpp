#pragma once

class prop{
    public:
    //render
    bool allow_render = 1;
    bool default_shader = 1;
    string name;
    shader* shaders;
    model* models;
    texture* textures;
    texture* speculars;
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 scale = glm::vec3( 1.0f,  1.0f,  1.0f);
    glm::vec3 rotation = glm::vec3( 0.0f,  0.0f,  0.0f);
    float angle = 0.0f;
    
    //cull
    bool skipCull = 0;
    float CullSize = 1;
    
    //phys
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