#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <array>
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
#include <random>
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <thread>

namespace status {
    const char *glsl_version = "#version 450";
}

// settings
const unsigned int SCR_WIDTH = 2000;
const unsigned int SCR_HEIGHT = 1500;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// fram
float frame = 60;
float minFgt = 1.0f / frame;

struct Obj {
    glm::vec3 position;
    float angle;
    glm::vec3 scale;
    glm::vec3 axis;
};

static constexpr int kPetalAmount = 2000;
static std::vector<Obj> petalObj;
static std::array<glm::mat4, kPetalAmount> petalPos;

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

void MouseCallback(GLFWwindow *window, double xpos, double ypos);

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

void ProcessInput(GLFWwindow *window);

void InitPetalPositions();

void RotatePetal(float time = glfwGetTime());

float GetGPUUsage();


static void GlfwErrorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static int ShowWindow() {
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    Shader ourShader{
        FileSystem::getPath(R"(resources/shader/love/petal.vert)").c_str(),
        FileSystem::getPath(R"(resources/shader/love/petal.frag)").c_str()
    };

    Model petalModel{FileSystem::getPath(R"(resources/objects/petal/new_petal1.obj)")};

    InitPetalPositions();

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, petalPos.size() * sizeof(glm::mat4), petalPos.data(), GL_DYNAMIC_DRAW);

    for (const auto &mesh: petalModel.meshes) {
        const unsigned int VAO = mesh.VAO;
        glBindVertexArray(VAO);
        // 顶点属性
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(status::glsl_version);

    io.Fonts->AddFontFromFileTTF(R"(C:/Windows/Fonts/msyh.ttc)", 22.0f, nullptr,
                                 io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != nullptr);

    constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    camera.ProcessMouseMovement(0, -200);

    float gpu_usage = 0.0f;
    std::thread t([&gpu_usage, &window] {
        while (!glfwWindowShouldClose(window)) {
            gpu_usage = GetGPUUsage();
        }
    });
    t.detach();

    // 主要修改1：在渲染循环开始时清除深度缓冲
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = (currentFrame - lastFrame) * 2;
        lastFrame = static_cast<float>(glfwGetTime());

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui界面代码...
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");
            ImGui::Text((std::string("GPU Usage: ") + std::to_string(gpu_usage)).data());
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float *) &clear_color);

            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            ImGuiViewport *viewport = ImGui::GetWindowViewport();
            if (viewport && viewport->PlatformHandle) {
                GLFWwindow *debug_imgui_window = static_cast<GLFWwindow *>(viewport->PlatformHandle);
                glfwSetWindowAttrib(debug_imgui_window, GLFW_FLOATING, GLFW_TRUE);
            }
            ImGui::End();
        }

        // 设置清除颜色并清除颜色和深度缓冲区
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 关键修改：添加深度缓冲清除

        // 3D渲染部分
        {
            ProcessInput(window);

            ourShader.use();
            ourShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
            ourShader.setVec3("viewPos", camera.Position);
            ourShader.setVec3("lightColor", glm::vec3(1.0f, 0.75f, 0.79f));

            // 视图/投影变换
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                    (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                    0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            RotatePetal();
            glBufferData(GL_ARRAY_BUFFER, petalPos.size() * sizeof(glm::mat4), petalPos.data(), GL_DYNAMIC_DRAW);

            // 渲染花瓣模型
            ourShader.setInt("texture_diffuse1", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, petalModel.textures_loaded[0].id);
            for (const auto &mesh: petalModel.meshes) {
                glBindVertexArray(mesh.VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()),
                                        GL_UNSIGNED_INT, 0, petalPos.size());
                glBindVertexArray(0);
            }
        }

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 处理多视口
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InitPetalPositions() {
    std::random_device rd;
    std::mt19937 gen(rd());
    constexpr float radius_limit = 5.0f;
    std::uniform_real_distribution<float> uniform(0.0f, 1.0f);
    std::uniform_real_distribution<float> angle_distrib(0.0f, 360.0f);

    petalObj.resize(kPetalAmount);

    for (int i = 0; auto &[position, angle, scale, axis]: petalObj) {
        // 圆盘采样
        float u = uniform(gen);
        float v = uniform(gen);
        float r = radius_limit * std::sqrt(u);
        float theta = glm::two_pi<float>() * v;

        float x = r * std::cos(theta);
        float z = r * std::sin(theta);
        position = glm::vec3(x, -1.0f, z);

        // 随机旋转
        angle = glm::radians(angle_distrib(gen));
        axis = glm::normalize(glm::vec3(uniform(gen), uniform(gen), uniform(gen)));

        scale = glm::vec3(0.1f);

        glm::mat4 model = glm::mat4{1.0f};
        model = glm::translate(model, position);
        model = glm::rotate(model, angle, axis);
        model = glm::scale(model, scale);

        petalPos[i] = model;
        ++i;
    }
}

