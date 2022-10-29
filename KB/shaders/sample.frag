#version 410

#define MAX_LIGHTS 128

out vec4 FragColor;
uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}