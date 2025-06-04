#include "milling_manager.h"
#include <glm/gtc/matrix_transform.hpp> // 用于 glm::inverse, glm::translate
#include <iostream> // Debug

MillingManager::MillingManager(float toolRadius, float toolTipLocalYOffset, float cubeMinLocalY, ToolType toolheadType)
    : toolRadius_(toolRadius),
      toolTipLocalYOffset_(toolTipLocalYOffset),
      cubeMinLocalY_(cubeMinLocalY),
      toolheadType_(toolheadType){}

bool MillingManager::processMilling(Model& cubeModel,
                                    const glm::vec3& cubeWorldPosition,
                                    const glm::vec3& toolBaseWorldPosition,
                                    bool isMillingEnabled) {
    if (!isMillingEnabled) {
        return false;
    }

    // 1. 获取从世界坐标到立方体局部空间的变换矩阵
    glm::mat4 model_cube_matrix = glm::translate(glm::mat4(1.0f), cubeWorldPosition);
    glm::mat4 world_to_cube_local_matrix = glm::inverse(model_cube_matrix);

    // 2. 计算工具尖端在立方体局部空间中的有效位置
    glm::vec3 tool_tip_effective_world_position = toolBaseWorldPosition;
    tool_tip_effective_world_position.y += toolTipLocalYOffset_; // 调整工具尖端的Y偏移
    glm::vec4 tool_tip_world_homogeneous = glm::vec4(tool_tip_effective_world_position, 1.0f);
    glm::vec4 tool_tip_cube_local_homogeneous = world_to_cube_local_matrix * tool_tip_world_homogeneous;
    glm::vec3 tool_tip_cube_local = glm::vec3(tool_tip_cube_local_homogeneous / tool_tip_cube_local_homogeneous.w);

    // 3. 遍历立方体模型的网格和顶点
    bool vertices_modified = false;
    for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
        Mesh& current_mesh = cubeModel.meshes[i]; // 使用引用以修改
        for (unsigned int j = 0; j < current_mesh.vertices.size(); ++j) {
            Vertex& current_vertex = current_mesh.vertices[j]; // 使用引用

            // 计算顶点在XZ平面上与工具尖端投影的2D距离的平方
            float dx = current_vertex.Position.x - tool_tip_cube_local.x;
            float dz = current_vertex.Position.z - tool_tip_cube_local.z;
            float dist_xz_squared = dx * dx + dz * dz;

            if (dist_xz_squared < (toolRadius_ * toolRadius_)) {
                // 顶点在XZ平面上位于工具的切割半径内
                float target_y_cut = glm::max(tool_tip_cube_local.y, cubeMinLocalY_); // 防止切割到立方体定义的最小Y值以下
                if (current_vertex.Position.y > target_y_cut) {
                    switch (toolheadType_)
                    {
                    case flat:
                        current_vertex.Position.y = target_y_cut;
                        break;
                    case ball:
                        current_vertex.Position.y = glm::max(target_y_cut, target_y_cut);
                        break;
                    default:
                        break;
                    }
                    
                    vertices_modified = true;
                }
            }
        }
    }

    // 4. 如果顶点被修改，则更新所有网格的VBO
    if (vertices_modified) {
        for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
            cubeModel.meshes[i].updateVertexBuffer();
        }
    }
    return vertices_modified;
} 