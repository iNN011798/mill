#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <glad/glad.h>      // ȷ�� GLAD ���ȱ�����
#define GLFW_INCLUDE_NONE   // ���� GLFW ��Ҫ�������Լ��� OpenGL ͷ�ļ�
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>
#include <glm/glm.hpp>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

class FPSRecorder; // ǰ������

class InputHandler {
public:
    InputHandler(Camera& camera,
                 glm::vec3& cubeWorldPosition,
                 glm::vec3& toolBaseWorldPosition,
                 bool& enableMilling,
                 bool& millingKeyPressed,
                 float& deltaTime,
                 FPSRecorder* fpsRecorder); // ��� FPSRecorder ָ��

    void processInput(GLFWwindow *window);

    // ��̬�ص�����������ע�ᵽGLFW
    static void framebuffer_size_callback_glfw(GLFWwindow* window, int width, int height);
    static void mouse_callback_glfw(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback_glfw(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback_glfw(GLFWwindow* window, int key, int scancode, int action, int mods); // ���ڴ������¼�������M�����л�

    // ��Ա�ص�����
    void framebuffer_size_callback(int width, int height);
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(double xoffset, double yoffset);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


    // ������mouse_callback�и������λ��
    float lastX; // ���ڹ��캯���г�ʼ��
    float lastY; // ���ڹ��캯���г�ʼ��
    bool firstMouse = true;

private:
    Camera& camera_;
    glm::vec3& cubeWorldPosition_;
    glm::vec3& toolBaseWorldPosition_;
    bool& enableMilling_;
    bool& millingKeyPressed_;
    float& deltaTime_; // deltaTime ��Ҫ����ѭ�����»�ͨ��InputHandler�Լ�����
    FPSRecorder* fpsRecorder_; // ָ�� FPSRecorder ʵ��
};

#endif // INPUT_HANDLER_H 