#pragma once

#include <learnopengl/mesh.h>

static float grassWidth = 0.5f; // 窗户宽度
static float grassHeight = 0.5f; // 窗户高度
static float grassZ = 0.0f; // 窗户深度

static std::vector<GLfloat> grassVertices = {
    // 位置            // 纹理坐标
    // 位置                // 法线          // 纹理坐标
    -grassWidth / 2, -grassHeight / 2, grassZ, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下角 0
    grassWidth / 2, -grassHeight / 2, grassZ, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 右下角 1
    grassWidth / 2, grassHeight / 2, grassZ, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 右上角 2
    -grassWidth / 2, grassHeight / 2, grassZ, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f // 左上角 3
};
static std::vector<unsigned int> grassIndices = {
    0, 1, 2, // 第一个三角形
    2, 3, 0 // 第二个三角形
};

class GrassMesh : public Mesh {
public:
    GrassMesh(): Mesh(grassVertices, grassIndices, getTexture()) {
    }

private:
    static std::vector<Texture> getTexture() {
        auto diffuseTexturePath = FileSystem::getPath("resources/texture/grass.png");
        const std::string dir;

        return std::vector<Texture>{
            Texture{
                TextureFromFile(diffuseTexturePath.c_str(), dir, false, GL_CLAMP_TO_EDGE),
                std::string{"texture_diffuse"},
                aiString{diffuseTexturePath}
            },
        };
    }
};
