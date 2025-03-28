#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aIntensity;

out float intensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float volumeScale;

void main() {
    // 使用volumeScale进行缩放
    vec3 scaledPos = aPos * volumeScale;
    gl_Position = projection * view * model * vec4(scaledPos, 1.0);
    gl_PointSize = gl_Position.z;
    intensity = aIntensity;
}