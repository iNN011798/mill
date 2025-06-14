#include "quadtree_node.h"
#include "quadtree.h" // ��Ҫ���� Quadtree �� maxLevels �� maxVerticesPerNode
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
    return children[0] == nullptr; // �����һ���ӽڵ�Ϊ�գ�����Ϊ��Ҷ�ڵ�
}

void QuadtreeNode::subdivide() {
    if (level >= tree->maxLevels) return; //�ﵽ���㼶�����ٷ���

    glm::vec2 halfSize = (maxBounds - minBounds) / 2.0f;
    glm::vec2 center = minBounds + halfSize;

    // �ӽڵ�˳��: NW, NE, SW, SE
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

    // ����ǰ�ڵ�Ķ�����䵽�ӽڵ�
    std::vector<Vertex*> tempVertices = vertices;
    //std::cout << "now the node's num of vertices is " << tempVertices.size() << endl;
    vertices.clear();

    for (Vertex* vertex : tempVertices) {
        insert(vertex); // ���²��뵽��ǰ�ڵ㣬�����Զ����䵽�ӽڵ�
    }
}

void QuadtreeNode::insert(Vertex* vertex) {
    if (!containsPoint(vertex->Position)) {
        return; // ���㲻�ڴ˽ڵ�߽���
    }

    if (!isLeaf()) {
        int index = getChildIndex(vertex->Position);
        if (index != -1) {
            children[index]->insert(vertex);
            return;
        }
    } 
    // �����Ҷ�ӽڵ㣬�����޷�ȷ���ӽڵ㣨�߽������������ӵ���ǰ�ڵ�
    vertices.push_back(vertex);

    // �����ǰ��Ҷ�ӽڵ㣬�Ҷ��������������ƣ�����δ�ﵽ���㼶�������
    if (isLeaf() && vertices.size() > (size_t)tree->maxVerticesPerNode && level < tree->maxLevels) {
        subdivide();
    }
}

