#include "quadtree_node.h"
#include "quadtree.h" // 需要访问 Quadtree 的 maxLevels 和 maxVerticesPerNode
#include <algorithm> // For std::max and std::min for intersection checks
#include <iostream> // For std::cout

QuadtreeNode::QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree)
    : minBounds(minB), maxBounds(maxB), level(lvl), tree(ownerTree) {
    for (int i = 0; i < 4; ++i) {
        children[i] = nullptr;
    }
}

QuadtreeNode::~QuadtreeNode() {
    for (int i = 0; i < 4; ++i) {
        delete children[i];
        children[i] = nullptr;
    }
    // vertices vector will clear itself, as it stores pointers, not objects.
    // The actual Vertex objects are owned by the Mesh class.
}

bool QuadtreeNode::isLeaf() const {
    return children[0] == nullptr; // 如果第一个子节点为空，则认为是叶节点
}

void QuadtreeNode::subdivide() {
    if (level >= tree->maxLevels) return; //达到最大层级，不再分裂

    glm::vec2 halfSize = (maxBounds - minBounds) / 2.0f;
    glm::vec2 center = minBounds + halfSize;

    // 子节点顺序: NW, NE, SW, SE
    // 0: NW (minBounds.x, center.y) to (center.x, maxBounds.y)
    children[0] = new QuadtreeNode(glm::vec2(minBounds.x, center.y), 
                                   glm::vec2(center.x, maxBounds.y), 
                                   level + 1, tree);
    // 1: NE (center.x, center.y) to (maxBounds.x, maxBounds.y)
    children[1] = new QuadtreeNode(center, 
                                   maxBounds, 
                                   level + 1, tree);
    // 2: SW (minBounds.x, minBounds.y) to (center.x, center.y)
    children[2] = new QuadtreeNode(minBounds, 
                                   center, 
                                   level + 1, tree);
    // 3: SE (center.x, minBounds.y) to (maxBounds.x, center.y)
    children[3] = new QuadtreeNode(glm::vec2(center.x, minBounds.y), 
                                   glm::vec2(maxBounds.x, center.y), 
                                   level + 1, tree);

    // 将当前节点的顶点分配到子节点
    std::vector<Vertex*> tempVertices = vertices;
    //std::cout << "now the node's num of vertices is " << tempVertices.size() << endl;
    vertices.clear();

    for (Vertex* vertex : tempVertices) {
        insert(vertex); // 重新插入到当前节点，它会自动分配到子节点
    }
}

void QuadtreeNode::insert(Vertex* vertex) {
    if (!containsPoint(vertex->Position)) {
        return; // 顶点不在此节点边界内
    }

    if (!isLeaf()) {
        int index = getChildIndex(vertex->Position);
        if (index != -1) {
            children[index]->insert(vertex);
            return;
        }
    } 
    // 如果是叶子节点，或者无法确定子节点（边界情况），则添加到当前节点
    vertices.push_back(vertex);

    // 如果当前是叶子节点，且顶点数量超过限制，并且未达到最大层级，则分裂
    if (isLeaf() && vertices.size() > (size_t)tree->maxVerticesPerNode && level < tree->maxLevels) {
        subdivide();
    }
}

// 检查点是否在节点的XZ边界内
bool QuadtreeNode::containsPoint(const glm::vec3& pointPosition) const {
    return (pointPosition.x >= minBounds.x && pointPosition.x <= maxBounds.x &&
            pointPosition.z >= minBounds.y && pointPosition.z <= maxBounds.y);
            // 注意：minBounds.y 和 maxBounds.y 对应 Z 坐标
}

int QuadtreeNode::getChildIndex(const glm::vec3& pointPosition) const {
    glm::vec2 center = minBounds + (maxBounds - minBounds) / 2.0f;
    bool topQuadrant = pointPosition.z >= center.y;    // Z >= center.z
    bool bottomQuadrant = pointPosition.z < center.y; // Z < center.z
    bool leftQuadrant = pointPosition.x < center.x;   // X < center.x
    bool rightQuadrant = pointPosition.x >= center.x; // X >= center.x

    if (topQuadrant) {
        if (leftQuadrant) return 0; // NW
        if (rightQuadrant) return 1; // NE
    } else if (bottomQuadrant) {
        if (leftQuadrant) return 2; // SW
        if (rightQuadrant) return 3; // SE
    }
    return -1; // Should not happen if point is within bounds
}

// 检查圆形查询区域是否与此节点的边界框相交
bool QuadtreeNode::intersectsCircle(const glm::vec2& center, float radius) const {
    // 圆心到矩形边界最近点的距离的平方
    float closestX = (std::max)(minBounds.x, (std::min)(center.x, maxBounds.x));
    float closestZ = (std::max)(minBounds.y, (std::min)(center.y, maxBounds.y)); // minBounds.y is Z

    float distanceX = center.x - closestX;
    float distanceZ = center.y - closestZ;

    return (distanceX * distanceX + distanceZ * distanceZ) <= (radius * radius);
}

void QuadtreeNode::queryRange(const glm::vec2& center, float radius, std::vector<Vertex*>& resultVertices) const {
    if (!intersectsCircle(center, radius)) {
        return; // 此节点与查询范围不相交
    }

    if (isLeaf()) {
        // 如果是叶节点，检查此节点中的所有顶点
        for (Vertex* vertex : vertices) {
            // 再次确认顶点在圆形范围内 (因为节点边界是矩形)
            float dx = vertex->Position.x - center.x;
            float dz = vertex->Position.z - center.y; // center.y is query_center_z
            if ((dx * dx + dz * dz) <= (radius * radius)) {
                resultVertices.push_back(vertex);
            }
        }
    } else {
        // 如果不是叶节点，递归查询子节点
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                children[i]->queryRange(center, radius, resultVertices);
            }
        }
    }
}

void QuadtreeNode::printVertices(int indentLevel) const {
    std::string indent(indentLevel * 2, ' '); // 创建缩进字符串

    std::cout << indent << "Node Level: " << level 
              << ", Bounds: [(" << minBounds.x << ", " << minBounds.y << ") to (" 
              << maxBounds.x << ", " << maxBounds.y << ")]"
              << ", Vertices directly in this node: " << vertices.size(); // 只显示本节点直接存储的顶点数

    if (isLeaf()) {
        std::cout << " (Leaf Node)" << std::endl;
        // 不再打印叶子节点的详细顶点坐标
        // if (!vertices.empty()) {
        //     std::cout << indent << "  Leaf Vertices (X, Y, Z):" << std::endl;
        //     for (const Vertex* v_ptr : vertices) {
        //         if (v_ptr) { 
        //             std::cout << indent << "    (" 
        //                       << v_ptr->Position.x << ", " 
        //                       << v_ptr->Position.y << ", " 
        //                       << v_ptr->Position.z << ")" << std::endl;
        //         }
        //     }
        // }
    } else {
        std::cout << " (Internal Node)" << std::endl;
        // std::cout << indent << "  Children:" << std::endl; // 可以省略这行，直接列出子节点
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                // std::cout << indent << "  Child [" << i << "] :" << std::endl; // 也可以省略，让子节点的输出自带缩进
                children[i]->printVertices(indentLevel + 1); // 缩进级别调整为+1，使结构更紧凑
            } else {
                 // 对于内部节点，通常在分裂后其自身 vertices 会清空，除非未分裂或顶点无法分配
                 // 如果需要显示空的子节点占位符，可以取消注释下一行
                 // std::cout << indent << std::string((indentLevel + 1) * 2, ' ') << "Child [" << i << "] : nullptr" << std::endl;
            }
        }
    }
} 