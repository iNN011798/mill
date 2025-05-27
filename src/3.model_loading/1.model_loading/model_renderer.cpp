#include "model_renderer.h"

void renderModels(Shader& shader, Model& cubeModel, Model& toolModel,
    const glm::vec3& cubePosition,
    const glm::vec3& toolBasePosition,
    float time)
{
    // 毛坯模型 (Cube Model)
    glm::vec3 cube_scale = glm::vec3(1.0f, 1.0f, 1.0f); // 毛坯的缩放，根据模型大小调整

    glm::mat4 model_cube = glm::mat4(1.0f);
    model_cube = glm::translate(model_cube, cubePosition); // 使用传入的位置
    model_cube = glm::scale(model_cube, cube_scale);
    shader.setMat4("model", model_cube);
    cubeModel.Draw(shader);

    // 刀具模型 (Tool Model)
    //float tool_y_dynamic_offset = sin(time * 2.0f) * 0.5f; // Y轴浮动：振幅0.5，频率2.0

    //glm::vec3 current_tool_world_position = toolBasePosition; // 从基础位置开始
    //current_tool_world_position.y += tool_y_dynamic_offset; // 应用Y轴的动态偏移

    glm::vec3 tool_scale = glm::vec3(1.0f, 1.0f, 1.0f); // 刀具的缩放，根据模型大小调整

    glm::mat4 model_tool = glm::mat4(1.0f);
    model_tool = glm::translate(model_tool, toolBasePosition);
    model_tool = glm::scale(model_tool, tool_scale);
    shader.setMat4("model", model_tool);
    toolModel.Draw(shader);
}