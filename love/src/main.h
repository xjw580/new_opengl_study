#pragma once
#ifdef MAIN_H
#error "禁止在多个源文件中包含main.h"
#else
#define MAIN_H

struct Petal {
    glm::vec3 curr_position;
    glm::vec3 src_position;
    glm::vec3 target_position;
    float angle;
    glm::vec3 scale;
    glm::vec3 axis;
};

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

void MouseCallback(GLFWwindow *window, double xpos, double ypos);

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

void ProcessInput(GLFWwindow *window);

void InitPetalObjs();

void UpdatePetalAmount();

void RotatePetal(float time = glfwGetTime());

void MovePetal(float time = glfwGetTime());

glm::mat4 InitPetal(Petal &petal);

void UpdateStatus(GLFWwindow *window);

static const char *glsl_version = "#version 450";

// settings
static unsigned int kScrWidth = 1500;
static unsigned int kScrHeight = 1000;

static float windowAspect = static_cast<float>(kScrWidth) / static_cast<float>(kScrHeight);

// camera
static Camera camera(glm::vec3(0.0f, 10.0f, 45.0f));
static float lastX = static_cast<float>(kScrWidth) / 2.0f;
static float lastY = static_cast<float>(kScrHeight) / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

// fram
static float frame = 60;
static float minFgt = 1.0f / frame;

static int petalAmount = 6000;
static int prevPetalAmount = petalAmount;
static std::vector<Petal> petalObjs;
static std::vector<glm::mat4> petalPos;
static char phase[255];
static unsigned int petalBuffer;
static Model *roseModel;
static Shader *ourShader;
static Model *petalModel;
static GLFWwindow *window;

#endif
