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

#define SHARER_DIR "resources/shader/3d_brain_map_test"

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

glm::vec3 mapToColor(float v) {
    v = glm::clamp(v, 0.0f, 1.0f); // 保证 v 在 [0, 1] 之间

    // 颜色映射：从蓝色 -> 绿色 -> 红色
    glm::vec3 color;
    if (v < 0.5f) {
        // 蓝色 -> 绿色
        color = glm::mix(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), v * 2.0f);
    } else {
        // 绿色 -> 红色
        color = glm::mix(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (v - 0.5f) * 2.0f);
    }

    return color;
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
    std::vector<GLfloat> colors(brainLength * brainLength * brainLength * 4);
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
    int cI = 0;
    for (size_t i = 3; i < vertices.size(); i += 4) {
        vertices[i] = (vertices[i] - minValue) / (maxValue - minValue);
        auto color = mapToColor(vertices[i]);
        colors[cI++] = color.r;
        colors[cI++] = color.g;
        colors[cI++] = color.b;
        // colors[cI++] = 0.5;
        // colors[cI++] = 0.5;
        // colors[cI++] = 0.5;
        colors[cI++] = 0.5;
    }

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

    Shader volumeShader{
        FileSystem::getPath(std::format(R"({}/volume.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/volume.frag)", SHARER_DIR)).c_str()
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    float vertices1[] = {
        // 位置       // 纹理坐标
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    GLuint volumeTexture;
    glGenTextures(1, &volumeTexture);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // 上传纹理数据
    glTexImage3D(GL_TEXTURE_3D,
                 0,                 // 细节层次
                 GL_RGBA32F,        // 内部格式
                 brainLength,brainLength,brainLength,
                 0,                 // 边框
                 GL_RGBA,           // 数据格式
                 GL_FLOAT,          // 数据类型
                 colors.data());      // 数据指针

    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        lastFrame = static_cast<float>(glfwGetTime());

        processInput(window);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 渲染循环中
        volumeShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, volumeTexture);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        volumeShader.setMat4("projection", projection);
        volumeShader.setMat4("view", view);
        // 沿z轴绘制多个切片
        int numSlices = 100;
        for(int i = 0; i < numSlices; i++) {
            float z = -1.0f + 2.0f * float(i) / float(numSlices - 1);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, z));
            volumeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        // // 离屏渲染
        // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //
        // // 渲染脑模型
        // glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_STENCIL_TEST);
        // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
        // glStencilMask(0xFF);
        // glm::mat4 model;
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
        //                                         100.0f);
        // glm::mat4 view = camera.GetViewMatrix();
        // commonShader.use();
        // commonShader.setMat4("projection", projection);
        // commonShader.setMat4("view", view);
        // model = {1.0f};
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // commonShader.setMat4("model", model);
        // commonShader.setFloat("volumeScale", 1.0f / brainLength);
        // brainModel.Draw(commonShader);
        //
        // // 渲染值
        // glStencilFunc(GL_EQUAL, 1, 0xFF);
        // glStencilMask(0x00); // 禁止模板缓冲的写入
        // glDisable(GL_DEPTH_TEST);
        // brainShader.use();
        // brainShader.setMat4("projection", projection);
        // brainShader.setMat4("view", view);
        // model = {1.0f};
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // brainShader.setMat4("model", model);
        // brainShader.setFloat("volumeScale", 1.0f / brainLength);
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_POINTS, 0, values.size());
        // glStencilMask(0xff);
        // glDisable(GL_STENCIL_TEST);
        //
        // // 渲染到屏幕
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // // clear all relevant buffers
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        // glClear(GL_COLOR_BUFFER_BIT);
        // screenShader.use();
        // glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        // glBindVertexArray(quadVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);

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
