#version 330 core
uniform sampler2D texturez;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
out vec4 FragColor;

void main() {

    //moewmoemwo.y = (sin(v_tex_coords.y*20+time*0.02)*0.02);

    FragColor = texture(texturez, v_tex_coords).rgba;
    // FragColor = vec4(FragColor.g*0.0001,FragColor.r*0.6,FragColor.b,1.0);
    
}