#include "renderer_setup.h"
#include <iostream> // 用于 std::cout

// Forward declaration if needed, or include the header that provides it.
// For stbi_set_flip_vertically_on_load:
#include <stb_image.h> 
// 注意: 如果其他地方没有 #define STB_IMAGE_IMPLEMENTATION，在此处添加它可能会导致多重定义。
// 最好确保主文件或某个特定的 .cpp 文件有这个定义。
// 鉴于我们只是重构，假设 learnopengl/model.h 或 model_loading.cpp 已经处理了它。

bool initGLFW(GLFWwindow*& window, const char* title,
    unsigned int scr_width, unsigned int scr_height)
{
    // glfw: 初始化并配置
    // ------------------------------
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw 窗口创建
    // --------------------
    window = glfwCreateWindow(scr_width, scr_height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, fb_callback); // 由调用者设置
    // glfwSetCursorPosCallback(window, mouse_callback);   // 由调用者设置
    // glfwSetScrollCallback(window, scroll_callback);     // 由调用者设置

    // 通知GLFW捕获我们的鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return true;
}

bool initGLAD()
{
    // glad: 加载所有OpenGL函数指针
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

void configureGlobalOpenGLState()
{
    // 配置全局 opengl 状态
    // -----------------------------
    // 告诉 stb_image.h 在加载模型前翻转y轴（纹理坐标）
    // 这个函数通常在加载任何纹理或模型之前调用一次。
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
}