void RotatePetal(const float time) {
    float baseAngle = glm::radians(0.1f); // 每帧基础旋转角速度
    float frequency = 1.0f; // 浮动频率
    float amplitude = 0.1f; // 浮动振幅

    for (int i = 0; auto &[position, angle, scale, axis]: petalObj) {
        // 让每个花瓣有不同的旋转速率和起始角度，增加“生命感”
        float deltaAngle = baseAngle * (0.8f + 0.4f * std::sin(time + i));
        float radius = glm::length(glm::vec2(position.x, position.z));
        float theta = std::atan2(position.z, position.x);

        // 角度偏移旋转
        theta += deltaAngle;

        // 更新位置（仍保持圆心旋转，但带有一点非均匀性）
        position.x = radius * std::cos(theta);
        position.z = radius * std::sin(theta);

        // 上下浮动，模拟风感
        position.y = -1.0f + std::sin(time * frequency + i) * amplitude;

        // 可选：让花瓣自身也慢慢旋转（增加变化感）
        float selfSpin = glm::radians(1.0f) * std::sin(time + i);
        float totalAngle = angle + selfSpin;
        angle = totalAngle;

        // 重建模型矩阵
        glm::mat4 model{1.0f};
        model = glm::translate(model, position);
        model = glm::rotate(model, totalAngle, axis);
        model = glm::scale(model, scale);
        petalPos[i] = model;
        ++i;
    }
}

float GetGPUUsage() {
    PDH_HQUERY hQuery;
    PDH_HCOUNTER hCounter;
    PDH_FMT_COUNTERVALUE counterVal;

    // 创建 PDH 查询
    if (PdhOpenQuery(NULL, 0, &hQuery) != ERROR_SUCCESS) {
        std::cerr << "Failed to open PDH query." << std::endl;
        return 1;
    }

    // 添加计数器：这个路径在任务管理器里可以查到（可用 perfmon 验证）
    const char* counterPath = R"(\GPU Engine(*)\Utilization Percentage)";

    if (PdhAddEnglishCounter(hQuery, counterPath, 0, &hCounter) != ERROR_SUCCESS) {
        std::cerr << "Failed to add PDH counter." << std::endl;
        PdhCloseQuery(hQuery);
        return 1;
    }

    // 初次收集数据
    PdhCollectQueryData(hQuery);
    Sleep(1000);  // 等一秒再采样

    // 第二次收集数据后才能计算百分比
    if (PdhCollectQueryData(hQuery) == ERROR_SUCCESS) {
        if (PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS) {
            // std::cout << "GPU Usage: " << counterVal.doubleValue << " %" << std::endl;
            return counterVal.doubleValue;
        } else {
            std::cerr << "Failed to format counter value." << std::endl;
        }
    } else {
        std::cerr << "Failed to collect query data." << std::endl;
    }

    PdhCloseQuery(hQuery);

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

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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


// Main code
int main(int, char **) {
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    std::cout << "show_imgui res: " << ShowWindow() << std::endl;

    return 0;
}
