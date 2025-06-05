#include "quadtree.h"
#include <iostream> // For std::cout in printTreeContents

Quadtree::Quadtree(glm::vec2 minBounds, glm::vec2 maxBounds, int maxLvl, int maxVertsPerNode)
    : maxLevels(maxLvl), maxVerticesPerNode(maxVertsPerNode) {
    root = new QuadtreeNode(minBounds, maxBounds, 0, this);
}

Quadtree::~Quadtree() {
    clear(); // clearRecursive will delete root and all children
}

void Quadtree::insert(Vertex* vertex) {
    if (root && root->containsPoint(vertex->Position)) { // 确保顶点在根节点范围内
        root->insert(vertex);
    }
    // else: Vertex is outside the bounds of the quadtree, decide how to handle (e.g., ignore, log error)
}

std::vector<Vertex*> Quadtree::queryRange(const glm::vec2& center, float radius) const {
    std::vector<Vertex*> resultVertices;
    if (root) {
        root->queryRange(center, radius, resultVertices);
    }
    return resultVertices;
}

void Quadtree::clear() {
    clearRecursive(root);
    root = nullptr; // Important: set root to null after deleting its contents
}

void Quadtree::clearRecursive(QuadtreeNode* node) {
    if (node == nullptr) {
        return;
    }
    for (int i = 0; i < 4; ++i) {
        if (node->children[i] != nullptr) {
            clearRecursive(node->children[i]);
            node->children[i] = nullptr; // Though ~QuadtreeNode should handle deletion
        }
    }
    delete node; // This will call ~QuadtreeNode, which deletes its children again if not null
                 // It's generally safer to delete children explicitly before deleting the parent in a recursive clear
                 // or ensure the destructor of QuadtreeNode only deletes children if they haven't been cleared.
                 // Current ~QuadtreeNode deletes its children, so this is okay, but can be redundant if children were already cleared.
                 // A better pattern for clearRecursive might be:
                 // if (!node) return;
                 // for (int i=0; i<4; ++i) clearRecursive(node->children[i]);
                 // delete node; (and in ~QuadtreeNode, set children to null after delete but don't re-delete)
                 // For simplicity, current approach is mostly fine. The ~QuadtreeNode will handle freeing children.
}

void Quadtree::printTreeContents() const {
    if (root) {
        std::cout << "\n--- Quadtree Contents Start ---" << std::endl;
        root->printVertices(0);
        std::cout << "--- Quadtree Contents End ---\n" << std::endl;
    } else {
        std::cout << "Quadtree is empty (root is nullptr)." << std::endl;
    }
} 