#define GL_SILENCE_DEPRECATION
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
#include <thread>
#include "main.h"

static void GlfwErrorCallback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static int ShowWindow() {
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    // Create window with graphics context
    window = glfwCreateWindow(kScrWidth, kScrHeight, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
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


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    // 不要自动合并窗口
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const auto font = io.Fonts->AddFontFromFileTTF(R"(C:/Windows/Fonts/simsun.ttc)", 22.0f, nullptr,
                                             io.Fonts->GetGlyphRangesDefault());
    IM_ASSERT(font != nullptr);
    io.Fonts->Build();


    constexpr ImVec4 bg_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    constexpr ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    // constexpr ImVec4 petal_color = ImVec4(1.0f, 0.75f, 0.79f, 1.00f);
    constexpr ImVec4 petal_color = ImVec4(1.0f, 0.05f, 0.1f, 1.00f);

    ourShader = new Shader{
        FileSystem::getPath(R"(resources/shader/love/petal.vert)").c_str(),
        FileSystem::getPath(R"(resources/shader/love/petal.frag)").c_str()
    };

    std::thread petalThread(UpdateStatus, window);
    petalThread.detach();
    petalModel = new Model{FileSystem::getPath(R"(resources/objects/petal/new_petal1.obj)")};
    InitPetalObjs();

    glGenBuffers(1, &petalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, petalBuffer);

    for (const auto &mesh: petalModel->meshes) {
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
    camera.ProcessMouseMovement(0, 30);

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
            ImGui::SetNextWindowSize(ImVec2(600, 300));
            ImGui::Begin("Console");

            ImGui::ColorEdit3("Background color", (float *) &bg_color);
            ImGui::ColorEdit3("Light color", (float *) &light_color);
            ImGui::ColorEdit3("Petal color", (float *) &petal_color);
            ImGui::SliderInt("Petal amount", &petalAmount, 1, 10000);
            ImGui::Text("Current phase: %s", phase);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            ImGuiViewport *viewport = ImGui::GetWindowViewport();
            if (viewport && viewport->PlatformHandle) {
                GLFWwindow *debug_imgui_window = static_cast<GLFWwindow *>(viewport->PlatformHandle);
                glfwSetWindowAttrib(debug_imgui_window, GLFW_FLOATING, GLFW_TRUE);
            }

            ImGui::End();
        }

        // 设置清除颜色并清除颜色和深度缓冲区
        glClearColor(bg_color.x * bg_color.w, bg_color.y * bg_color.w, bg_color.z * bg_color.w,
                     bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D渲染部分
        {
            ProcessInput(window);

            ourShader->use();
            ourShader->setVec3("lightPos", glm::vec3(0.0f, 0.0f, 50.0f));
            ourShader->setVec3("viewPos", camera.Position);
            ourShader->setVec3("objColor", *(glm::vec3 *) &petal_color);
            ourShader->setVec3("lightColor", *(glm::vec3 *) &light_color);

            // 视图/投影变换
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                    windowAspect,
                                                    0.1f, 100.0f);
            ourShader->setMat4("projection", projection);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader->setMat4("view", view);

            glBindBuffer(GL_ARRAY_BUFFER, petalBuffer);
            glBufferData(GL_ARRAY_BUFFER, std::min(petalAmount, prevPetalAmount) * sizeof(glm::mat4), petalPos.data(),
                         GL_DYNAMIC_DRAW);

            // 渲染花瓣模型
            ourShader->setInt("texture_diffuse1", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, petalModel->textures_loaded[0].id);
            for (const auto &mesh: petalModel->meshes) {
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

static void InitPetalObjs() {
    petalObjs.resize(petalAmount);
    petalPos.resize(petalAmount);
    // roseModel = new Model{FileSystem::getPath(R"(resources/objects/rose/rose1.obj)")};
    roseModel = new Model{FileSystem::getPath(R"(resources/objects/rose/rose_advanced.obj)")};
    // roseModel = new Model{FileSystem::getPath(R"(resources/objects/rose/heart1.obj)")};
    auto vertices = roseModel->meshes[0].vertices;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution uniform(0.0f, static_cast<float>(vertices.size()));
    const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3{0.5f});
    constexpr glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3{0.0f, -2.0f, 0.0f});
    for (int i = 0; auto &petalObj: petalObjs) {
        petalPos[i] = InitPetal(petalObj);

        // 获取原始顶点位置
        glm::vec4 originalVertexPos = glm::vec4(vertices[uniform(gen)].Position, 1.0f); // 确保是齐次坐标

        // 应用缩放变换
        // 将缩放后的顶点位置设置为目标位置
        petalObj.target_position = glm::vec3(translateMatrix * scaleMatrix * originalVertexPos); // 转换为vec3
        ++i;
    }
}

static void UpdatePetalAmount() {
    if (petalAmount - prevPetalAmount > 20) {
        petalPos.reserve(petalAmount);
        petalObjs.reserve(petalAmount);
    }
    while (true) {
        if (prevPetalAmount < petalAmount) {
            Petal petal{};
            petalPos.push_back(InitPetal(petal));
            petalObjs.push_back(petal);
            ++prevPetalAmount;
        } else if (prevPetalAmount > petalAmount) {
            --prevPetalAmount;
            petalObjs.pop_back();
            petalPos.pop_back();
        } else break;
    }
}

static void RotatePetal(const float time) {
    float baseAngle = glm::radians(0.1f); // 每帧基础旋转角速度
    float frequency = 1.0f; // 浮动频率
    float amplitude = 0.1f; // 浮动振幅

    for (int i = 0; auto &[curr_position, src_position, target_position, angle, scale, axis]: petalObjs) {
        // 让每个花瓣有不同的旋转速率和起始角度，增加“生命感”
        float deltaAngle = baseAngle * (0.8f + 0.4f * std::sin(time + i));
        float radius = glm::length(glm::vec2(curr_position.x, curr_position.z));
        float theta = std::atan2(curr_position.z, curr_position.x);

        // 角度偏移旋转
        theta += deltaAngle;

        // 更新位置（仍保持圆心旋转，但带有一点非均匀性）
        curr_position.x = radius * std::cos(theta);
        curr_position.z = radius * std::sin(theta);

        // 上下浮动，模拟风感
        curr_position.y = -1.0f + std::sin(time * frequency + i) * amplitude;

        // 可选：让花瓣自身也慢慢旋转（增加变化感）
        float selfSpin = glm::radians(1.0f) * std::sin(time + i);
        float totalAngle = angle + selfSpin;
        angle = totalAngle;

        // 重建模型矩阵
        glm::mat4 model{1.0f};
        model = glm::translate(model, curr_position);
        model = glm::rotate(model, totalAngle, axis);
        model = glm::scale(model, scale);
        petalPos[i] = model;
        ++i;
    }
}

void MovePetal(const float time) {
    static float windStrength = 1.0f;
    static glm::vec2 windDirection = glm::normalize(glm::vec2(1.0f, 0.3f));

    // === 循环时间控制 ===
    static float scatterTime = 3.0f; // 自由飞舞时间
    static float assemblyTime = 3.0f; // 组装时间
    static float finalAssemblyTime = 0.5f; // 最终定位时间
    static float holdTime = 1.0f; // 保持玫瑰形状时间
    static float disassemblyTime = 5.0f; // 解散时间
    static float returnTime = 4.0f; // 返回原点时间

    float totalCycleTime = scatterTime + assemblyTime + finalAssemblyTime + holdTime + disassemblyTime + returnTime;
    float cycleTime = fmod(time, totalCycleTime); // 当前循环内的时间

    // 计算当前阶段和进度
    enum Phase { SCATTER, ASSEMBLY, FINAL_ASSEMBLY, HOLD, DISASSEMBLY, RETURN };
    Phase currentPhase;
    float phaseProgress = 0.0f; //[0,1]
    static float prevPhaseProgress = 0.0f;

    if (cycleTime < scatterTime) {
        currentPhase = SCATTER;
        std::strcpy(phase, "SCATTER");
        phaseProgress = cycleTime / scatterTime;
    } else if (int tempTime = 0; cycleTime < (tempTime = scatterTime + assemblyTime)) {
        currentPhase = ASSEMBLY;
        std::strcpy(phase, "ASSEMBLY");
        phaseProgress = (cycleTime - scatterTime) / assemblyTime;
    } else if (cycleTime < (tempTime = tempTime + finalAssemblyTime)) {
        currentPhase = FINAL_ASSEMBLY;
        std::strcpy(phase, "FINAL_ASSEMBLY");
        phaseProgress = (cycleTime - scatterTime - assemblyTime) / finalAssemblyTime;
    } else if (cycleTime < (tempTime = tempTime + holdTime)) {
        currentPhase = HOLD;
        std::strcpy(phase, "HOLD");
        phaseProgress = (cycleTime - scatterTime - assemblyTime - finalAssemblyTime) / holdTime;
    } else if (cycleTime < tempTime + disassemblyTime) {
        currentPhase = DISASSEMBLY;
        std::strcpy(phase, "DISASSEMBLY");
        phaseProgress = (cycleTime - scatterTime - assemblyTime - finalAssemblyTime - holdTime) / disassemblyTime;
    } else {
        currentPhase = RETURN;
        std::strcpy(phase, "RETURN");
        phaseProgress = (cycleTime - scatterTime - assemblyTime - finalAssemblyTime - holdTime - disassemblyTime) /
                        returnTime;
    }
    prevPhaseProgress = phaseProgress;

    // Perlin插值，输入[0,1]，输出[0,1]
    static auto smoothStep = [](const float t) -> float {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    };

    // 动态风力效果
    float windVariation = 0.7f + 0.3f * std::sin(time * 0.5f);
    glm::vec2 currentWind = windDirection * windStrength * windVariation;

    for (int i = 0; auto &[curr_position, src_position, target_position, angle, scale, axis]: petalObjs) {
        float petalTime = time + i * 0.3f;

        // === 每个花瓣的个性化延迟 ===
        float petalDelay = (i % 3) * 0.4f + std::sin(i * 0.5f) * 0.3f;

        // 根据当前阶段计算个体进度[0,1]
        float individualProgress = 0.0f;
        if (currentPhase == ASSEMBLY) {
            individualProgress = glm::clamp((phaseProgress * assemblyTime - petalDelay) / assemblyTime, 0.0f, 1.0f);
        } else if (currentPhase == FINAL_ASSEMBLY) {
            individualProgress = 1.0f;
        } else if (currentPhase == HOLD) {
            individualProgress = 1.0f;
        } else if (currentPhase == DISASSEMBLY) {
            individualProgress = glm::clamp(1.0f - (phaseProgress * disassemblyTime - petalDelay) / disassemblyTime,
                                            0.0f, 1.0f);
        } else if (currentPhase == RETURN) {
            individualProgress = 0.0f;
        }

        float individualSmooth = smoothStep(individualProgress);

        // === 基础自然运动计算 ===
        float currentRadius = glm::length(glm::vec2(curr_position.x, curr_position.z));
        float radian = std::atan2(curr_position.z, curr_position.x);

        // 根据阶段调整运动强度
        float movementIntensity = 1.0f;
        if (currentPhase == HOLD) {
            movementIntensity = 1.0f;
        } else if (currentPhase == ASSEMBLY || currentPhase == FINAL_ASSEMBLY) {
            movementIntensity = 1.0f - individualSmooth * 0.8f;
        } else if (currentPhase == DISASSEMBLY) {
            movementIntensity = 0.2f + (1.0f - individualProgress) * 0.8f; // 逐渐恢复运动
        }

        // 旋转和扩散
        float baseRotSpeed = (0.8f + 0.4f * std::sin(petalTime * 0.7f + i)) * movementIntensity;
        float rotationSpeed = glm::radians(baseRotSpeed);

        float spiralExpansion = 0.002f * (1.0f + 0.5f * std::sin(petalTime + i)) * movementIntensity;
        currentRadius += spiralExpansion;
        radian += rotationSpeed;

        // 风力影响
        float windDamping = (currentPhase == HOLD) ? 0.05f : (1.0f - individualSmooth * 0.9f);
        glm::vec2 windOffset = currentWind * 0.02f * std::sin(petalTime + i) * windDamping;

        // 基础自然位置
        glm::vec3 naturalPos;
        naturalPos.x = currentRadius * std::cos(radian) + windOffset.x;
        naturalPos.z = currentRadius * std::sin(radian) + windOffset.y;

        // === 垂直运动控制 ===
        static std::vector verticalVelocity(petalObjs.size(), 0.0f);
        if (verticalVelocity.size() != petalObjs.size()) {
            verticalVelocity.resize(petalObjs.size(), 0.0f);
        }

        if (currentPhase == SCATTER || (currentPhase == DISASSEMBLY && individualProgress > 0.5f) || currentPhase ==
            RETURN) {
            // 自由飞舞阶段的垂直运动
            float liftForce = 0.015f * std::exp(-cycleTime * 0.1f) * (1.0f + 0.3f * std::sin(petalTime + i));
            float gravity = -0.005f;
            float airResistance = -verticalVelocity[i] * 0.15f;
            float windLift = currentWind.x * 0.01f * std::sin(petalTime * 2.0f + i);

            float verticalForce = liftForce + gravity + airResistance + windLift;
            verticalVelocity[i] += verticalForce;
        } else {
            // 组装/保持阶段：垂直运动阻尼
            float dampingFactor;
            if (currentPhase == HOLD) {
                dampingFactor = 0.4f;
            } else if (currentPhase == DISASSEMBLY) {
                dampingFactor = 1.0f;
            } else {
                dampingFactor = 0.25f;
            }
            verticalVelocity[i] *= 1.0f - dampingFactor;
        }

        naturalPos.y = curr_position.y + verticalVelocity[i];

        // 地面碰撞
        if (naturalPos.y < -2.0f && (currentPhase == SCATTER || currentPhase == RETURN)) {
            naturalPos.y = -2.0f;
            verticalVelocity[i] *= -0.3f;
        }

        // === 根据阶段计算目标位置 ===

        if (currentPhase == SCATTER) {
            // 自由飞舞阶段 - 保持自然运动
            curr_position = naturalPos;
        } else if (currentPhase == ASSEMBLY || currentPhase == FINAL_ASSEMBLY) {
            // 组装阶段 - 向target_position移动
            glm::vec3 toTarget = target_position - naturalPos;
            float distanceToTarget = glm::length(toTarget);

            // 螺旋接近路径
            auto approachPath = glm::vec3(0.0f);
            if (individualProgress > 0.0f && individualProgress < 0.9f) {
                float spiralRadius = distanceToTarget * (1.0f - individualProgress) * 0.8f;
                float spiralAngle = petalTime * 4.5f + i * 1.2f;

                approachPath.x = spiralRadius * std::cos(spiralAngle);
                approachPath.z = spiralRadius * std::sin(spiralAngle);
                approachPath.y = std::sin(petalTime * 0.3f + i) * 0.05f * (1.0f - individualProgress);
            }

            glm::vec3 spiralTarget = target_position + approachPath;

            if (currentPhase == FINAL_ASSEMBLY) {
                float finalWeight = smoothStep(phaseProgress);
                curr_position = glm::mix(spiralTarget, target_position, finalWeight);
            } else {
                curr_position = glm::mix(naturalPos, spiralTarget, individualSmooth);
            }
        } else if (currentPhase == HOLD) {
        } else if (currentPhase == DISASSEMBLY) {
            // 解散阶段 - 从target_position回到自然运动
            if (float proportion = 0.5f; phaseProgress < proportion) {
                static constexpr float scaleFactor = 1.002f;
                glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3{
                                                       1.0f + (scaleFactor - 1.0f) * individualSmooth * (
                                                           proportion - phaseProgress) / proportion
                                                   });
                curr_position = glm::vec3(scaleMatrix * glm::vec4(naturalPos, 1.0f));
            } else {
                glm::vec3 scatterPath = glm::vec3(0.0f);
                // 逐渐增加螺旋离散效果
                float spiralOut = (phaseProgress - proportion) / (1 - proportion);
                float outwardRadius = spiralOut * 1.5f;
                float outwardAngle = petalTime * 3.0f + i * 1.5f;

                scatterPath.x += outwardRadius * std::cos(outwardAngle);
                scatterPath.z += outwardRadius * std::sin(outwardAngle);
                scatterPath.y += std::sin(petalTime * 0.5f + i) * 0.3f * spiralOut;

                curr_position = naturalPos + scatterPath;
            }
        } else if (currentPhase == RETURN) {
            // 返回阶段 - 回到src_position
            glm::vec3 toSrc = src_position - naturalPos;
            float distanceToSrc = glm::length(toSrc);

            // 类似组装的螺旋回归
            glm::vec3 returnPath = glm::vec3(0.0f);
            if (phaseProgress > 0.0f && phaseProgress < 0.9f) {
                float returnRadius = distanceToSrc * (1.0f - phaseProgress) * 0.6f;
                float returnAngle = petalTime * -3.0f + i * 0.8f; // 反向旋转

                returnPath.x = returnRadius * std::cos(returnAngle);
                returnPath.z = returnRadius * std::sin(returnAngle);
                returnPath.y = std::sin(petalTime * 0.4f + i) * 0.1f * (1.0f - phaseProgress);
            }

            glm::vec3 returnTarget = src_position + returnPath;
            curr_position = glm::mix(naturalPos, returnTarget, smoothStep(phaseProgress));
        }

        // === 旋转和翻滚控制 ===
        float tumblingIntensity = movementIntensity;
        if (currentPhase == HOLD) {
            tumblingIntensity = 0.05f; // 保持时几乎不翻滚
        }

        float tumblingX = std::sin(petalTime * 1.2f + i) * 0.3f * tumblingIntensity;
        float tumblingY = std::cos(petalTime * 0.8f + i * 1.5f) * 0.2f * tumblingIntensity;
        float tumblingZ = std::sin(petalTime * 1.5f + i * 0.7f) * 0.4f * tumblingIntensity;

        // 主轴旋转
        float rotationDamping = (currentPhase == HOLD) ? 0.05f : glm::max(0.1f, movementIntensity);
        angle += glm::radians(2.0f) * (1.0f + 0.5f * std::sin(petalTime + i)) * rotationDamping;

        // === 构建变换矩阵 ===
        glm::mat4 model{1.0f};
        model = glm::translate(model, curr_position);
        model = glm::rotate(model, angle, axis);
        model = glm::rotate(model, tumblingX, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, tumblingY, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, tumblingZ, glm::vec3(0.0f, 0.0f, 1.0f));

        // 尺寸控制
        float scaleVariation = 1.0f + 0.1f * std::sin(petalTime * 0.6f + i) * tumblingIntensity;

        // 组装时的分层效果
        if (currentPhase == ASSEMBLY || currentPhase == FINAL_ASSEMBLY || currentPhase == HOLD) {
            float layerScale = 1.0f - (i % 7) * 0.05f;
            float layerWeight = (currentPhase == HOLD) ? 1.0f : individualSmooth;
            scaleVariation = glm::mix(scaleVariation, layerScale, layerWeight);
        }

        glm::vec3 dynamicScale = scale * scaleVariation;
        model = glm::scale(model, dynamicScale);
        // model = glm::scale(model, scale);

        petalPos[i] = model;
        ++i;
    }
}

static glm::mat4 InitPetal(Petal &petal) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    constexpr float radius_limit = 10.0f;
    std::uniform_real_distribution uniform(0.0f, 1.0f);
    std::uniform_real_distribution angle_distrib(0.0f, 360.0f);

    auto &[curr_position, src_position,target_position,angle,scale, axis] = petal;
    // 圆盘采样
    float u = uniform(gen);
    float v = uniform(gen);
    float r = radius_limit * std::sqrt(u);
    float theta = glm::two_pi<float>() * v;

    float x = r * std::cos(theta);
    float z = r * std::sin(theta);
    src_position = glm::vec3(x, -2.0f, z);
    curr_position = src_position;

    // 随机旋转
    angle = glm::radians(angle_distrib(gen));
    axis = glm::normalize(glm::vec3(uniform(gen), uniform(gen), uniform(gen)));

    scale = glm::vec3(0.1f);

    glm::mat4 model = glm::mat4{1.0f};
    model = glm::translate(model, curr_position);
    model = glm::rotate(model, angle, axis);
    model = glm::scale(model, scale);
    return model;
}

