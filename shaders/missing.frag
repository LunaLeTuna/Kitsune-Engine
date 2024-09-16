#version 410

in vec3 local;
in vec3 v_normal;
in vec3 v_position;
in vec2 v_tex_coords;

uniform vec3 u_light;
uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform vec3 Color;
uniform mat4 view;
uniform float trans;
uniform float time;

out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

void main() {

    color = vec4(Color+normalize(v_normal)-0.4, trans);
    
}