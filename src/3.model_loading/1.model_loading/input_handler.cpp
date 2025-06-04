#include "input_handler.h"
#include <learnopengl/camera.h> // 确保 Camera 定义可见
#include <iostream> // 用于输出

InputHandler::InputHandler(Camera& camera,
                           glm::vec3& cubeWorldPosition,
                           glm::vec3& toolBaseWorldPosition,
                           bool& enableMilling,
                           bool& millingKeyPressed,
                           float& deltaTime)
    : camera_(camera),
      cubeWorldPosition_(cubeWorldPosition),
      toolBaseWorldPosition_(toolBaseWorldPosition),
      enableMilling_(enableMilling),
      millingKeyPressed_(millingKeyPressed),
      deltaTime_(deltaTime),
      lastX(SCR_WIDTH / 2.0f),
      lastY(SCR_HEIGHT / 2.0f),
      firstMouse(true) {}

// 在processInput中处理持续按键的动作
void InputHandler::processInput(GLFWwindow *window) {
    //std::cout << deltaTime_<< std::endl;
    // 相机控制（WASD键）
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_.ProcessKeyboard(FORWARD, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_.ProcessKeyboard(BACKWARD, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_.ProcessKeyboard(LEFT, deltaTime_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_.ProcessKeyboard(RIGHT, deltaTime_);

    // 毛坯模型控制 (箭头键)
    const float cubeMoveSpeed = 2.5f;
    float actualCubeMoveSpeed = cubeMoveSpeed * deltaTime_;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cubeWorldPosition_.z -= actualCubeMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cubeWorldPosition_.z += actualCubeMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cubeWorldPosition_.x -= actualCubeMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cubeWorldPosition_.x += actualCubeMoveSpeed;

    // 刀具模型控制（page_up/down键）
    const float toolMoveSpeed = 2.5f;
    float actualToolMoveSpeed = toolMoveSpeed * deltaTime_;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        toolBaseWorldPosition_.y += actualToolMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        toolBaseWorldPosition_.y -= actualToolMoveSpeed;
}

// 静态回调函数实现
void InputHandler::framebuffer_size_callback_glfw(GLFWwindow* window, int width, int height) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) {
        handler->framebuffer_size_callback(width, height);
    }
}

void InputHandler::mouse_callback_glfw(GLFWwindow* window, double xpos, double ypos) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) {
        handler->mouse_callback(xpos, ypos);
    }
}

void InputHandler::scroll_callback_glfw(GLFWwindow* window, double xoffset, double yoffset) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) {
        handler->scroll_callback(xoffset, yoffset);
    }
}

void InputHandler::key_callback_glfw(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) {
        handler->key_callback(window, key, scancode, action, mods);
    }
}

// 成员回调函数实现
void InputHandler::framebuffer_size_callback(int width, int height) {
    glViewport(0, 0, width, height);
}

void InputHandler::mouse_callback(double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y坐标从下到上

    lastX = xpos;
    lastY = ypos;

    camera_.ProcessMouseMovement(xoffset, yoffset);
}

void InputHandler::scroll_callback(double xoffset, double yoffset) {
    camera_.ProcessMouseScroll(static_cast<float>(yoffset));
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_M) {
        if (action == GLFW_PRESS && !millingKeyPressed_) { // 只在按下且之前未标记为按下时切换
            enableMilling_ = !enableMilling_;
            millingKeyPressed_ = true; // 标记M键逻辑已处理（等待释放）
            if (enableMilling_) std::cout << "Milling ENABLED" << std::endl;
            else std::cout << "Milling DISABLED" << std::endl;
        } else if (action == GLFW_RELEASE) {
            millingKeyPressed_ = false; // M键已释放，可以再次处理按下事件
        }
    }
}

