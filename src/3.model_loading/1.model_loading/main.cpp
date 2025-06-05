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

#include "renderer_setup.h"
#include "model_renderer.h"
#include "milling_manager.h"
#include "input_handler.h"

// ȫ��״̬�����ڴ洢ģ��λ��
glm::vec3 g_cubeWorldPosition(0.0f, 0.0f, 0.0f); // ë��ģ�͵���������
glm::vec3 g_toolBaseWorldPosition(0.0f, 0.0f, 0.0f); // ����ģ�͵Ļ����������� (XZ�̶�, YΪ��������)

bool g_enableMilling = false;    // �Ƿ�����ϳ���Ŀ���
bool g_millingKeyPressed = false; // ���ڼ��ϳ�������Ƿ��������

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.5f, 2.5f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

MillingManager millingManager(0.01f, 0.39f, -0.3f, ToolType::ball); // ʾ����ʹ����ͷ�������������
// MillingManager millingManager; // ʹ��Ĭ�ϲ���

int main()
{
    GLFWwindow* window = nullptr;
    // ��ʼ��GLFW����������
    if (!initGLFW(window, "LearnOpenGL_ModelLoading_Refactored", SCR_WIDTH, SCR_HEIGHT))
    {
        return -1;
    }

    // ���� InputHandler ʵ�������ûص�
    InputHandler inputHandler(camera, g_cubeWorldPosition, g_toolBaseWorldPosition, g_enableMilling, g_millingKeyPressed, deltaTime);
    glfwSetWindowUserPointer(window, &inputHandler); // ��handlerʵ��ָ�봫�ݸ�GLFW
    glfwSetFramebufferSizeCallback(window, InputHandler::framebuffer_size_callback_glfw);
    glfwSetCursorPosCallback(window, InputHandler::mouse_callback_glfw);
    glfwSetScrollCallback(window, InputHandler::scroll_callback_glfw);
    glfwSetKeyCallback(window, InputHandler::key_callback_glfw); // ���ð����ص�

    // ��ʼ��GLAD
    if(!initGLAD())
    {
        glfwTerminate();    // ���GLADʧ�ܣ�Ҳ��Ҫ��ֹGLFW
        return -1;
    }

    // ����ȫ��OpenGL״̬ (����stb_image��ת����Ȳ���)
    configureGlobalOpenGLState();

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // ��blender������obj��mtl��jpg��һϵ��ģ���ļ���װ��Model
    // ����ģ�͡�ë��ģ��
    Model cubeModel(FileSystem::getPath("resources/objects/stl/stl.stl"));
    Model toolModel(FileSystem::getPath("resources/objects/mill/tool/tool.obj"));
    //Model toolModel(FileSystem::getPath("resources/objects/obj_tool/tool_obj.obj"));

    // ��ģ�ͼ��غ󣬳�ʼ�� MillingManager �Ŀռ�����ṹ
    // ����Ҫ��������ģ�͵�����Щ����
    float surfaceYValue = 0.0f;         // ���������Ҫ�� Y=0 ���� (��Ҫ��������ģ�͵���)
    float surfaceYThreshold = 0.1f;     // Y������ݲΧ
    int quadtreeMaxLevels = 3;          // �Ĳ���������
    int quadtreeMaxVertsPerNode = 20;   // ÿ��Ҷ�ڵ���󶥵���
    // TODO: ������� cubeModel ��ʵ�ʱ���Yֵ������Ӧ�ص��� surfaceYValue �� surfaceYThreshold��
    // ���磬�������cubeģ���ϱ�����ƽ����Y=0.5���� surfaceYValue������Ϊ0.5f��
    // ���ģ���Ǵ�Blender�����ģ�ԭ�������ģ�͵ļ������ģ����ߵײ���
    // ��������Ҫ�ȼ��һ�� cubeModel.meshes[0].vertices[any_surface_vertex].Position.y ��ֵ��
    millingManager.initializeSpatialPartition(cubeModel, 
                                            surfaceYValue, 
                                            surfaceYThreshold, 
                                            quadtreeMaxLevels, 
                                            quadtreeMaxVertsPerNode);

    // �߿�ģʽ
    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        // processInput(window); // �� InputHandler ����ѭ���д����ͨ���ص�����
        inputHandler.processInput(window); // �����������������

        // ʹ�� MillingManager ����ϳ���߼�
        millingManager.processMilling(cubeModel, g_cubeWorldPosition, g_toolBaseWorldPosition, g_enableMilling);

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

        // ʹ���µĺ�����Ⱦģ��
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

