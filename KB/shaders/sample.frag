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

uniform float y_scan;
uniform bool dont;

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float diff = max(dot(norm, dirLight.direction*-20), 3.0);

    vec3 result = (dirLight.ambient * diff) * color;

    float glow_depth = 3;

    if(dont)
    FragColor = vec4(result, 1);

    else if(FragPos.y >= y_scan){
    FragColor = vec4(result, 0);

    }else if(FragPos.y+glow_depth >= y_scan){
        float a = ((FragPos.y-(y_scan-glow_depth)));
        FragColor = vec4(result.r+a/4, result.g+a/3, result.b+a/2, 1);
    }
    else if(FragPos.y <= y_scan)
    FragColor = vec4(result, 1);
}