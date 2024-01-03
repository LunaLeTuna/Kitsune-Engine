#version 330 core
uniform sampler2D screenbuffer;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
out vec4 FragColor;
void main() {

    FragColor = texture(screenbuffer, v_tex_coords).rgba;
    
}