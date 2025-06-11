#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <vector>
#include <glm/glm.hpp>
#include <iostream>

class PathManager {
public:
    PathManager(glm::vec3& workpiecePosition, float movementSpeed);

    // 每帧调用以更新毛坯位置
    void Update(float deltaTime);

    // 启动预设的 'e' 形路径
    void StartEPath();

    // 检查路径当前是否正在执行
    bool IsPathActive() const;

private:
    // 初始化 'e' 形路径的航点
    void InitializeEPath();

    glm::vec3& workpiecePosition_;          // 对毛坯世界坐标的引用
    std::vector<glm::vec3> pathWaypoints_;  // 存储路径航点的向量
    int currentWaypointIndex_;              // 当前目标航点的索引
    float movementSpeed_;                   // 毛坯在路径上移动的速度
    bool isPathActive_;                     // 路径是否正在执行的标志
};

#endif // PATH_MANAGER_H 