#include "PathManager.h"
#include <glm/gtc/constants.hpp>
#include "Method.h"

// --- 路径选择 ---
// 设置为 1 使用螺旋线路径
// 设置为 0 使用方形凹槽路径
// #define USE_SPIRAL_PATH 1 // This is now in config.h

PathManager::PathManager(glm::vec3& workpiecePosition, float movementSpeed)
    : workpiecePosition_(workpiecePosition),
      movementSpeed_(movementSpeed),
      currentWaypointIndex_(0),
      isPathActive_(false) 
{
    InitializeEPath();
}

void PathManager::InitializeEPath() 
{
    pathWaypoints_.clear();

#if USE_SPIRAL_PATH
    // --- 路径1：螺旋线 ---
    // 刀具的路径是一个从工件中心开始的螺旋线。
    // 路径点定义了工件的位置，而刀具固定在世界坐标系的原点。
    // 为了让刀具在工件上刻画出 (x, z) 的路径，我们必须将工件移动到 (-x, -z)。

    const float maxRadius = 1.0f; // 最大半径，与之前'e'路径的尺寸相似
    const float numRotations = 9.0f; // 螺旋线的圈数
    const int pointsPerRotation = 360; // 每圈的点数，以保证路径平滑
    const int totalPoints = static_cast<int>(numRotations * pointsPerRotation);
    
    // 从工件中心开始，此时刀具也位于工件的中心。
    pathWaypoints_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    for (int i = 1; i <= totalPoints; ++i)
    {
        // 螺旋线的参数方程
        float angle = static_cast<float>(i) / pointsPerRotation * 2.0f * glm::pi<float>();
        float radius = maxRadius * static_cast<float>(i) / totalPoints;

        float tool_x = radius * cos(angle);
        float tool_z = radius * sin(angle);

        // 移动工件到反向位置，以实现所需的刀具路径
        pathWaypoints_.push_back(glm::vec3(-tool_x, 0.0f, -tool_z));
    }
#else
    // --- 路径2：方形凹槽（光栅/之字形） ---
    // 路径从中心开始，以之字形来回移动，切削出一个方形凹槽。

    const float pocketSize = 0.4f; // 方形凹槽的边长
    const float stepover = 0.005f;  // 每条平行路径间的距离
    const int numPasses = static_cast<int>(pocketSize / stepover);
    const float halfSize = pocketSize / 2.0f;

    bool movingPositiveX = true; // 初始方向，沿X轴正方向

    // 路径从中心点开始
    //pathWaypoints_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    // 移动到第一个加工点（左下角）
    //pathWaypoints_.push_back(glm::vec3(0.5, 0.0f, 0.5));

    for (int i = 0; i <= numPasses; ++i)
    {
        float currentZ = -halfSize + i * stepover;

        // 当前加工行的起点和终点
        float startX = movingPositiveX ? -halfSize : halfSize;
        float endX = movingPositiveX ? halfSize : -halfSize;

        // 添加当前加工行的终点（注意坐标需要取反）
        pathWaypoints_.push_back(glm::vec3(-endX, 0.0f, -currentZ));

        // 如果不是最后一行，则添加移动到下一行的路径点
        if (i < numPasses) {
            float nextZ = -halfSize + (i + 1) * stepover;
            pathWaypoints_.push_back(glm::vec3(-endX, 0.0f, -nextZ));
        }

        // 切换方向
        movingPositiveX = !movingPositiveX;
    }
#endif
}

void PathManager::StartEPath() 
{
    if (!isPathActive_) {
        isPathActive_ = true;
        currentWaypointIndex_ = 0;
        std::cout << "Starting 'e' path machining..." << std::endl;
    }
}

bool PathManager::IsPathActive() const 
{
    return isPathActive_;
}

void PathManager::Update(float deltaTime) 
{
    if (!isPathActive_) {
        return;
    }

    if (currentWaypointIndex_ >= pathWaypoints_.size()) {
        if (isPathActive_) {
            isPathActive_ = false;
            std::cout << "Path finished." << std::endl;
        }
        return;
    }

    glm::vec3 targetPosition = pathWaypoints_[currentWaypointIndex_];
    glm::vec3 currentPosition = workpiecePosition_;
    
    // 路径规划只在XZ平面上进行
    targetPosition.y = currentPosition.y;

    glm::vec3 direction = targetPosition - currentPosition;
    
    float distance = glm::length(direction);
    const float waypointThreshold = 0.01f;

    if (distance < waypointThreshold) {
        currentWaypointIndex_++;
    } else {
        glm::vec3 moveVector = glm::normalize(direction) * movementSpeed_ * deltaTime;
        // 防止移动过头
        if (glm::length(moveVector) > distance) {
            workpiecePosition_ = targetPosition;
            currentWaypointIndex_++;
        } else {
            workpiecePosition_ += moveVector;
        }
    }
} 