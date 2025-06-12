#ifndef QUADTREE_H
#define QUADTREE_H

#include "quadtree_node.h"
#include <vector>
#include <glm/glm.hpp>
// #include <learnopengl/mesh.h> // Vertex is included via quadtree_node.h

class Quadtree {
public:
    QuadtreeNode* root;
    int maxLevels;
    int maxVerticesPerNode; // Ҷ�ӽڵ��ڷ���ǰ�������ɵ���󶥵���

    // ���캯����Ҫ������������ض��㼯��XZ�߽�
    Quadtree(glm::vec2 minBounds, glm::vec2 maxBounds, int maxLvl, int maxVertsPerNode);
    ~Quadtree();

    void insert(Vertex* vertex);
    // ��ѯ�����Բ�������ཻ�Ķ��� (XZƽ��)
    std::vector<Vertex*> queryRange(const glm::vec2& center, float radius) const;

    // ������������Ҷ�ӽڵ����Z�������Ż�
    void optimize();

    void clear(); // �������ɾ�����нڵ�Ͷ���ָ�룩

    // ��������ӡ������������ (���ڵ���)
    void printTreeContents() const;

private:
    void clearRecursive(QuadtreeNode* node);
};

#endif // QUADTREE_H 