#version 330 core
uniform sampler2D screenbuffer;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
out vec4 FragColor;

uniform bool horizontal = true;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {

    //if(v_tex_coords.x > (sin(v_tex_coords.y*20+time*0.002)*0.02)+0.5)
    FragColor = texture(screenbuffer, v_tex_coords).rgba;
    //else
    //FragColor = vec4((texture(screenbufferdepth, v_tex_coords).rrr/5*2.3)-0.452,1.0);
    
}