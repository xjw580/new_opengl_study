#pragma once

#include <learnopengl/mesh.h>

static auto boxVertices = std::vector<GLfloat>{
    // positions          // normals           // texture coords
    // 前面 (逆时针)
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

    // 后面 (顺时针)
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

    // 左面 (逆时针)
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

    // 右面 (顺时针)
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // 底面 (逆时针)
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    // 顶面 (顺时针)
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

static auto boxIndices = std::vector<unsigned int>{
    // 前面
    0, 1, 2, 2, 3, 0,
    // 后面
    4, 6, 5, 6, 4, 7,
    // 左面
    8, 9, 10, 10, 11, 8,
    // 右面
    12, 14, 13, 14, 12, 15,
    // 底面
    16, 17, 18, 18, 19, 16,
    // 顶面
    20, 22, 21, 22, 20, 23
};;

class BoxMesh : public Mesh {
public:
    BoxMesh(): Mesh(boxVertices, boxIndices, getTexture()) {
    }

private:
    static std::vector<Texture> getTexture() {
        const std::string dir;
        auto boxDiffuseTexturePath = FileSystem::getPath("resources/texture/box_diffuse.png");

        return std::vector<Texture>{
            Texture{
                TextureFromFile(boxDiffuseTexturePath.c_str(), dir, false),
                std::string{"texture_diffuse"},
                aiString{boxDiffuseTexturePath}
            },
        };
    }
};
