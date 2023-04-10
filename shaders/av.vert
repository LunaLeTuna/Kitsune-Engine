#version 150

in vec3 position;
in vec3 normal;
in vec2 tex_coords;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform float poggers;

void main() {

    v_position = vec3(model * vec4(position * sin(poggers), 1.0));
    v_normal = mat3(transpose(inverse(model))) * normal;
    v_tex_coords = tex_coords;

    gl_Position = perspective * view * vec4(v_position, 1.0);
}