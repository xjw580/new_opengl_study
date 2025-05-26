#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D volumeTexture;

vec2 texelSize = vec2(1/100.0, 1/100.0);
int blurRadius = 10;
float pointSize = 1.0;// 点的大小
vec2 resolution = vec2(100, 100);// 屏幕分辨率

void main() {
    vec2 pixelCoords = TexCoords * resolution;
    vec4 centerColor = texture(screenTexture, TexCoords);
//
//    // 如果是空白区域，直接返回透明
//    if (centerColor.a < 0.01) {
//        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
//        return;
//    }
//
//    vec3 result = vec3(0.0);
//    float totalWeight = 0.0;
//
//    // 计算点的半径（像素）
//    float halfPointSize = pointSize / 2.0;
//
//    // 高斯模糊
//    for (int x = -blurRadius; x <= blurRadius; ++x) {
//        for (int y = -blurRadius; y <= blurRadius; ++y) {
//            vec2 offset = vec2(x, y) * texelSize;
//            vec2 sampleCoords = TexCoords + offset;
//            vec4 sampleColor = texture(screenTexture, sampleCoords);
//
//            // 只计算在点范围内的像素
//            vec2 samplePixelCoords = sampleCoords * resolution;
//            float distToCenter = distance(pixelCoords, samplePixelCoords);
//
//            if (distToCenter <= halfPointSize && sampleColor.a > 0.01) {
//                float weight = exp(-(x*x + y*y) / (2.0 * blurRadius * blurRadius));
//                result += sampleColor.rgb * weight;
//                totalWeight += weight;
//            }
//        }
//    }
//
//    // 防止除零
//    if (totalWeight > 0.0) {
//        result /= totalWeight;
//        FragColor = vec4(result, 1.0);
//    } else {
//        FragColor = centerColor;
//    }

    FragColor = centerColor;
}