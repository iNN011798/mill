#ifndef QUADTREE_NODE_H
#define QUADTREE_NODE_H

#include <vector>
#include <utility> // For std::pair
#include <cstdint> // For uint64_t
#include <glm/glm.hpp>
#include <learnopengl/mesh.h> // For Vertex struct
#include "morton_code.h"      // ���������µ�Morton�빤��

// ǰ������
class Quadtree;

class QuadtreeNode {
public:
    // XZƽ���ϵı߽�
    glm::vec2 minBounds;
    glm::vec2 maxBounds;
    
    // �洢�ڸýڵ�Ķ���ָ�� (��Z���������ܻᱻ���)
    std::vector<Vertex*> vertices; 
    QuadtreeNode* children[4];     // �ӽڵ�ָ��: 0: NW, 1: NE, 2: SW, 3: SE
    
    int level;                     // ��ǰ�ڵ�Ĳ㼶
    Quadtree* tree;                // ָ�������� Quadtree

    // --- Z�������Ż�������Ա ---
    bool isZSorted;                                       // ��Ǵ�Ҷ�ӽڵ��Ƿ��ѽ���Z������
    std::vector<std::pair<uint64_t, Vertex*>> zSortedVertices; // �洢��Morton������Ķ���
    // --- Z�������Ż�������Ա���� ---

    QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree);
    ~QuadtreeNode();

    bool isLeaf() const;
    void insert(Vertex* vertex);
    void subdivide();
    
    // �������Դ˽ڵ㼰���ӽڵ����Z���Ż�
    void optimize();
    
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