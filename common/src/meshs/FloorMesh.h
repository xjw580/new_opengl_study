#pragma once

#include <learnopengl/mesh.h>


static std::vector<GLfloat> floorVertices = {
    // 位置                // 法线          // 纹理坐标
    5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,  // 0: 右上
   -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // 1: 左上
   -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,  // 2: 左下
    5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f   // 3: 右下
};
static std::vector<GLuint> floorIndices = {
    0, 1, 2,  // 第一个三角形：右上 -> 左上 -> 左下
    0, 2, 3   // 第二个三角形：右上 -> 左下 -> 右下
};

class FloorMesh : public Mesh {
public:
    FloorMesh(): Mesh(floorVertices, floorIndices, getTexture()) {
    }

private:
    static std::vector<Texture> getTexture() {
        auto diffuseTexturePath = FileSystem::getPath("resources/texture/metal.png");
        const std::string dir;

        return std::vector<Texture>{
            Texture{
                TextureFromFile(diffuseTexturePath.c_str(), dir, false),
                std::string{"texture_diffuse"},
                aiString{diffuseTexturePath}
            },
        };
    }
};