void UpdateStatus(GLFWwindow *window) {
    while (!glfwWindowShouldClose(window)) {
        UpdatePetalAmount();
        // RotatePetal();
        MovePetal();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

static void ProcessInput(GLFWwindow *window) {
    static bool isFullscreen = false;
    static int windowedX, windowedY, windowedWidth, windowedHeight;
    static double glfw_get_time = 0;
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if (glfwGetTime() - glfw_get_time > 0.1) {
            glfw_get_time = glfwGetTime();
            if (!isFullscreen) {
                GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode *mode = glfwGetVideoMode(monitor);
                // 保存原始窗口大小和位置
                glfwGetWindowPos(window, &windowedX, &windowedY);
                glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                isFullscreen = true;
            } else {
                glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
                isFullscreen = false;
            }
        }
    }
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

static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (true)return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    windowAspect = static_cast<float>(width) / static_cast<float>(height);
}

static void MouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
    if (true)return;
    auto x_pos = static_cast<float>(xposIn);
    auto y_pos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = x_pos;
        lastY = y_pos;
        firstMouse = false;
    }

    float x_offset = x_pos - lastX;
    float y_offset = lastY - y_pos; // reversed since y-coordinates go from bottom to top

    lastX = x_pos;
    lastY = y_pos;

    camera.ProcessMouseMovement(x_offset, y_offset);
}

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

    auto res = ShowWindow();
    std::cout << "show_imgui res: " << res << std::endl;

    return 0;
}
