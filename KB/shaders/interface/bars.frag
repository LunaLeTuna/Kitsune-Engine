#version 410
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;

void main()
{
    vec2 vtexCoords = TexCoords;
    color = vec4(vtexCoords.y+vtexCoords.x,vtexCoords.y/2,0.0,1.0);
}
