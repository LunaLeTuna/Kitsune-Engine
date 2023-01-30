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
uniform Material material;

uniform vec3 brick_size;
uniform float y_scan;
uniform bool dont;
uniform bool base;

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float diff = max(dot(norm, dirLight.direction*-20), 3.0);

    vec3 result = (dirLight.ambient * diff) * color;

    float glow_depth = 3;
    float dist = length(FragPos); //FragPos.y

    if(norm.y >= 0.01){
        result = vec3(texture(material.diffuse, vec2(TexCoords.x*brick_size.z*2,TexCoords.y*brick_size.x*2)))* color;
    }else if(base){
        result = vec3(texture(material.diffuse, vec2(TexCoords.x*brick_size.z,TexCoords.y*brick_size.x)))* color;
    }

    FragColor = vec4(result, 1);

    // FragColor = vec4(dist, 0, 0, 1);
}