#ifndef MILLING_MANAGER_H
#define MILLING_MANAGER_H

#include <learnopengl/model.h>
#include <glm/glm.hpp>
#include <memory> // For std::unique_ptr

// Forward declaration
class Quadtree;

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
                   ToolType toolheadType_ = ToolType::flat);
    ~MillingManager(); // 为了管理 unique_ptr 的析构，最好有一个析构函数

    // 处理基于刀具位置对cubeModel的铣削操作。
    // 如果任何顶点被修改，则返回true。
    bool processMilling(Model& cubeModel,
                        const glm::vec3& cubeWorldPosition,
                        const glm::vec3& toolBaseWorldPosition,
                        bool isMillingEnabled);

    // 新方法：初始化空间分区结构 (四叉树)
    // surfaceYValue: 用于识别表面顶点的Y坐标参考值
    // surfaceYThreshold: Y坐标与surfaceYValue的最大允许差值
    // quadtreeMaxLevels: 四叉树的最大层数 (例如3层，大约64个叶子节点)
    // quadtreeMaxVertsPerNode: 每个叶子节点在分裂前允许的最大顶点数
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
    
    std::unique_ptr<Quadtree> quadtree_; // 使用智能指针管理四叉树
};

#endif // MILLING_MANAGER_H 