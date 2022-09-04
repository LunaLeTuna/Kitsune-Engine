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
};