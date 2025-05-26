#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <filesystem.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <iostream>
#include "common.h"
#include "data.h"
#include "blur.h"
#include "meshs/BoxMesh.h"
#include "meshs/WindowMesh.h"
#include "meshs/GrassMesh.h"
#include "meshs/FloorMesh.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef K::Point_3 Point;


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 2000;
const unsigned int SCR_HEIGHT = 1500;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// fram
float frame = 60;
float min_fgt = 1.0f / frame;


// 防止深度冲突
// 第一个也是最重要的技巧是永远不要把多个物体摆得太靠近，以至于它们的一些三角形会重叠。通过在两个物体之间设置一个用户无法注意到的偏移值，你可以完全避免这两个物体之间的深度冲突。在箱子和地板的例子中，我们可以将箱子沿着正y轴稍微移动一点。箱子位置的这点微小改变将不太可能被注意到，但它能够完全减少深度冲突的发生。然而，这需要对每个物体都手动调整，并且需要进行彻底的测试来保证场景中没有物体会产生深度冲突。
//
// 第二个技巧是尽可能将近平面设置远一些。在前面我们提到了精度在靠近近平面时是非常高的，所以如果我们将近平面远离观察者，我们将会对整个平截头体有着更大的精度。然而，将近平面设置太远将会导致近处的物体被裁剪掉，所以这通常需要实验和微调来决定最适合你的场景的近平面距离。
//
// 另外一个很好的技巧是牺牲一些性能，使用更高精度的深度缓冲。大部分深度缓冲的精度都是24位的，但现在大部分的显卡都支持32位的深度缓冲，这将会极大地提高精度。所以，牺牲掉一些性能，你就能获得更高精度的深度测试，减少深度冲突。

#define SHARER_DIR "resources/shader/3d_brain_map"

auto boxPos = std::vector<glm::vec3>{
    glm::vec3{0.0f, 0.0f, -1.0f},
    glm::vec3{2.0f, 0.0f, -1.0f},
};

// 先绘制远处的镜子，防止因深度测试导致远处镜子的片段不绘制
auto windowPos = std::vector<glm::vec3>{
    glm::vec3{1.0f, 0.0f, 0.0f},
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{2.0f, 0.0f, 1.0f},
};

auto grassPos = std::vector<glm::vec3>{
    glm::vec3{1.0f, 0.0f, -0.5},
    glm::vec3{0.0f, 0.0f, 1.5},
    glm::vec3{2.0f, 0.0f, -0.25},
    glm::vec3{1.0f, 0.0f, 2.5},
};

void smoothValues(std::vector<GLfloat> &vertices, int width, int height, int depth, int iterations = 5) {
    // Create a copy of the original v values
    std::vector<GLfloat> smoothedValues(width * height * depth);

    // Extract v values from vertices
    for (int i = 0; i < width * height * depth; i++) {
        smoothedValues[i] = vertices[i * 4 + 3]; // v component is at index 3
    }

    // Perform smoothing iterations
    for (int iter = 0; iter < iterations; iter++) {
        std::vector<GLfloat> newValues = smoothedValues;

        for (int z = 1; z < depth - 1; z++) {
            for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
                    int idx = x + y * width + z * width * height;

                    // Calculate average of neighbors (6-neighborhood)
                    float sum = 0.0f;
                    sum += smoothedValues[idx - 1]; // -x
                    sum += smoothedValues[idx + 1]; // +x
                    sum += smoothedValues[idx - width]; // -y
                    sum += smoothedValues[idx + width]; // +y
                    sum += smoothedValues[idx - width * height]; // -z
                    sum += smoothedValues[idx + width * height]; // +z

                    // Update value (lambda controls smoothing strength, 0.0-1.0)
                    float lambda = 0.5f;
                    newValues[idx] = (1.0f - lambda) * smoothedValues[idx] +
                                     (lambda / 6.0f) * sum;
                }
            }
        }

        smoothedValues = newValues;
    }

    // Write smoothed values back to the original vertices array
    for (int i = 0; i < width * height * depth; i++) {
        vertices[i * 4 + 3] = smoothedValues[i];
    }
}

// 三线性插值函数
float trilinearInterpolate(const std::vector<GLfloat> &vertices,
                           int width, int height, int depth,
                           float x, float y, float z) {
    int x0 = floor(x);
    int y0 = floor(y);
    int z0 = floor(z);
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;

    // 确保索引在边界内
    x0 = std::max(0, std::min(x0, width - 1));
    y0 = std::max(0, std::min(y0, height - 1));
    z0 = std::max(0, std::min(z0, depth - 1));
    x1 = std::max(0, std::min(x1, width - 1));
    y1 = std::max(0, std::min(y1, height - 1));
    z1 = std::max(0, std::min(z1, depth - 1));

    // 获取八个顶点的值
    float v000 = vertices[(x0 + y0 * width + z0 * width * height) * 4 + 3];
    float v001 = vertices[(x0 + y0 * width + z1 * width * height) * 4 + 3];
    float v010 = vertices[(x0 + y1 * width + z0 * width * height) * 4 + 3];
    float v011 = vertices[(x0 + y1 * width + z1 * width * height) * 4 + 3];
    float v100 = vertices[(x1 + y0 * width + z0 * width * height) * 4 + 3];
    float v101 = vertices[(x1 + y0 * width + z1 * width * height) * 4 + 3];
    float v110 = vertices[(x1 + y1 * width + z0 * width * height) * 4 + 3];
    float v111 = vertices[(x1 + y1 * width + z1 * width * height) * 4 + 3];

    // 计算插值权重
    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    // 三线性插值
    float c00 = v000 * (1 - xd) + v100 * xd;
    float c01 = v001 * (1 - xd) + v101 * xd;
    float c10 = v010 * (1 - xd) + v110 * xd;
    float c11 = v011 * (1 - xd) + v111 * xd;

    float c0 = c00 * (1 - yd) + c10 * yd;
    float c1 = c01 * (1 - yd) + c11 * yd;

    return c0 * (1 - zd) + c1 * zd;
}

