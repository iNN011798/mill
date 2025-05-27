#include "model_renderer.h"

void renderModels(Shader& shader, Model& cubeModel, Model& toolModel,
    const glm::vec3& cubePosition,
    const glm::vec3& toolBasePosition,
    float time)
{
    // ë��ģ�� (Cube Model)
    glm::vec3 cube_scale = glm::vec3(1.0f, 1.0f, 1.0f); // ë�������ţ�����ģ�ʹ�С����

    glm::mat4 model_cube = glm::mat4(1.0f);
    model_cube = glm::translate(model_cube, cubePosition); // ʹ�ô����λ��
    model_cube = glm::scale(model_cube, cube_scale);
    shader.setMat4("model", model_cube);
    cubeModel.Draw(shader);

    // ����ģ�� (Tool Model)
    //float tool_y_dynamic_offset = sin(time * 2.0f) * 0.5f; // Y�ḡ�������0.5��Ƶ��2.0

    //glm::vec3 current_tool_world_position = toolBasePosition; // �ӻ���λ�ÿ�ʼ
    //current_tool_world_position.y += tool_y_dynamic_offset; // Ӧ��Y��Ķ�̬ƫ��

    glm::vec3 tool_scale = glm::vec3(1.0f, 1.0f, 1.0f); // ���ߵ����ţ�����ģ�ʹ�С����

    glm::mat4 model_tool = glm::mat4(1.0f);
    model_tool = glm::translate(model_tool, toolBasePosition);
    model_tool = glm::scale(model_tool, tool_scale);
    shader.setMat4("model", model_tool);
    toolModel.Draw(shader);
}