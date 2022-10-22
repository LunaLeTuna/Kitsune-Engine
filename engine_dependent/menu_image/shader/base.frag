#version 410
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform bool FlipX;
uniform bool FlipY;

void main()
{
    vec2 vtexCoords = TexCoords;
    if(FlipX)
       vtexCoords = vec2(vtexCoords.s, 1.0 - vtexCoords.t);
    if(FlipY)
       vtexCoords = vec2(1.0 - vtexCoords.s, vtexCoords.t);
    color = texture(text, vtexCoords);
    if(color.a < 0.1)
        discard;
}
