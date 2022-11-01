#pragma once

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