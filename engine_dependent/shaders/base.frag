#version 410

in vec3 local;
in vec3 v_normal;
in vec3 v_position;
in vec2 v_tex_coords;

out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

layout(std140) uniform;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(std140) uniform;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight LeDirLight;

uniform int NR_POINT_LIGHTS;
uniform PointLight pointLights[64];

uniform Material material;
uniform float trans;


uniform vec3 u_light;
uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform vec3 Color;
uniform vec3 view;

const vec3 specular_color = vec3(1.0, 1.0, 1.0);

mat3 cotangent_frame(vec3 normal, vec3 pos, vec2 uv) {
   vec3 dp1 = dFdx(pos);
   vec3 dp2 = dFdy(pos);
   vec2 duv1 = dFdx(uv);
   vec2 duv2 = dFdy(uv);

   vec3 dp2perp = cross(dp2, normal);
   vec3 dp1perp = cross(normal, dp1);
   vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
   vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

   float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
   return mat3(T * invmax, B * invmax, normal);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, v_tex_coords).rgb);
    vec3 diffuse = light.diffuse * diff;// * vec3(texture(material.diffuse, v_tex_coords).rgb);
    vec3 specular = light.specular * spec;// * vec3(texture(material.specular, v_tex_coords).rgb);
    return (reflectDir);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, v_tex_coords).rgb);
    vec3 diffuse = light.diffuse * diff;// * vec3(texture(material.diffuse, v_tex_coords).rgb);
    vec3 specular = light.specular * spec;// * vec3(texture(material.diffuse, v_tex_coords).rgb);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse;
}

void main() {
    vec3 norm = normalize(v_normal);
    vec3 viewDir = normalize(view - v_position);

    vec3 result = LeDirLight.diffuse*0.1;

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
      result += CalcPointLight(pointLights[i], norm, v_position, viewDir);

    // if(local.y >= 0.8)
    // color = vec4(result, 1.0);
    // else if(local.x >= 0.8)
    // color = vec4(result*vec3(1.0,0.0,0.0), 1.0);
    // else
    color = vec4(result*Color, trans);
}