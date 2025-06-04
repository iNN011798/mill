#include "renderer_setup.h"
#include <iostream> // ���� std::cout

// Forward declaration if needed, or include the header that provides it.
// For stbi_set_flip_vertically_on_load:
#include <stb_image.h> 
// ע��: ��������ط�û�� #define STB_IMAGE_IMPLEMENTATION���ڴ˴���������ܻᵼ�¶��ض��塣
// ���ȷ�����ļ���ĳ���ض��� .cpp �ļ���������塣
// ��������ֻ���ع������� learnopengl/model.h �� model_loading.cpp �Ѿ�����������

bool initGLFW(GLFWwindow*& window, const char* title,
    unsigned int scr_width, unsigned int scr_height)
{
    // glfw: ��ʼ��������
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

    // glfw ���ڴ���
    // --------------------
    window = glfwCreateWindow(scr_width, scr_height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, fb_callback); // �ɵ���������
    // glfwSetCursorPosCallback(window, mouse_callback);   // �ɵ���������
    // glfwSetScrollCallback(window, scroll_callback);     // �ɵ���������

    // ֪ͨGLFW�������ǵ����
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return true;
}

bool initGLAD()
{
    // glad: ��������OpenGL����ָ��
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
    // ����ȫ�� opengl ״̬
    // -----------------------------
    // ���� stb_image.h �ڼ���ģ��ǰ��תy�ᣨ�������꣩
    // �������ͨ���ڼ����κ������ģ��֮ǰ����һ�Ρ�
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
}