#version 450 core
uniform sampler3D volumeTexture;
in vec3 texCoord;
out vec4 FragColor;

void main() {
    FragColor = texture(volumeTexture, texCoord);
}