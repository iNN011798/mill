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

// �����´�����ͷ�ļ�
#include "renderer_setup.h"
#include "model_renderer.h"

// cursorģ������˶�
// ȫ��״̬�����ڴ洢ģ��λ��
glm::vec3 g_cubeWorldPosition(0.0f, 0.0f, 0.0f); // ë��ģ�͵���������
//glm::vec3 g_toolBaseWorldPosition(0.0f, 0.0f, 0.0f); // ����ģ�͵Ļ����������� (XZ�̶�, YΪ��������)
glm::vec3 g_toolBaseWorldPosition(0.0f, -0.5f, 0.0f); // ����ģ�͵Ļ����������� (XZ�̶�, YΪ��������)
// end

// �����������и����
const float TOOL_RADIUS = 0.01f; // ���ߵ��и�뾶������ģ�ʹ�С����
bool g_enableMilling = false;    // �Ƿ�����ϳ���Ŀ���
bool g_millingKeyPressed = false; // ���ڼ��ϳ�������Ƿ��������

// �������� (�ص����������봦����)
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
    // ��ʼ��GLFW����������
    if (!initGLFW(window, "LearnOpenGL_ModelLoading_Refactored", framebuffer_size_callback, mouse_callback, scroll_callback, SCR_WIDTH, SCR_HEIGHT))
    {
        return -1;
    }
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
    Model cubeModel(FileSystem::getPath("resources/objects/mill/cube/cube.obj"));
    Model toolModel(FileSystem::getPath("resources/objects/mill/tool/tool.obj"));


    // �߿�ģʽ
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

        // ------ Milling Logic Start ------
        if (g_enableMilling) { // ֻ�е�ϳ������ʱ��ִ��
            // 1. ��ȡ�任����
            // �������ģ�;��� (��ƽ�ƺ����ţ�����ת)
            glm::mat4 model_cube_matrix = glm::mat4(1.0f);
            model_cube_matrix = glm::translate(model_cube_matrix, g_cubeWorldPosition);
            // model_cube_matrix = glm::scale(model_cube_matrix, glm::vec3(1.0f)); // �����������������Ϊ1�����cube.obj�����гߴ磬���������Ҫ�������ģ�Ͷ�ȡ

            // ���ߵ�ģ�;��� (��ƽ�ƺ����ţ�����ת)
            // glm::mat4 model_tool_matrix = glm::mat4(1.0f);
            // model_tool_matrix = glm::translate(model_tool_matrix, g_toolBaseWorldPosition);
            // model_tool_matrix = glm::scale(model_tool_matrix, glm::vec3(1.0f)); // ���蹤�߻�������Ϊ1

            glm::mat4 world_to_cube_local_matrix = glm::inverse(model_cube_matrix);

            // 2. ���߼����������ֲ��ռ������
            // ���蹤�߼���ڹ���ģ�͵ľֲ�ԭ�� (0,0,0) -> ����������� g_toolBaseWorldPosition
            glm::vec4 tool_tip_world = glm::vec4(g_toolBaseWorldPosition, 1.0f); 
            glm::vec4 tool_tip_cube_local_homogeneous = world_to_cube_local_matrix * tool_tip_world;
            glm::vec3 tool_tip_cube_local = glm::vec3(tool_tip_cube_local_homogeneous / tool_tip_cube_local_homogeneous.w); // Perspective divide


            // 3. ����������ģ�͵�ÿ������Ͷ���
            bool vertices_modified = false;
            for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
                Mesh& current_mesh = cubeModel.meshes[i]; // ע���ȡ����
                for (unsigned int j = 0; j < current_mesh.vertices.size(); ++j) {
                    Vertex& current_vertex = current_mesh.vertices[j]; // ע���ȡ����

                    // ���㶥����������ֲ�XZƽ�����빤�߼��ͶӰ��2D����
                    float dx = current_vertex.Position.x - tool_tip_cube_local.x;
                    float dz = current_vertex.Position.z - tool_tip_cube_local.z;
                    float dist_xz_squared = dx * dx + dz * dz; // ʹ�þ����ƽ���Ա��� sqrt

                    if (dist_xz_squared < (TOOL_RADIUS * TOOL_RADIUS)) {
                        if (current_vertex.Position.y > tool_tip_cube_local.y) {
                             current_vertex.Position.y = tool_tip_cube_local.y;
                             vertices_modified = true;
                             // ��ѡ�����·��ߡ��������������㱻�޸ģ��䷨��ָ����Y (�����и�����ƽ��)
                             // current_vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // ע�⣺���ģ���������任��������߿��ܲ���ȷ
                        }
                    }
                }
            }

            // 4. ����ж��㱻�޸ģ������VBO
            if (vertices_modified) {
                for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
                    cubeModel.meshes[i].updateVertexBuffer();
                }
            }
        }
        // ------ Milling Logic End ------

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // Esc���˳����رմ���
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ������ƣ�WASD����
    // wasd�����λ���ƶ�
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    // ë��ģ�Ϳ��� (��ͷ��)
    const float cubeMoveSpeed = 2.5f; // ����ë�����ƶ��ٶ�
    float actualCubeMoveSpeed = cubeMoveSpeed * deltaTime; // ȷ���ƶ��ٶ���֡���޹�

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_cubeWorldPosition.z -= actualCubeMoveSpeed; // ��ǰ (Z�Ḻ����)
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_cubeWorldPosition.z += actualCubeMoveSpeed; // ��� (Z��������)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_cubeWorldPosition.x -= actualCubeMoveSpeed; //���� (X�Ḻ����)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_cubeWorldPosition.x += actualCubeMoveSpeed; // ���� (X��������)

    // ����ģ�Ϳ��ƣ�page_up/down����
    const float toolMoveSpeed = 2.5f;
    float actualToolMoveSpeed = toolMoveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        g_toolBaseWorldPosition.y += actualToolMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        g_toolBaseWorldPosition.y -= actualToolMoveSpeed;

    // ����������ϳ������ (���磬�� M ���л�)
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !g_millingKeyPressed) {
        g_enableMilling = !g_enableMilling;
        g_millingKeyPressed = true;
        if (g_enableMilling) std::cout << "Milling ENABLED" << std::endl;
        else std::cout << "Milling DISABLED" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        g_millingKeyPressed = false;
    }
}

// �����ӿڻص�
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// ����ƶ��ص�
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

    // ����x��y�����ϣ�����һ����������ƫ��
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // ��x��y��������ƫ���������������һ�˳��ӽǵ�ת��
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// ���ֻص�
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
