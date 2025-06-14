#include "quadtree_node.h"
#include "quadtree.h" // 需要访问 Quadtree 的 maxLevels 和 maxVerticesPerNode
#include <algorithm> // For std::max and std::min for intersection checks, and std::sort
#include <iostream> // For std::cout
#include "Method.h"

QuadtreeNode::QuadtreeNode(glm::vec2 minB, glm::vec2 maxB, int lvl, Quadtree* ownerTree)
    : minBounds(minB), maxBounds(maxB), level(lvl), tree(ownerTree), isZSorted(false) {
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

void QuadtreeNode::optimize() {
    // 如果是内部节点，则递归优化子节点
    if (!isLeaf()) {
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                children[i]->optimize();
            }
        }
        return;
    }

    // 如果是叶子节点，并且顶点数量大于一个阈值，则进行Z阶排序
    // 我们只对"臃肿"的叶子节点进行优化，避免不必要的计算
    const size_t Z_SORT_THRESHOLD = 50; 
    if (vertices.size() > Z_SORT_THRESHOLD) {
        isZSorted = true;
        
        // 预留空间以提高效率
        zSortedVertices.reserve(vertices.size());

        // 计算每个顶点的Morton码
        for (Vertex* vertex : vertices) {
            uint64_t mortonCode = MortonCode::getVertexMortonCode(vertex, minBounds, maxBounds);
            zSortedVertices.emplace_back(mortonCode, vertex);
        }

        // 根据Morton码进行排序
        std::sort(zSortedVertices.begin(), zSortedVertices.end(), 
            [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

        // 优化后，可以清空原始的顶点数组以节省内存
        vertices.clear();
        vertices.shrink_to_fit();
        
        // std::cout << "Optimized a leaf node with " << zSortedVertices.size() << " vertices using Z-order curve." << std::endl;
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
        // 如果是经过Z阶优化的叶子节点
        if (isZSorted) {
            // --- "中心扩散搜索"查询路径 (已修正bug) ---
            const float radiusSq = radius * radius;

            // 1. 计算查询中心点的Morton码
            uint64_t centerCode = MortonCode::getMortonCodeFromCoord(
                {center.x, 0.0f, center.y}, // 直接使用坐标
                minBounds, maxBounds
            );

            // 2. 使用二分查找快速定位到中心点在排序数组中的位置
            auto it_center = std::lower_bound(zSortedVertices.begin(), zSortedVertices.end(), centerCode,
                [](const std::pair<uint64_t, Vertex*>& element, uint64_t value) {
                    return element.first < value;
                });

            // 3. 从中心点向前（向大）搜索
            for (auto it = it_center; it != zSortedVertices.end(); ++it) {
                Vertex* vertex = it->second;
                float dx = vertex->Position.x - center.x;
                float dz = vertex->Position.z - center.y;
                if ((dx * dx + dz * dz) <= radiusSq) {
                    resultVertices.push_back(vertex);
                } else {
                    // 启发式剪枝：如果一个点在X轴上的距离已经超过了查询半径，
                    // 那么后续的点在圆内的可能性就很小了（因为Z曲线优先填充X方向）。
#if ENABLE_Z_ORDER_QUERY_HEURISTIC_PRUNING
                    if (std::abs(dx) > radius) {
                        break;
                    }
#endif
                }
            }
            
            // 4. 从中心点向后（向小）搜索，使用反向迭代器以保证安全和代码简洁性
            for (auto it = std::reverse_iterator<decltype(it_center)>(it_center); it != zSortedVertices.rend(); ++it) {
                Vertex* vertex = it->second; // it->second 是 Vertex*
                float dx = vertex->Position.x - center.x;
                float dz = vertex->Position.z - center.y;
                if ((dx * dx + dz * dz) <= radiusSq) {
                    resultVertices.push_back(vertex);
                } else {
#if ENABLE_Z_ORDER_QUERY_HEURISTIC_PRUNING
                    if (std::abs(dx) > radius) {
                        break;
                    }
#endif
                }
            }
        } else {
            // --- 原始路径：非优化叶子节点，线性扫描 ---
            float radiusSq = radius * radius;
            for (Vertex* vertex : vertices) {
                float dx = vertex->Position.x - center.x;
                float dz = vertex->Position.z - center.y;
                if ((dx * dx + dz * dz) <= radiusSq) {
                    resultVertices.push_back(vertex);
                }
            }
        }
    } else {
        // 如果是内部节点，则递归查询子节点
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