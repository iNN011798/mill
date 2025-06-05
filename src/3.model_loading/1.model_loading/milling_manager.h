#ifndef MILLING_MANAGER_H
#define MILLING_MANAGER_H

#include <learnopengl/model.h>
#include <glm/glm.hpp>

// 可以将这些设为MillingManager的成员或通过构造函数传入
// const float DEFAULT_TOOL_RADIUS = 0.1f;
// const float DEFAULT_TOOL_TIP_LOCAL_Y_OFFSET = 0.39f;
// const float DEFAULT_CUBE_MIN_LOCAL_Y = -0.3f;
enum ToolType
{
    flat,
    ball,
};
class MillingManager {
public:
    MillingManager(float toolRadius = 0.01f,    // 刀具半径
                   float toolTipLocalYOffset = -0.11f,  // 刀具模型底部y坐标，[0.4, 0.6]高度的模型，其底部y坐标是0.39
                   float cubeMinLocalY = -0.3f, // 毛坯模型的y坐标最小值，即下表面顶点的y值
                   ToolType toolheadType_ = ball);

    // 处理基于刀具位置对cubeModel的铣削操作。
    // 如果任何顶点被修改，则返回true。
    bool processMilling(Model& cubeModel,
                        const glm::vec3& cubeWorldPosition,
                        const glm::vec3& toolBaseWorldPosition,
                        bool isMillingEnabled);

private:
    float toolRadius_;
    float toolTipLocalYOffset_;
    float cubeMinLocalY_;
    ToolType toolheadType_; 
    float Y_ball_center;
    float new_Y;
};

#endif // MILLING_MANAGER_H 