void QuadtreeNode::optimize() {
    // ������ڲ��ڵ㣬��ݹ��Ż��ӽڵ�
    if (!isLeaf()) {
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                children[i]->optimize();
            }
        }
        return;
    }

    // �����Ҷ�ӽڵ㣬���Ҷ�����������һ����ֵ�������Z������
    // ����ֻ��"ӷ��"��Ҷ�ӽڵ�����Ż������ⲻ��Ҫ�ļ���
    const size_t Z_SORT_THRESHOLD = 50; 
    if (vertices.size() > Z_SORT_THRESHOLD) {
        isZSorted = true;
        
        // Ԥ���ռ������Ч��
        zSortedVertices.reserve(vertices.size());

        // ����ÿ�������Morton��
        for (Vertex* vertex : vertices) {
            uint64_t mortonCode = MortonCode::getVertexMortonCode(vertex, minBounds, maxBounds);
            zSortedVertices.emplace_back(mortonCode, vertex);
        }

        // ����Morton���������
        std::sort(zSortedVertices.begin(), zSortedVertices.end(), 
            [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

        // �Ż��󣬿������ԭʼ�Ķ��������Խ�ʡ�ڴ�
        vertices.clear();
        vertices.shrink_to_fit();
        
        // std::cout << "Optimized a leaf node with " << zSortedVertices.size() << " vertices using Z-order curve." << std::endl;
    }
}

// �����Ƿ��ڽڵ��XZ�߽���
bool QuadtreeNode::containsPoint(const glm::vec3& pointPosition) const {
    return (pointPosition.x >= minBounds.x && pointPosition.x <= maxBounds.x &&
            pointPosition.z >= minBounds.y && pointPosition.z <= maxBounds.y);
            // ע�⣺minBounds.y �� maxBounds.y ��Ӧ Z ����
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

// ���Բ�β�ѯ�����Ƿ���˽ڵ�ı߽���ཻ
bool QuadtreeNode::intersectsCircle(const glm::vec2& center, float radius) const {
    // Բ�ĵ����α߽������ľ����ƽ��
    float closestX = (std::max)(minBounds.x, (std::min)(center.x, maxBounds.x));
    float closestZ = (std::max)(minBounds.y, (std::min)(center.y, maxBounds.y)); // minBounds.y is Z

    float distanceX = center.x - closestX;
    float distanceZ = center.y - closestZ;

    return (distanceX * distanceX + distanceZ * distanceZ) <= (radius * radius);
}

void QuadtreeNode::queryRange(const glm::vec2& center, float radius, std::vector<Vertex*>& resultVertices) const {
    if (!intersectsCircle(center, radius)) {
        return; // �˽ڵ����ѯ��Χ���ཻ
    }

    if (isLeaf()) {
        // ����Ǿ���Z���Ż���Ҷ�ӽڵ�
        if (isZSorted) {
            // --- "������ɢ����"��ѯ·�� (������bug) ---
            const float radiusSq = radius * radius;

            // 1. �����ѯ���ĵ��Morton��
            uint64_t centerCode = MortonCode::getMortonCodeFromCoord(
                {center.x, 0.0f, center.y}, // ֱ��ʹ������
                minBounds, maxBounds
            );

            // 2. ʹ�ö��ֲ��ҿ��ٶ�λ�����ĵ������������е�λ��
            auto it_center = std::lower_bound(zSortedVertices.begin(), zSortedVertices.end(), centerCode,
                [](const std::pair<uint64_t, Vertex*>& element, uint64_t value) {
                    return element.first < value;
                });

            // 3. �����ĵ���ǰ���������
            for (auto it = it_center; it != zSortedVertices.end(); ++it) {
                Vertex* vertex = it->second;
                float dx = vertex->Position.x - center.x;
                float dz = vertex->Position.z - center.y;
                if ((dx * dx + dz * dz) <= radiusSq) {
                    resultVertices.push_back(vertex);
                } else {
                    // ����ʽ��֦�����һ������X���ϵľ����Ѿ������˲�ѯ�뾶��
                    // ��ô�����ĵ���Բ�ڵĿ����Ծͺ�С�ˣ���ΪZ�����������X���򣩡�
#if ENABLE_Z_ORDER_QUERY_HEURISTIC_PRUNING
                    if (std::abs(dx) > radius) {
                        break;
                    }
#endif
                }
            }
            
            // 4. �����ĵ������С��������ʹ�÷���������Ա�֤��ȫ�ʹ�������
            for (auto it = std::reverse_iterator<decltype(it_center)>(it_center); it != zSortedVertices.rend(); ++it) {
                Vertex* vertex = it->second; // it->second �� Vertex*
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
            // --- ԭʼ·�������Ż�Ҷ�ӽڵ㣬����ɨ�� ---
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
        // ������ڲ��ڵ㣬��ݹ��ѯ�ӽڵ�
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                children[i]->queryRange(center, radius, resultVertices);
            }
        }
    }
}

void QuadtreeNode::printVertices(int indentLevel) const {
    std::string indent(indentLevel * 2, ' '); // ���������ַ���

    std::cout << indent << "Node Level: " << level 
              << ", Bounds: [(" << minBounds.x << ", " << minBounds.y << ") to (" 
              << maxBounds.x << ", " << maxBounds.y << ")]"
              << ", Vertices directly in this node: " << vertices.size(); // ֻ��ʾ���ڵ�ֱ�Ӵ洢�Ķ�����

    if (isLeaf()) {
        std::cout << " (Leaf Node)" << std::endl;
        // ���ٴ�ӡҶ�ӽڵ����ϸ��������
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
        // std::cout << indent << "  Children:" << std::endl; // ����ʡ�����У�ֱ���г��ӽڵ�
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                // std::cout << indent << "  Child [" << i << "] :" << std::endl; // Ҳ����ʡ�ԣ����ӽڵ������Դ�����
                children[i]->printVertices(indentLevel + 1); // �����������Ϊ+1��ʹ�ṹ������
            } else {
                 // �����ڲ��ڵ㣬ͨ���ڷ��Ѻ������� vertices ����գ�����δ���ѻ򶥵��޷�����
                 // �����Ҫ��ʾ�յ��ӽڵ�ռλ��������ȡ��ע����һ��
                 // std::cout << indent << std::string((indentLevel + 1) * 2, ' ') << "Child [" << i << "] : nullptr" << std::endl;
            }
        }
    }
} 