#version 410

#define MAX_LIGHTS 128

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// struct SpotLight {
//     vec3 position;
//     vec3 direction;
//     float cutOff;
//     float outerCutOff;
//
//     float constant;
//     float linear;
//     float quadratic;
//
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform int NR_POINT_LIGHTS;
uniform PointLight pointLights[MAX_LIGHTS];

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    FragColor = vec4(color, 1.0);
}