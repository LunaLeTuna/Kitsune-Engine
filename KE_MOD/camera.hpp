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
    float far = 1000.0f;
    float fov = 60.0f;
    float aspect = 0;

    void refresh(){
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, front.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, front.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, front.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-position.x, -position.y, -position.z));
        view = model;
    }

    void reproject(){
        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, near, far);
    }

    void set_rotation(glm::vec3 sc){
        front = sc;
        refresh();
    }

    void look_at(glm::vec3 target){
        glm::mat4 trass = glm::lookAt(position, target, up);

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(trass, scale, rotation, translation, skew, perspective);

        rotation = glm::conjugate(rotation);

        front  = glm::eulerAngles(rotation);// * 3.14159f / 180.f;
        front = glm::vec3(-front.x,-front.y,front.z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, front.x, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, front.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, front.z, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::translate(model, glm::vec3(-position.x, -position.y, -position.z));
        view = model;
    }
};