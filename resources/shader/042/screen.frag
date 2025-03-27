#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    // 基本的纹理渲染
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // 灰度效果
    // float average = (color.r + color.g + color.b) / 3.0;
    // FragColor = vec4(average, average, average, 1.0);

    // 反相效果
     FragColor = vec4(1.0 - color, 1.0);

    // 模糊效果
//     const float offset = 1.0 / 300.0;
//     vec2 offsets[9] = vec2[](
//         vec2(-offset,  offset), // 左上
//         vec2( 0.0f,    offset), // 正上
//         vec2( offset,  offset), // 右上
//         vec2(-offset,  0.0f),   // 左
//         vec2( 0.0f,    0.0f),   // 中
//         vec2( offset,  0.0f),   // 右
//         vec2(-offset, -offset), // 左下
//         vec2( 0.0f,   -offset), // 正下
//         vec2( offset, -offset)  // 右下
//     );
//
//     float blurKernel[9] = float[](
//         1.0/9.0, 1.0/9.0, 1.0/9.0,
//         1.0/9.0, 1.0/9.0, 1.0/9.0,
//         1.0/9.0, 1.0/9.0, 1.0/9.0
//     );
//
//     vec3 sampleTex[9];
//     for(int i = 0; i < 9; i++) {
//         sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//     }
//     vec3 blurColor = vec3(0.0);
//     for(int i = 0; i < 9; i++)
//         blurColor += sampleTex[i] * blurKernel[i];
//     FragColor = vec4(blurColor, 1.0);

    // 锐化效果
    // const float offset = 1.0 / 300.0;
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset,  offset), // 左上
    //     vec2( 0.0f,    offset), // 正上
    //     vec2( offset,  offset), // 右上
    //     vec2(-offset,  0.0f),   // 左
    //     vec2( 0.0f,    0.0f),   // 中
    //     vec2( offset,  0.0f),   // 右
    //     vec2(-offset, -offset), // 左下
    //     vec2( 0.0f,   -offset), // 正下
    //     vec2( offset, -offset)  // 右下
    // );
    //
    // float sharpenKernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    //
    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++) {
    //     sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    // }
    // vec3 sharpenColor = vec3(0.0);
    // for(int i = 0; i < 9; i++)
    //     sharpenColor += sampleTex[i] * sharpenKernel[i];
    // FragColor = vec4(sharpenColor, 1.0);

    // 边缘检测效果
//    const float offset = 1.0 / 300.0;
//    vec2 offsets[9] = vec2[](
//    vec2(-offset,  offset), // 左上
//    vec2( 0.0f,    offset), // 正上
//    vec2( offset,  offset), // 右上
//    vec2(-offset,  0.0f),   // 左
//    vec2( 0.0f,    0.0f),   // 中
//    vec2( offset,  0.0f),   // 右
//    vec2(-offset, -offset), // 左下
//    vec2( 0.0f,   -offset), // 正下
//    vec2( offset, -offset)  // 右下
//    );
//
//    float kernel[9] = float[](
//    1,  1, 1,
//    1, -8, 1,
//    1,  1, 1
//    );
//
//    vec3 sampleTex[9];
//    for(int i = 0; i < 9; i++) {
//        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//    }
//    vec3 col = vec3(0.0);
//    for(int i = 0; i < 9; i++)
//    col += sampleTex[i] * kernel[i];
//
//    FragColor = vec4(col, 1.0);

    // 默认直接渲染
    // FragColor = vec4(color, 1.0);
}