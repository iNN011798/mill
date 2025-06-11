#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <vector>
#include <glm/glm.hpp>
#include <iostream>

class PathManager {
public:
    PathManager(glm::vec3& workpiecePosition, float movementSpeed);

    // ÿ֡�����Ը���ë��λ��
    void Update(float deltaTime);

    // ����Ԥ��� 'e' ��·��
    void StartEPath();

    // ���·����ǰ�Ƿ�����ִ��
    bool IsPathActive() const;

private:
    // ��ʼ�� 'e' ��·���ĺ���
    void InitializeEPath();

    glm::vec3& workpiecePosition_;          // ��ë���������������
    std::vector<glm::vec3> pathWaypoints_;  // �洢·�����������
    int currentWaypointIndex_;              // ��ǰĿ�꺽�������
    float movementSpeed_;                   // ë����·�����ƶ����ٶ�
    bool isPathActive_;                     // ·���Ƿ�����ִ�еı�־
};

#endif // PATH_MANAGER_H 