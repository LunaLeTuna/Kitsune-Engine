#version 330 core
uniform sampler2D screenbuffer;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
out vec4 FragColor;

void main() {

    //if(v_tex_coords.x > (sin(v_tex_coords.y*20+time*0.002)*0.02)+0.5)
    FragColor = texture(screenbuffer, v_tex_coords).rgba;
    //else
    //FragColor = vec4((texture(screenbufferdepth, v_tex_coords).rrr/5*2.3)-0.452,1.0);
    
}