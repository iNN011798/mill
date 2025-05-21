#include "renderer_setup.h"
#include <iostream> // 用于 std::cout

// learnopengl/stb_image.h 需要这个定义，我们将其放在.cpp中以避免多重定义
// 通常这个定义会在包含stb_image.h的那个.cpp文件的顶部，或者在全局配置文件中
// 这里假设 model_loading.cpp 中会有 #include <learnopengl/model.h> 进而包含 stb_image
// 如果没有，并且 configureGlobalOpenGLState 中调用了 stbi_ 函数，则需要确保 stb_image.h 被包含
// 但由于stbi_set_flip_vertically_on_load是在model.h中处理的，这里不需要显式包含stb_image.h或定义STB_IMAGE_IMPLEMENTATION
// 我们将stbi_set_flip_vertically_on_load的调用放在 configureGlobalOpenGLState 中

// 引入stb_image.h的依赖，以便调用stbi_set_flip_vertically_on_load
// 这个通常是在model.h中处理的，但为了确保函数独立性，最好在这里明确。
// 不过，更规范的做法是让调用stbi_set_flip_vertically_on_load的地方确保stb_image.h已被正确包含和配置。
// 为了简单起见，这里假设它在主文件 (model_loading.cpp) 中通过 #include <learnopengl/model.h> 间接包含。
// 如果直接在此处使用stbi函数，则需要 #define STB_IMAGE_IMPLEMENTATION 和 #include "stb_image.h"
// 但因为我们只封装 glEnable 和未来可能的其他OpenGL状态，stbi的调用仍可保留在main或移到model loading部分。
// 让我们将 stbi_set_flip_vertically_on_load(true); 放在 configureGlobalOpenGLState()。
// 要这样做，需要能访问到这个函数。它是在 learnopengl/model.h 中被调用的，间接来自于 stb_image.h。
// 这意味着 model.h 或其依赖项应该处理stb_image的包含。
// 实际上，`stbi_set_flip_vertically_on_load` 是 stb_image.h 中的函数。
// `learnopengl/model.h` 中有 `#include <stb_image.h>`。
// 通常，`#define STB_IMAGE_IMPLEMENTATION` 会放在一个 cpp 文件中，以避免链接错误。
// learnopengl 教程中，这个通常是在主 cpp 文件或者某个只包含一次的 cpp 文件。
// 假设 `model_loading.cpp` 是包含 `STB_IMAGE_IMPLEMENTATION` 的地方（如果它还没有的话）
// 或者 `learnopengl/model.h` 内部已经处理。
// 我们这里只调用这个函数，依赖于它在别处被正确链接。

// Forward declaration if needed, or include the header that provides it.
// For stbi_set_flip_vertically_on_load:
#include <stb_image.h> 
// 注意: 如果其他地方没有 #define STB_IMAGE_IMPLEMENTATION，在此处添加它可能会导致多重定义。
// 最好确保主文件或某个特定的 .cpp 文件有这个定义。
// 鉴于我们只是重构，假设 learnopengl/model.h 或 model_loading.cpp 已经处理了它。

bool initGLFW(GLFWwindow*& window, const char* title,
    void (*fb_callback)(GLFWwindow*, int, int),
    void (*mouse_callback)(GLFWwindow*, double, double),
    void (*scroll_callback)(GLFWwindow*, double, double),
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
    glfwSetFramebufferSizeCallback(window, fb_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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