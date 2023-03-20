#version 140

in vec3 v_normal;
in vec3 v_position;
in vec2 v_tex_coords;

out vec4 color;

uniform vec3 u_light;
uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;

const vec3 specular_color = vec3(1.0, 1.0, 1.0);

void main() {

   color = vec4(1.0, 0.0, 0.0, 1.0);
}