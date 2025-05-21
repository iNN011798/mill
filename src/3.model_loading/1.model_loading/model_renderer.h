#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // 用于 glm::translate, glm::scale

// 渲染两个模型：毛坯和刀具
// shader: 使用的着色器程序
// cubeModel: 毛坯模型对象
// toolModel: 刀具模型对象
// cubePosition: 毛坯模型在世界坐标系中的位置
// toolBasePosition: 刀具模型的基础世界坐标 (XZ固定, Y为浮动中心)
// time: 当前时间，用于刀具的动画
void renderModels(Shader& shader, Model& cubeModel, Model& toolModel,
    const glm::vec3& cubePosition,
    const glm::vec3& toolBasePosition,
    float time);

#endif // MODEL_RENDERER_H 