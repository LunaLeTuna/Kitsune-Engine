#pragma once

class cameras{
    public:
    string name;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3( 0.0f,  0.0f,  0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::quat rotation;
    glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
    
    float near = 0.1f;
    float far = 100.0f;
    float fov = 45.0f;
    float aspect = 0;

    void refresh(){
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, front.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, front.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, front.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-position.x, -position.y, -position.z));
        view = model;
    }

    void set_rotation(glm::vec3 sc){
        front = sc;
        refresh();
    }
};