#pragma once

#include <learnopengl/mesh.h>

static float width = 1.0f; // 窗户宽度
static float height = 1.0f; // 窗户高度
static float z = 0.0f; // 窗户深度

static std::vector<GLfloat> windowVertices = {
    // 位置            // 纹理坐标
    // 位置                // 法线          // 纹理坐标
    -width / 2, -height / 2, z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下角 0
    width / 2, -height / 2, z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 右下角 1
    width / 2, height / 2, z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 右上角 2
    -width / 2, height / 2, z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f // 左上角 3
};
static std::vector<unsigned int> windowIndices = {
    0, 1, 2, // 第一个三角形
    2, 3, 0 // 第二个三角形
};

class WindowMesh : public Mesh {
public:
    WindowMesh(): Mesh(windowVertices, windowIndices, getTexture()) {
    }

private:
    static std::vector<Texture> getTexture() {
        auto windowDiffuseTexturePath = FileSystem::getPath("resources/texture/window.png");
        const std::string dir;

        return std::vector<Texture>{
            Texture{
                TextureFromFile(windowDiffuseTexturePath.c_str(), dir, false),
                std::string{"texture_diffuse"},
                aiString{windowDiffuseTexturePath}
            },
        };
    }
};
