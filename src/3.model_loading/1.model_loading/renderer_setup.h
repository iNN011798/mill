#ifndef RENDERER_SETUP_H
#define RENDERER_SETUP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ��������
// ��ʼ��GLFW����������
// window: GLFW���ڶ�������ã����ɸú������������
// title: ���ڱ���
// scr_width, scr_height: ��Ļ�����ڣ��Ŀ�Ⱥ͸߶�
// ����ֵ: ����ɹ����������򷵻� true�����򷵻� false
bool initGLFW(GLFWwindow*& window, const char* title,
    unsigned int scr_width, unsigned int scr_height);

// ��ʼ��GLAD
// ����ֵ: ����ɹ���ʼ��GLAD�򷵻� true�����򷵻� false
bool initGLAD();

// ����ȫ��OpenGL״̬
void configureGlobalOpenGLState();

#endif // RENDERER_SETUP_H 