#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <filesystem.h>

#include <iostream>
#include "../../libs/include/meshs/BoxMesh.h"
#include "meshs/WindowMesh.h"
#include "../../libs/include/meshs/GrassMesh.h"


void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

void MouseCallback(GLFWwindow *window, double xpos, double ypos);

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

void ProcessInput(GLFWwindow *window);

// settings
const unsigned int kScrWidth = 2000;
const unsigned int kScrHeight = 1500;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = kScrWidth / 2.0f;
float lastY = kScrHeight / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// fram
float frame = 60;
float minFgt = 1.0f / frame;


// 防止深度冲突
// 第一个也是最重要的技巧是永远不要把多个物体摆得太靠近，以至于它们的一些三角形会重叠。通过在两个物体之间设置一个用户无法注意到的偏移值，你可以完全避免这两个物体之间的深度冲突。在箱子和地板的例子中，我们可以将箱子沿着正y轴稍微移动一点。箱子位置的这点微小改变将不太可能被注意到，但它能够完全减少深度冲突的发生。然而，这需要对每个物体都手动调整，并且需要进行彻底的测试来保证场景中没有物体会产生深度冲突。
//
// 第二个技巧是尽可能将近平面设置远一些。在前面我们提到了精度在靠近近平面时是非常高的，所以如果我们将近平面远离观察者，我们将会对整个平截头体有着更大的精度。然而，将近平面设置太远将会导致近处的物体被裁剪掉，所以这通常需要实验和微调来决定最适合你的场景的近平面距离。
//
// 另外一个很好的技巧是牺牲一些性能，使用更高精度的深度缓冲。大部分深度缓冲的精度都是24位的，但现在大部分的显卡都支持32位的深度缓冲，这将会极大地提高精度。所以，牺牲掉一些性能，你就能获得更高精度的深度测试，减少深度冲突。

#define SHARER_DIR "resources/shader/041"

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(kScrWidth, kScrHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader commonShader{
        FileSystem::getPath(std::format(R"({}/common.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/common.frag)", SHARER_DIR)).c_str()
    };

    Shader grassShader{
        FileSystem::getPath(std::format(R"({}/common.vert)", SHARER_DIR)).c_str(),
        FileSystem::getPath(std::format(R"({}/grass.frag)", SHARER_DIR)).c_str()
    };


    BoxMesh boxMesh;
    WindowMesh windowMesh;
    GrassMesh grassMesh;

    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        lastFrame = static_cast<float>(glfwGetTime());

        ProcessInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_CULL_FACE);

        commonShader.use();
        commonShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f)); // 光源位置
        commonShader.setVec3("viewPos", camera.Position); // 相机位置
        commonShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // 光源颜色（白色）

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) kScrWidth / (float) kScrHeight, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        commonShader.setMat4("projection", projection);
        commonShader.setMat4("view", view);

        glm::mat4 model{};
        for (const auto &box_po: boxPos) {
            model = {1.0f};
            model = glm::translate(model, box_po);
            commonShader.setMat4("model", model);
            boxMesh.Draw(commonShader);
        }

        glDisable(GL_CULL_FACE);

        grassShader.use();
        grassShader.setMat4("projection", projection);
        grassShader.setMat4("view", view);

        for (const auto & grass_po : grassPos) {
            model = {1.0f};
            model = glm::translate(model, grass_po );
            grassShader.setMat4("model", model);
            grassMesh.Draw(grassShader);
            model = glm::rotate(model, glm::radians(90.0f),  glm::vec3(0, 1, 0));
            grassShader.setMat4("model", model);
            grassMesh.Draw(grassShader);
        }

        commonShader.use();

        for (const auto &window_po: windowPos) {
            model = {1.0f};
            model = glm::translate(model, window_po);
            commonShader.setMat4("model", model);
            windowMesh.Draw(commonShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow *window) {
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
void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void MouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
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
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
