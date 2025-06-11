#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <glad/glad.h>      // 确保 GLAD 首先被包含
#define GLFW_INCLUDE_NONE   // 告诉 GLFW 不要包含它自己的 OpenGL 头文件
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>
#include <glm/glm.hpp>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

class FPSRecorder; // 前向声明

class InputHandler {
public:
    InputHandler(Camera& camera,
                 glm::vec3& cubeWorldPosition,
                 glm::vec3& toolBaseWorldPosition,
                 bool& enableMilling,
                 bool& millingKeyPressed,
                 float& deltaTime,
                 FPSRecorder* fpsRecorder); // 添加 FPSRecorder 指针

    void processInput(GLFWwindow *window);

    // 静态回调函数，用于注册到GLFW
    static void framebuffer_size_callback_glfw(GLFWwindow* window, int width, int height);
    static void mouse_callback_glfw(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback_glfw(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback_glfw(GLFWwindow* window, int key, int scancode, int action, int mods); // 用于处理按键事件，例如M键的切换

    // 成员回调函数
    void framebuffer_size_callback(int width, int height);
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(double xoffset, double yoffset);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


    // 用于在mouse_callback中跟踪鼠标位置
    float lastX; // 将在构造函数中初始化
    float lastY; // 将在构造函数中初始化
    bool firstMouse = true;

private:
    Camera& camera_;
    glm::vec3& cubeWorldPosition_;
    glm::vec3& toolBaseWorldPosition_;
    bool& enableMilling_;
    bool& millingKeyPressed_;
    float& deltaTime_; // deltaTime 需要从主循环更新或通过InputHandler自己管理
    FPSRecorder* fpsRecorder_; // 指向 FPSRecorder 实例
};

#endif // INPUT_HANDLER_H 