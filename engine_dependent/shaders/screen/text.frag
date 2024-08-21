#version 330 core
uniform sampler2D texturez;
uniform sampler2D screenbufferdepth;
uniform float time;
uniform vec4 uv;
in vec2 v_tex_coords;
out vec4 FragColor;

void main() {

    //if(v_tex_coords.x > (sin(v_tex_coords.y*20+time*0.002)*0.02)+0.5)
    vec2 texturezsie = textureSize(texturez,0);

    vec2 screenbit = ((v_tex_coords*texturezsie)-vec2(0,texturezsie.y))/texturezsie;
    screenbit*=(uv.zw*texturezsie)/texturezsie;
    screenbit+=vec2(uv.x,-uv.y);
    FragColor = texture(texturez, screenbit).rgba;//vec4(screenbit.x,screenbit.y,0,1);
    //else
    //FragColor = vec4((texture(screenbufferdepth, v_tex_coords).rrr/5*2.3)-0.452,1.0);
    
}