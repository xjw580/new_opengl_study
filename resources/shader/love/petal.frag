#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;// 法线贴图用的切线空间矩阵

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;// 法线贴图
uniform bool useNormalMap;// 是否启用法线贴图

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objColor;

void main()
{
    // 读取基础颜色（漫反射）
//    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;
    vec3 albedo = objColor;

    // 计算法线
    vec3 normal = normalize(Normal);// 默认使用模型法线
    if (useNormalMap) {
        vec3 tangentNormal = texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0;// 从法线贴图获取法线
        normal = normalize(TBN * tangentNormal);// 转换到世界坐标
    }

    // 计算光照（Blinn-Phong）
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);// 漫反射

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);// 高光

    vec3 diffuse = diff * lightColor * albedo;
    vec3 specular = spec * lightColor;

        FragColor = vec4(diffuse + specular, 1.0);
//        FragColor = vec4(albedo, 1.0);
    // 颜色基于坐标
//    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
