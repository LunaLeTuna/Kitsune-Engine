#version 330 core
uniform sampler2D screenbuffer;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
out vec4 FragColor;

void main() {

    vec2 moewmoemwo = vec2(0.0,0.0);
    //moewmoemwo.y = (sin(v_tex_coords.y*20+time*0.02)*0.02);

    FragColor = texture(screenbuffer, v_tex_coords+moewmoemwo).rgba;
    // FragColor = vec4(FragColor.g*0.0001,FragColor.r*0.6,FragColor.b,1.0);
    
}