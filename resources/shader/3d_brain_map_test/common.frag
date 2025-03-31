#version 450 core
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

void main()
{
    // 读取基础颜色（漫反射）
        vec4 albedo = texture(texture_diffuse1, TexCoords);
    //
    //    // 计算法线
    //    vec3 normal = normalize(Normal);// 默认使用模型法线
    //    if (useNormalMap) {
    //        vec3 tangentNormal = texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0;// 从法线贴图获取法线
    //        normal = normalize(TBN * tangentNormal);// 转换到世界坐标
    //    }
    //
    //    // 计算光照（Blinn-Phong）
    //    vec3 lightDir = normalize(lightPos - FragPos);
    //    float diff = max(dot(normal, lightDir), 0.0);// 漫反射
    //
    //    vec3 viewDir = normalize(viewPos - FragPos);
    //    vec3 halfwayDir = normalize(lightDir + viewDir);
    //    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);// 高光
    //
    //    vec4 diffuse = vec4(diff * lightColor, 1.0) * albedo;
    //    vec4 specular = vec4(spec * lightColor, 1.0);

    //    FragColor = diffuse + specular;
    //    FragColor = albedo;
    FragColor = vec4(0, 0, 0, 0);
    //    discard;
}
