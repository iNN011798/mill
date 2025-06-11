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
#include <iomanip> // For std::fixed and std::setprecision
#include <sstream> // For std::stringstream

#include "renderer_setup.h"
#include "model_renderer.h"
#include "milling_manager.h"
#include "input_handler.h"
#include "text_renderer.h"
#include "light_source.h"
#include "FPSRecorder.h" // 包含 FPSRecorder

// 全局状态，用于存储模型位置
glm::vec3 g_cubeWorldPosition(0.0f, 0.0f, 0.0f); // 毛坯模型的世界坐标
glm::vec3 g_toolBaseWorldPosition(0.0f, 0.0f, 0.0f); // 刀具模型的基础世界坐标 (XZ固定, Y为浮动中心)

bool g_enableMilling = false;    // 是否启用铣削的开关
bool g_millingKeyPressed = false; // 用于检测铣削按键是否持续按下

const unsigned int SCR_WIDTH = 800;     // opengl 窗体宽高
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.5f, 2.5f));     // 摄像机视角初始位置

float deltaTime = 0.0f;     // 计算时间差的变量
float lastFrame = 0.0f;

// FPS 计算的变量 - 这些将被移入或由 FPSRecorder 管理
// float fpsAccumulator = 0.0f;
// int frameCount = 0;
// float fpsUpdateInterval = 1.0f; // FPS 的更新值
// std::string fpsText = "FPS: 0.0"; // opengl 窗体左上角 FPS 的文字内容

//MillingManager millingManager(0.01f, 0.39f, -0.3f, ToolType::ball); // 示例：使用球头刀，并传入参数
MillingManager millingManager(0.01f, -0.11f, -0.3f, ToolType::ball); // 示例：使用球头刀，并传入参数（刀具半径、刀头y坐标偏移量、毛坯底面偏移量）
// MillingManager millingManager; // 使用默认参数

int main()
{
    GLFWwindow* window = nullptr;
    // 初始化GLFW并创建窗口
    if (!initGLFW(window, "LearnOpenGL_ModelLoading_Refactored", SCR_WIDTH, SCR_HEIGHT))
    {
        return -1;
    }

    // 创建 FPSRecorder 实例
    FPSRecorder fpsRecorder;

    // 创建 InputHandler 实例并设置回调，因为后续会对模型进行移动，因此传入参数时，形参是引用
    InputHandler inputHandler(camera, g_cubeWorldPosition, g_toolBaseWorldPosition, g_enableMilling, g_millingKeyPressed, deltaTime, &fpsRecorder);
    glfwSetWindowUserPointer(window, &inputHandler); // 将handler实例指针传递给GLFW
    // 回调函数：窗体大小、光标移动、滚轮事件、键盘输入
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

    // 初始化文本渲染器
    TextRenderer textRenderer(SCR_WIDTH, SCR_HEIGHT);
    textRenderer.Load(FileSystem::getPath("resources/fonts/Antonio-Bold.ttf"), 24);

    // 构建编译模型、点光源的着色器
    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");
    Shader lightCubeShader("light_cube.vs", "light_cube.fs");

    // 创建光源
    LightSource light(glm::vec3(0.5f, 0.7f, 2.0f));

    // 将blender导出的obj、mtl、jpg等一系列模型文件封装成Model
    // 刀具模型、毛坯模型
    Model cubeModel(FileSystem::getPath("resources/objects/stl/stl.stl"));
    //Model toolModel(FileSystem::getPath("resources/objects/mill/tool/tool.obj"));
    Model toolModel(FileSystem::getPath("resources/objects/obj_tool/tool_obj.obj"));

    // 在模型加载后，初始化 MillingManager 的空间分区结构
    // 您需要根据您的模型调整这些参数
    float surfaceYValue = 0.0f;         // 假设表面主要在 Y=0 附近 (需要根据您的模型调整)
    float surfaceYThreshold = 0.01f;     // Y坐标的容差范围
    int quadtreeMaxLevels = 3;          // 四叉树最大层数
    int quadtreeMaxVertsPerNode = 20;   // 每个叶节点最大顶点数

    // 初始化空间分区结构 (四叉树)
    millingManager.initializeSpatialPartition(cubeModel, surfaceYValue, surfaceYThreshold, quadtreeMaxLevels, quadtreeMaxVertsPerNode);

    // 线框模式
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 使用 FPSRecorder 更新 FPS
        fpsRecorder.Update(deltaTime);

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

        // 更新光源位置
        light.Position.x = 1.0f + sin(glfwGetTime()) * 1.5f;
        light.Position.z = cos(glfwGetTime()) * 1.5f;

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // 设置光照和材质uniforms
        ourShader.setVec3("viewPos", camera.Position);
        light.SetLightUniforms(ourShader);
        
        // 为毛坯设置材质
        ourShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
        ourShader.setFloat("material.shininess", 16.0f);

        // 视图、投影转换
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // 使用新的函数渲染模型
        renderModels(ourShader, cubeModel, toolModel, g_cubeWorldPosition, g_toolBaseWorldPosition, currentFrame);
        
        // 渲染光源立方体
        lightCubeShader.use();
        light.Draw(lightCubeShader, view, projection);

        // 渲染FPS文本
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 为文本渲染切换到填充模式
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        textRenderer.RenderText(fpsRecorder.GetFPSText(), 10.0f, SCR_HEIGHT - 30.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        // 恢复OpenGL状态
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 切换回线框模式

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

