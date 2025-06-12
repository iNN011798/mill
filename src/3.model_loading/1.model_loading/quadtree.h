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
    int maxVerticesPerNode; // 叶子节点在分裂前可以容纳的最大顶点数

    // 构造函数需要整个场景或相关顶点集的XZ边界
    Quadtree(glm::vec2 minBounds, glm::vec2 maxBounds, int maxLvl, int maxVertsPerNode);
    ~Quadtree();

    void insert(Vertex* vertex);
    // 查询与给定圆形区域相交的顶点 (XZ平面)
    std::vector<Vertex*> queryRange(const glm::vec2& center, float radius) const;

    // 新增：对树的叶子节点进行Z阶曲线优化
    void optimize();

    void clear(); // 清空树（删除所有节点和顶点指针）

    // 新增：打印整个树的内容 (用于调试)
    void printTreeContents() const;

private:
    void clearRecursive(QuadtreeNode* node);
};

#endif // QUADTREE_H 