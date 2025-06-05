#ifndef MILLING_MANAGER_H
#define MILLING_MANAGER_H

#include <learnopengl/model.h>
#include <glm/glm.hpp>

// ���Խ���Щ��ΪMillingManager�ĳ�Ա��ͨ�����캯������
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
    MillingManager(float toolRadius = 0.01f,    // ���߰뾶
                   float toolTipLocalYOffset = -0.11f,  // ����ģ�͵ײ�y���꣬[0.4, 0.6]�߶ȵ�ģ�ͣ���ײ�y������0.39
                   float cubeMinLocalY = -0.3f, // ë��ģ�͵�y������Сֵ�����±��涥���yֵ
                   ToolType toolheadType_ = ball);

    // ������ڵ���λ�ö�cubeModel��ϳ��������
    // ����κζ��㱻�޸ģ��򷵻�true��
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