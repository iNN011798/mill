#include "PathManager.h"
#include <glm/gtc/constants.hpp>

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
    const float scaleX = 0.2f;
    const float scaleZ = 0.25f;

    // 定义毛坯(g_cubeWorldPosition)的移动路径.
    // 我们假设刀具在世界坐标的XZ平面上是固定的(0,0).
    // 毛坯从A点移动到B点，意味着刀具在毛坯自身的坐标系中，雕刻出了一条从-A到-B的路径。

    // 0. 将毛坯移动到第一个切削点。
    // 毛坯位于(-0.2, 0, 0), 此时刀具相对于毛坯的位置是(0.2, y, 0), 这对应'e'横线的最右端。
    pathWaypoints_.push_back(glm::vec3(scaleX, 0.0f, 0.0f));

    // 1. 雕刻'e'的水平横线。
    // 毛坯向右移动，刀具则相对于毛坯向左切割。
    pathWaypoints_.push_back(glm::vec3(-scaleX, 0.0f, 0.0f));

    // 2. 雕刻'e'的C形外壳。
    // 毛坯需要走出一个"反C"形，从而让固定的刀具刻出C形。
    pathWaypoints_.push_back(glm::vec3(-scaleX,     0.0f,  scaleZ));
    pathWaypoints_.push_back(glm::vec3(0.0f,       0.0f,  scaleZ * 1.4f));
    pathWaypoints_.push_back(glm::vec3(scaleX,    0.0f,  scaleZ));
    pathWaypoints_.push_back(glm::vec3(scaleX,    0.0f, -scaleZ));
    pathWaypoints_.push_back(glm::vec3(0.0f,       0.0f, -scaleZ * 1.4f));
    pathWaypoints_.push_back(glm::vec3(-scaleX*0.8f,0.0f, -scaleZ)); // C形不完全闭合

    // 3. 路径结束后，让毛坯返回中心。
    // pathWaypoints_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
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