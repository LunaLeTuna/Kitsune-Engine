#version 410
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text0;
uniform sampler2D text1;
uniform bool FlipX;
uniform bool FlipY;

void main()
{
   vec2 vtexCoords = TexCoords;
    if(FlipX)
       vtexCoords = vec2(vtexCoords.s, 1.0 - vtexCoords.t);
    if(FlipY)
       vtexCoords = vec2(1.0 - vtexCoords.s, vtexCoords.t);
   vec4 uva = texture2D(text1, vtexCoords);
   vec2 uv = vec2(uva.r, uva.g);
   color = texture(text0, vec2(uv.r, uv.g));
   if(color.a < 0.1)
        discard;
}
