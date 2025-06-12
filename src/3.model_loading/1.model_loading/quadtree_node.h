#ifndef QUADTREE_NODE_H
#define QUADTREE_NODE_H

#include <vector>
#include <utility> // For std::pair
#include <cstdint> // For uint64_t
#include <glm/glm.hpp>
#include <learnopengl/mesh.h> // For Vertex struct
#include "morton_code.h"      // 引入我们新的Morton码工具

// 前向声明
class Quadtree;

class QuadtreeNode {
public:
    // XZ平面上的边界
    glm::vec2 minBounds;
    glm::vec2 maxBounds;
    
    // 存储在该节点的顶点指针 (在Z阶排序后可能会被清空)
    std::vector<Vertex*> vertices; 
    QuadtreeNode* children[4];     // 子节点指针: 0: NW, 1: NE, 2: SW, 3: SE
    
    int level;                     // 当前节点的层级
    Quadtree* tree;                // 指向所属的 Quadtree

    // --- Z阶曲线优化新增成员 ---
    bool isZSorted;                                       // 标记此叶子节点是否已进行Z阶排序
    std::vector<std::pair<uint64_t, Vertex*>> zSortedVertices; // 存储按Morton码排序的顶点
    // --- Z阶曲线优化新增成员结束 ---

    QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree);
    ~QuadtreeNode();

    bool isLeaf() const;
    void insert(Vertex* vertex);
    void subdivide();
    
    // 新增：对此节点及其子节点进行Z阶优化
    void optimize();
    
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