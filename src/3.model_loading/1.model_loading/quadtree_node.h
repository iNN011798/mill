#ifndef QUADTREE_NODE_H
#define QUADTREE_NODE_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/mesh.h> // For Vertex struct

// ǰ������
class Quadtree;

class QuadtreeNode {
public:
    // XZƽ���ϵı߽�
    glm::vec2 minBounds;
    glm::vec2 maxBounds;
    
    std::vector<Vertex*> vertices; // �洢�ڸýڵ�Ķ���ָ��
    QuadtreeNode* children[4];     // �ӽڵ�ָ��: 0: NW, 1: NE, 2: SW, 3: SE
    
    int level;                     // ��ǰ�ڵ�Ĳ㼶
    Quadtree* tree;                // ָ�������� Quadtree

    QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree);
    ~QuadtreeNode();

    bool isLeaf() const;
    void insert(Vertex* vertex);
    void subdivide();
    
    // ��ѯ�����Բ�������ཻ�Ķ���
    void queryRange(const glm::vec2& center, float radius, std::vector<Vertex*>& resultVertices) const;
    
    // ���һ�����Ƿ��ڴ˽ڵ�ı߽��� (XZƽ��)
    bool containsPoint(const glm::vec3& pointPosition) const;
    // ���һ��Բ�������Ƿ���˽ڵ�ı߽��ཻ (XZƽ��)
    bool intersectsCircle(const glm::vec2& center, float radius) const;

    // ��������ӡ�˽ڵ㼰���ӽڵ�洢�Ķ�����Ϣ (���ڵ���)
    void printVertices(int indentLevel = 0) const;

private:
    // ��ȡ�����ڵ��ӽڵ�����
    int getChildIndex(const glm::vec3& pointPosition) const;
};

#endif // QUADTREE_NODE_H 