#include "PathManager.h"
#include <glm/gtc/constants.hpp>
#include "Method.h"

// --- ·��ѡ�� ---
// ����Ϊ 1 ʹ��������·��
// ����Ϊ 0 ʹ�÷��ΰ���·��
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
    // --- ·��1�������� ---
    // ���ߵ�·����һ���ӹ������Ŀ�ʼ�������ߡ�
    // ·���㶨���˹�����λ�ã������߹̶�����������ϵ��ԭ�㡣
    // Ϊ���õ����ڹ����Ͽ̻��� (x, z) ��·�������Ǳ��뽫�����ƶ��� (-x, -z)��

    const float maxRadius = 1.0f; // ���뾶����֮ǰ'e'·���ĳߴ�����
    const float numRotations = 9.0f; // �����ߵ�Ȧ��
    const int pointsPerRotation = 360; // ÿȦ�ĵ������Ա�֤·��ƽ��
    const int totalPoints = static_cast<int>(numRotations * pointsPerRotation);
    
    // �ӹ������Ŀ�ʼ����ʱ����Ҳλ�ڹ��������ġ�
    pathWaypoints_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    for (int i = 1; i <= totalPoints; ++i)
    {
        // �����ߵĲ�������
        float angle = static_cast<float>(i) / pointsPerRotation * 2.0f * glm::pi<float>();
        float radius = maxRadius * static_cast<float>(i) / totalPoints;

        float tool_x = radius * cos(angle);
        float tool_z = radius * sin(angle);

        // �ƶ�����������λ�ã���ʵ������ĵ���·��
        pathWaypoints_.push_back(glm::vec3(-tool_x, 0.0f, -tool_z));
    }
#else
    // --- ·��2�����ΰ��ۣ���դ/֮���Σ� ---
    // ·�������Ŀ�ʼ����֮���������ƶ���������һ�����ΰ��ۡ�

    const float pocketSize = 0.4f; // ���ΰ��۵ı߳�
    const float stepover = 0.005f;  // ÿ��ƽ��·����ľ���
    const int numPasses = static_cast<int>(pocketSize / stepover);
    const float halfSize = pocketSize / 2.0f;

    bool movingPositiveX = true; // ��ʼ������X��������

    // ·�������ĵ㿪ʼ
    //pathWaypoints_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    // �ƶ�����һ���ӹ��㣨���½ǣ�
    //pathWaypoints_.push_back(glm::vec3(0.5, 0.0f, 0.5));

    for (int i = 0; i <= numPasses; ++i)
    {
        float currentZ = -halfSize + i * stepover;

        // ��ǰ�ӹ��е������յ�
        float startX = movingPositiveX ? -halfSize : halfSize;
        float endX = movingPositiveX ? halfSize : -halfSize;

        // ��ӵ�ǰ�ӹ��е��յ㣨ע��������Ҫȡ����
        pathWaypoints_.push_back(glm::vec3(-endX, 0.0f, -currentZ));

        // ����������һ�У�������ƶ�����һ�е�·����
        if (i < numPasses) {
            float nextZ = -halfSize + (i + 1) * stepover;
            pathWaypoints_.push_back(glm::vec3(-endX, 0.0f, -nextZ));
        }

        // �л�����
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