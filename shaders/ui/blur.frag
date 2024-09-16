#version 330 core
uniform sampler2D screenbuffer;
uniform sampler2D screenbufferdepth;
uniform float time;
in vec2 v_tex_coords;
uniform vec2 framebufferSize;
out vec4 FragColor;

//https://xorshaders.weebly.com/tutorials/blur-shaders-5-part-2

void main() {

    //moewmoemwo.y = (sin(v_tex_coords.y*20+time*0.02)*0.02);

    
    
    float Directions = 16.0;
    float Quality = 3.0;
    float Size = 8.0;
    float Pi = 6.28318530718; //pi*2
   
    vec2 Radius = Size/framebufferSize.xy;
    
    vec2 uv = gl_FragCoord.xy/framebufferSize.xy;
    vec4 Color = texture(screenbuffer, uv);
    
    for( float d=0.0; d<Pi; d+=Pi/Directions)
    {
		for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
        {
			Color += texture( screenbuffer, uv+vec2(cos(d),sin(d))*Radius*i);		
        }
    }

    Color /= Quality * Directions - 15.0;

    vec2 aawaw = vec2(1.0+(sin((gl_FragCoord.y*0.02)+time/200)*0.01),1.0);

    vec4 tecx = texture(screenbuffer, (gl_FragCoord.xy/framebufferSize.xy)*aawaw).rgba;

    FragColor = Color-0.2;
    // FragColor = vec4(FragColor.g*0.0001,FragColor.r*0.6,FragColor.b,1.0);
    
}