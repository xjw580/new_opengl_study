/**************************************************************************

Author:肖嘉威

Version:1.0.0

Date:2025/5/26 12:24

Description:

**************************************************************************/
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
//
// #include <learnopengl/shader_m.h>
// #include <learnopengl/camera.h>
// #include <learnopengl/model.h>
// #include <filesystem.h>
//
// #include <iostream>
// #include <random>
//
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
//
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
//
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
//
// void processInput(GLFWwindow *window);
//
// // settings
// const unsigned int SCR_WIDTH = 2000;
// const unsigned int SCR_HEIGHT = 1500;
//
// // camera
// Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
// bool firstMouse = true;
//
// // timing
// float deltaTime = 0.0f;
// float lastFrame = 0.0f;
//
// // fram
// float frame = 60;
// float min_fgt = 1.0f / frame;
//
// struct Obj {
//     glm::vec3 position;
//     float angle;
//     glm::vec3 scale;
//     glm::vec3 axis;
// };
//
// std::vector<Obj> petalPositions;
//
// void initPetalPositions() {
//     petalPositions.resize(20);
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     constexpr float pos_limit = 2.0f;
//     std::uniform_real_distribution pos_distrib(-pos_limit, pos_limit);
//     std::uniform_real_distribution axis_distrib(0.0f, 1.0f);
//     std::uniform_real_distribution angle_distrib(0.0f, 360.0f);
//     for (auto &[position, angle, scale, axis]: petalPositions) {
//         position = glm::vec3(pos_distrib(gen), -1.0f, pos_distrib(gen));
//         angle = glm::radians(angle_distrib(gen));
//         scale = glm::vec3(0.2);
//         axis = glm::vec3(axis_distrib(gen), axis_distrib(gen), axis_distrib(gen));
//     }
// }
//
// int main() {
//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif
//
//     // glfw window creation
//     // --------------------
//     GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
//     if (window == nullptr) {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     glfwSetCursorPosCallback(window, mouse_callback);
//     glfwSetScrollCallback(window, scroll_callback);
//
//     // tell GLFW to capture our mouse
//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//     // glad: load all OpenGL function pointers
//     // ---------------------------------------
//     if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }
//
//     // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//     stbi_set_flip_vertically_on_load(true);
//
//     // configure global opengl state
//     // -----------------------------
//     glEnable(GL_DEPTH_TEST);
//
//     // build and compile shaders
//     // -------------------------
//     // Shader ourShader(FileSystem::getPath(R"(resources/1.model_loading.vert)").c_str(), FileSystem::getPath(R"(resources/1.model_loading.frag)").c_str());
//     Shader ourShader{
//         FileSystem::getPath(R"(resources/shader/love/assimp_demo.vert)").c_str(),
//         FileSystem::getPath(R"(resources/shader/love/assimp_demo.frag)").c_str()
//     };
//
//     Model petalModel{FileSystem::getPath(R"(resources/objects/petal/new_petal1.obj)")};
//
//     initPetalPositions();
//
//
//     // draw in wireframe
//     //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//     // render loop
//     // -----------
//     while (!glfwWindowShouldClose(window)) {
//         // per-frame time logic
//         // --------------------
//         auto currentFrame = static_cast<float>(glfwGetTime());
//         deltaTime = (currentFrame - lastFrame) * 2;
//         // if (deltaTime < min_fgt)
//         // {
//         //     double sleepTime = min_fgt - deltaTime;
//         //     glfwWaitEventsTimeout(sleepTime); // 让线程等待剩余的时间
//         // }
//         lastFrame = static_cast<float>(glfwGetTime());
//
//         // input
//         // -----
//         processInput(window);
//
//         // render
//         // ------
//         glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//         // don't forget to enable shader before setting uniforms
//         ourShader.use();
//         ourShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f)); // 光源位置
//         ourShader.setVec3("viewPos", camera.Position); // 相机位置
//         ourShader.setVec3("lightColor", glm::vec3(1.0f, 0.75f, 0.79f)); // 光源颜色
//
//         // view/projection transformations
//         glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
//                                                 100.0f);
//         // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
//         glm::mat4 view = camera.GetViewMatrix();
//         ourShader.setMat4("projection", projection);
//         ourShader.setMat4("view", view);
//
//         for (const auto &[position, angle,scale, axis]: petalPositions) {
//             glm::mat4 model = glm::mat4{1.0f};
//             model = glm::translate(model, position);
//             model = glm::rotate(model, angle, axis);
//             model = glm::scale(model, scale);
//             ourShader.setMat4("model", model);
//             petalModel.Draw(ourShader);
//         }
//
//
//         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//         // -------------------------------------------------------------------------------
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }
//
//     // glfw: terminate, clearing all previously allocated GLFW resources.
//     // ------------------------------------------------------------------
//     glfwTerminate();
//     return 0;
// }
//
// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window) {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         camera.ProcessKeyboard(FORWARD, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         camera.ProcessKeyboard(BACKWARD, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         camera.ProcessKeyboard(LEFT, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         camera.ProcessKeyboard(RIGHT, deltaTime);
// }
//
//
// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
//     // make sure the viewport matches the new window dimensions; note that width and
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }
//
// // glfw: whenever the mouse moves, this callback is called
// // -------------------------------------------------------
// void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
//     float xpos = static_cast<float>(xposIn);
//     float ypos = static_cast<float>(yposIn);
//
//     if (firstMouse) {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = false;
//     }
//
//     float xoffset = xpos - lastX;
//     float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//     lastX = xpos;
//     lastY = ypos;
//
//     camera.ProcessMouseMovement(xoffset, yoffset);
// }
//
