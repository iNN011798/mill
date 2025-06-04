#ifndef RENDERER_SETUP_H
#define RENDERER_SETUP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 函数声明
// 初始化GLFW并创建窗口
// window: GLFW窗口对象的引用，将由该函数创建和填充
// title: 窗口标题
// scr_width, scr_height: 屏幕（窗口）的宽度和高度
// 返回值: 如果成功创建窗口则返回 true，否则返回 false
bool initGLFW(GLFWwindow*& window, const char* title,
    unsigned int scr_width, unsigned int scr_height);

// 初始化GLAD
// 返回值: 如果成功初始化GLAD则返回 true，否则返回 false
bool initGLAD();

// 配置全局OpenGL状态
void configureGlobalOpenGLState();

#endif // RENDERER_SETUP_H 