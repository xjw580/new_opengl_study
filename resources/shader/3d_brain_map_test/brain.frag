#version 450 core

out vec4 FragColor;
in float intensity;


void main() {
    // 根据强度创建颜色映射
    vec3 color;
    if (intensity < 0.33) {
        color = mix(vec3(0, 0, 1), vec3(0, 1, 1), intensity * 3.0);  // 蓝色到青色
    } else if (intensity < 0.66) {
        color = mix(vec3(0, 1, 1), vec3(0, 1, 0), (intensity - 0.33) * 3.0);  // 青色到绿色
    } else {
        color = mix(vec3(0, 1, 0), vec3(1, 0, 0), (intensity - 0.66) * 3.0);  // 绿色到红色
    }

    FragColor = vec4(color, 1); // 半透明
}
