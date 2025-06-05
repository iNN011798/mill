#ifndef QUADTREE_NODE_H
#define QUADTREE_NODE_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/mesh.h> // For Vertex struct

// 前向声明
class Quadtree;

class QuadtreeNode {
public:
    // XZ平面上的边界
    glm::vec2 minBounds;
    glm::vec2 maxBounds;
    
    std::vector<Vertex*> vertices; // 存储在该节点的顶点指针
    QuadtreeNode* children[4];     // 子节点指针: 0: NW, 1: NE, 2: SW, 3: SE
    
    int level;                     // 当前节点的层级
    Quadtree* tree;                // 指向所属的 Quadtree

    QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree);
    ~QuadtreeNode();

    bool isLeaf() const;
    void insert(Vertex* vertex);
    void subdivide();
    
    // 查询与给定圆形区域相交的顶点
    void queryRange(const glm::vec2& center, float radius, std::vector<Vertex*>& resultVertices) const;
    
    // 检查一个点是否在此节点的边界内 (XZ平面)
    bool containsPoint(const glm::vec3& pointPosition) const;
    // 检查一个圆形区域是否与此节点的边界相交 (XZ平面)
    bool intersectsCircle(const glm::vec2& center, float radius) const;

    // 新增：打印此节点及其子节点存储的顶点信息 (用于调试)
    void printVertices(int indentLevel = 0) const;

private:
    // 获取点所在的子节点索引
    int getChildIndex(const glm::vec3& pointPosition) const;
};

#endif // QUADTREE_NODE_H 