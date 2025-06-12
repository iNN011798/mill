#ifndef MILLING_MANAGER_H
#define MILLING_MANAGER_H

#include <learnopengl/model.h>
#include <glm/glm.hpp>
#include <memory> // For std::unique_ptr

// Forward declaration
class Quadtree;

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
                   ToolType toolheadType_ = ToolType::flat);
    ~MillingManager(); // Ϊ�˹��� unique_ptr �������������һ����������

    // ������ڵ���λ�ö�cubeModel��ϳ��������
    // ����κζ��㱻�޸ģ��򷵻�true��
    bool processMilling(Model& cubeModel,
                        const glm::vec3& cubeWorldPosition,
                        const glm::vec3& toolBaseWorldPosition,
                        bool isMillingEnabled);

    // �·�������ʼ���ռ�����ṹ (�Ĳ���)
    // surfaceYValue: ����ʶ����涥���Y����ο�ֵ
    // surfaceYThreshold: Y������surfaceYValue����������ֵ
    // quadtreeMaxLevels: �Ĳ����������� (����3�㣬��Լ64��Ҷ�ӽڵ�)
    // quadtreeMaxVertsPerNode: ÿ��Ҷ�ӽڵ��ڷ���ǰ�������󶥵���
    void initializeSpatialPartition(Model& cubeModel, 
                                    float surfaceYValue, 
                                    float surfaceYThreshold, 
                                    int quadtreeMaxLevels, 
                                    int quadtreeMaxVertsPerNode);

    long long int getNumVertices();
    static long long int numVertices;
private:
    float toolRadius_;
    float toolTipLocalYOffset_;
    float cubeMinLocalY_;
    ToolType toolheadType_; 
    float Y_ball_center;
    float new_Y;
    
    std::unique_ptr<Quadtree> quadtree_; // ʹ������ָ������Ĳ���
};

#endif // MILLING_MANAGER_H 