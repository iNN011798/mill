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

// 包含新创建的头文件
#include "renderer_setup.h"
#include "model_renderer.h"

// cursor模型相对运动
// 全局状态，用于存储模型位置
glm::vec3 g_cubeWorldPosition(0.0f, 0.0f, 0.0f); // 毛坯模型的世界坐标
glm::vec3 g_toolBaseWorldPosition(0.0f, 0.0f, 0.0f); // 刀具模型的基础世界坐标 (XZ固定, Y为浮动中心)
// end

// 函数声明 (回调函数和输入处理函数)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(1.5f, 0.5f, 1.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    GLFWwindow* window = nullptr;
    // 初始化GLFW并创建窗口
    if (!initGLFW(window, "LearnOpenGL_ModelLoading_Refactored", framebuffer_size_callback, mouse_callback, scroll_callback, SCR_WIDTH, SCR_HEIGHT))
    {
        return -1;
    }
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
    Model cubeModel(FileSystem::getPath("resources/objects/download/blenderCube/cubeWithPicture.obj"));
    Model toolModel(FileSystem::getPath("resources/objects/download/tool/tool.obj"));


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

        // input
        // -----
        processInput(window);

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // Esc：退出，关闭窗体
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 相机控制（WASD键）
    // wasd：相机位置移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    // 毛坯模型控制 (箭头键)
    const float cubeMoveSpeed = 2.5f; // 定义毛坯的移动速度
    float actualCubeMoveSpeed = cubeMoveSpeed * deltaTime; // 确保移动速度与帧率无关

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_cubeWorldPosition.z -= actualCubeMoveSpeed; // 向前 (Z轴负方向)
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_cubeWorldPosition.z += actualCubeMoveSpeed; // 向后 (Z轴正方向)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_cubeWorldPosition.x -= actualCubeMoveSpeed; //向左 (X轴负方向)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_cubeWorldPosition.x += actualCubeMoveSpeed; // 向右 (X轴正方向)

    // 刀具模型控制（page_up/down键）
    const float toolMoveSpeed = 2.5f;
    float actualToolMoveSpeed = toolMoveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        g_toolBaseWorldPosition.y += actualToolMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        g_toolBaseWorldPosition.y -= actualToolMoveSpeed;
}

// 窗体视口回调
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 鼠标移动回调
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // 计算x、y方向上，和上一次鼠标坐标的偏移
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // 对x、y方向的鼠标偏移量进行摄像机第一人称视角的转动
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 滚轮回调
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
