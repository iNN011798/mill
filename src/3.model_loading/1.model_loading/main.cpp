#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#include "renderer_setup.h"
#include "model_renderer.h"
#include "milling_manager.h"
#include "input_handler.h"

// 全局状态，用于存储模型位置
glm::vec3 g_cubeWorldPosition(0.0f, 0.0f, 0.0f); // 毛坯模型的世界坐标
glm::vec3 g_toolBaseWorldPosition(0.0f, 0.0f, 0.0f); // 刀具模型的基础世界坐标 (XZ固定, Y为浮动中心)

bool g_enableMilling = false;    // 是否启用铣削的开关
bool g_millingKeyPressed = false; // 用于检测铣削按键是否持续按下

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.5f, 2.5f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

MillingManager millingManager;

int main()
{
    GLFWwindow* window = nullptr;
    // 初始化GLFW并创建窗口
    if (!initGLFW(window, "LearnOpenGL_ModelLoading_Refactored", SCR_WIDTH, SCR_HEIGHT))
    {
        return -1;
    }

    // 创建 InputHandler 实例并设置回调
    InputHandler inputHandler(camera, g_cubeWorldPosition, g_toolBaseWorldPosition, g_enableMilling, g_millingKeyPressed, deltaTime);
    glfwSetWindowUserPointer(window, &inputHandler); // 将handler实例指针传递给GLFW
    glfwSetFramebufferSizeCallback(window, InputHandler::framebuffer_size_callback_glfw);
    glfwSetCursorPosCallback(window, InputHandler::mouse_callback_glfw);
    glfwSetScrollCallback(window, InputHandler::scroll_callback_glfw);
    glfwSetKeyCallback(window, InputHandler::key_callback_glfw); // 设置按键回调

    // 初始化GLAD
    if(!initGLAD())
    {
        glfwTerminate();    // 如果GLAD失败，也需要终止GLFW
        return -1;
    }

    // 配置全局OpenGL状态 (包括stb_image翻转和深度测试)
    configureGlobalOpenGLState();

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // 将blender导出的obj、mtl、jpg等一系列模型文件封装成Model
    // 刀具模型、毛坯模型
    Model cubeModel(FileSystem::getPath("resources/objects/stl/stl.stl"));
    //Model toolModel(FileSystem::getPath("resources/objects/mill/tool/tool.obj"));
    Model toolModel(FileSystem::getPath("resources/objects/obj_tool/tool_obj.obj"));


    // 线框模式
    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        // processInput(window); // 由 InputHandler 在主循环中处理或通过回调处理
        inputHandler.processInput(window); // 处理持续按键的输入

        // 使用 MillingManager 处理铣削逻辑
        millingManager.processMilling(cubeModel, g_cubeWorldPosition, g_toolBaseWorldPosition, g_enableMilling);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // 使用新的函数渲染模型
        renderModels(ourShader, cubeModel, toolModel, g_cubeWorldPosition, g_toolBaseWorldPosition, currentFrame);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

