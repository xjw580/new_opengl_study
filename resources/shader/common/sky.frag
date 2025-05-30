#version 450 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube texture_cubemap1;

void main()
{
    FragColor = texture(texture_cubemap1, TexCoords);
}