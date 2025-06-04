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
    MillingManager(float toolRadius = 0.02f,
                   float toolTipLocalYOffset = 0.39f,
                   float cubeMinLocalY = -0.3f,
                   ToolType toolheadType_ = flat);

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
};

#endif // MILLING_MANAGER_H 