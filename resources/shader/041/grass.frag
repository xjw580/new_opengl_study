#version 450 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture_diffuse1;

void main()
{
    // 读取基础颜色（漫反射）
    vec4 albedo = texture(texture_diffuse1, TexCoords);
    if(albedo.a < 0.1){
        discard;
    }
    FragColor = albedo;
}