// 应用于整个数据集
void applyTrilinearSmoothing(std::vector<GLfloat> &vertices, int width, int height, int depth) {
    std::vector<GLfloat> smoothed(vertices.size());

    // 拷贝所有非v值
    for (int i = 0; i < width * height * depth; i++) {
        smoothed[i * 4] = vertices[i * 4]; // x
        smoothed[i * 4 + 1] = vertices[i * 4 + 1]; // y
        smoothed[i * 4 + 2] = vertices[i * 4 + 2]; // z
    }

    // 对每个点应用三线性插值
    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // 计算周围的平均坐标 (添加小偏移以获取平滑效果)
                float sx = x + 0.5f;
                float sy = y + 0.5f;
                float sz = z + 0.5f;

                // 应用插值获取平滑值
                float smoothValue = trilinearInterpolate(vertices, width, height, depth, sx, sy, sz);

                // 存储平滑后的值
                smoothed[(x + y * width + z * width * height) * 4 + 3] = smoothValue;
            }
        }
    }

    // 更新原始数据
    vertices = smoothed;
}

// 组合方法
void combinedSmoothing(std::vector<GLfloat> &vertices, int width, int height, int depth) {
    // 首先应用三线性插值来处理分块现象
    applyTrilinearSmoothing(vertices, width, height, depth);

    // 然后应用一次轻度拉普拉斯平滑来进一步增强平滑效果
    smoothValues(vertices, width, height, depth, 2); // 只需要几次迭代
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader brainShader{
        FileSystem::getPath(std::format(R"({}/brain.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/brain.frag)", SHARER_DIR)).c_str()
    };

    Shader commonShader{
        FileSystem::getPath(std::format(R"({}/common.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/common.frag)", SHARER_DIR)).c_str()
    };

    Model brainModel{FileSystem::getPath(R"(resources/objects/brain/Brain_Model.obj)")};

    constexpr int brainLength = 100;
    std::vector<float> values;
    test(values, brainLength, brainLength, brainLength);
    float minValue = values[0];
    float maxValue = values[0];
    std::vector<GLfloat> vertices(brainLength * brainLength * brainLength * 4);
    int index = 0;
    float half = brainLength / 2.0f;
    for (int z = 0; z < brainLength; z++) {
        for (int y = 0; y < brainLength; y++) {
            for (int x = 0; x < brainLength; x++) {
                int i = x + y * brainLength + z * brainLength * brainLength;
                auto v = values[i];
                minValue = std::min(minValue, v);
                maxValue = std::max(maxValue, v);
                vertices[index++] = static_cast<float>(x) - half;
                vertices[index++] = static_cast<float>(y);
                vertices[index++] = static_cast<float>(z) - half;
                vertices[index++] = v;
            }
        }
    }
    // 标准化强度值
    for (size_t i = 3; i < vertices.size(); i += 4) {
        vertices[i] = (vertices[i] - minValue) / (maxValue - minValue);
    }

    blurVertices(vertices, brainLength, brainLength, brainLength, 5.0, 3);
    // Assuming vertices is your std::vector<GLfloat> with size 100*100*100*4
    // combinedSmoothing(vertices, 100, 100, 100);
    // 创建VAO和VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定VAO和VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // 设置顶点属性
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(0);

    // 强度属性
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 帧缓冲

    Shader screenShader{
        FileSystem::getPath(std::format(R"({}/screen.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/screen.frag)", SHARER_DIR)).c_str()
    };
    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // 创建一个简单的全屏四边形
    float quadVertices[] = {
        // 位置        // 纹理坐标
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        lastFrame = static_cast<float>(glfwGetTime());

        processInput(window);
        // 离屏渲染
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 渲染脑模型
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
        glStencilMask(0xFF);
        glm::mat4 model;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT), 0.0f, -3.0f, 3.0f);
        glm::mat4 view = camera.GetViewMatrix();
        commonShader.use();
        commonShader.setMat4("projection", projection);
        commonShader.setMat4("view", view);
        model = {1.0f};
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        float scale = 0.75;
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        commonShader.setMat4("model", model);
        commonShader.setFloat("volumeScale", 1.0f / brainLength);
        brainModel.Draw(commonShader);

        // 渲染值
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入
        // glDisable(GL_DEPTH_TEST);
        brainShader.use();
        brainShader.setMat4("projection", projection);
        brainShader.setMat4("view", view);
        model = {1.0f};
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        brainShader.setMat4("model", model);
        brainShader.setFloat("volumeScale", 1.0f / brainLength);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, values.size());
        glStencilMask(0xff);
        glDisable(GL_STENCIL_TEST);

        // 渲染到屏幕
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.use();
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
