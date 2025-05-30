#pragma once

#include <learnopengl/mesh.h>

#include "filesystem.h"

static auto landscapeSkyVertices = std::vector<GLfloat>{
    // positions          // normals           // texture coords
    // 前面 (逆时针)
    -100.0f, -100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    100.0f, -100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

    // 后面 (顺时针)
    -100.0f, -100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    100.0f, -100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    100.0f, 100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -100.0f, 100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

    // 左面 (逆时针)
    -100.0f, -100.0f, -100.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -100.0f, -100.0f, 100.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -100.0f, 100.0f, 100.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -100.0f, 100.0f, -100.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

    // 右面 (顺时针)
    100.0f, -100.0f, -100.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    100.0f, -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    100.0f, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    100.0f, 100.0f, -100.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // 底面 (逆时针)
    -100.0f, -100.0f, -100.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    100.0f, -100.0f, -100.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    100.0f, -100.0f, 100.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -100.0f, -100.0f, 100.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    // 顶面 (顺时针)
    -100.0f, 100.0f, -100.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    100.0f, 100.0f, -100.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    100.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -100.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

static auto landscapeSkyIndices = std::vector<unsigned int>{
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

class LandscapeSkyMesh : public SkyMesh {
public:
    LandscapeSkyMesh(): SkyMesh(landscapeSkyVertices, landscapeSkyIndices, getFadesPath()) {
    }

private:
    static std::vector<std::string> getFadesPath() {
        return std::vector{
            FileSystem::getPath("resources/texture/skybox/right.jpg"),
            FileSystem::getPath("resources/texture/skybox/left.jpg"),
            FileSystem::getPath("resources/texture/skybox/top.jpg"),
            FileSystem::getPath("resources/texture/skybox/bottom.jpg"),
            FileSystem::getPath("resources/texture/skybox/front.jpg"),
            FileSystem::getPath("resources/texture/skybox/back.jpg")
        };
    }
};
