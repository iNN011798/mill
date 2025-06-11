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

    // ����ë��(g_cubeWorldPosition)���ƶ�·��.
    // ���Ǽ��赶�������������XZƽ�����ǹ̶���(0,0).
    // ë����A���ƶ���B�㣬��ζ�ŵ�����ë�����������ϵ�У���̳���һ����-A��-B��·����

    // 0. ��ë���ƶ�����һ�������㡣
    // ë��λ��(-0.2, 0, 0), ��ʱ���������ë����λ����(0.2, y, 0), ���Ӧ'e'���ߵ����Ҷˡ�
    pathWaypoints_.push_back(glm::vec3(scaleX, 0.0f, 0.0f));

    // 1. ���'e'��ˮƽ���ߡ�
    // ë�������ƶ��������������ë�������и
    pathWaypoints_.push_back(glm::vec3(-scaleX, 0.0f, 0.0f));

    // 2. ���'e'��C����ǡ�
    // ë����Ҫ�߳�һ��"��C"�Σ��Ӷ��ù̶��ĵ��߿̳�C�Ρ�
    pathWaypoints_.push_back(glm::vec3(-scaleX,     0.0f,  scaleZ));
    pathWaypoints_.push_back(glm::vec3(0.0f,       0.0f,  scaleZ * 1.4f));
    pathWaypoints_.push_back(glm::vec3(scaleX,    0.0f,  scaleZ));
    pathWaypoints_.push_back(glm::vec3(scaleX,    0.0f, -scaleZ));
    pathWaypoints_.push_back(glm::vec3(0.0f,       0.0f, -scaleZ * 1.4f));
    pathWaypoints_.push_back(glm::vec3(-scaleX*0.8f,0.0f, -scaleZ)); // C�β���ȫ�պ�

    // 3. ·����������ë���������ġ�
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
    
    // ·���滮ֻ��XZƽ���Ͻ���
    targetPosition.y = currentPosition.y;

    glm::vec3 direction = targetPosition - currentPosition;
    
    float distance = glm::length(direction);
    const float waypointThreshold = 0.01f;

    if (distance < waypointThreshold) {
        currentWaypointIndex_++;
    } else {
        glm::vec3 moveVector = glm::normalize(direction) * movementSpeed_ * deltaTime;
        // ��ֹ�ƶ���ͷ
        if (glm::length(moveVector) > distance) {
            workpiecePosition_ = targetPosition;
            currentWaypointIndex_++;
        } else {
            workpiecePosition_ += moveVector;
        }
    }
} 