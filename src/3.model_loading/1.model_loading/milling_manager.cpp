#include "milling_manager.h"
#include "quadtree.h" // Quadtree 定义
#include <glm/gtc/matrix_transform.hpp> 
#include <iostream>
#include <vector>
#include <limits> // For std::numeric_limits
#include <cmath>  // For std::abs and std::sqrt

MillingManager::MillingManager(float toolRadius, float toolTipLocalYOffset, float cubeMinLocalY, ToolType toolType)
    : toolRadius_(toolRadius),
      toolTipLocalYOffset_(toolTipLocalYOffset),
      cubeMinLocalY_(cubeMinLocalY),
      toolheadType_(toolType),
      quadtree_(nullptr) {}

MillingManager::~MillingManager() {
    // std::unique_ptr will automatically handle deletion of the Quadtree object
}

void MillingManager::initializeSpatialPartition(Model& cubeModel, 
                                                float surfaceYValue, 
                                                float surfaceYThreshold, 
                                                int quadtreeMaxLevels, 
                                                int quadtreeMaxVertsPerNode) {
    quadtree_.reset(); // Clear any existing quadtree

    std::vector<Vertex*> surfaceVertices;
    glm::vec2 minXZ(std::numeric_limits<float>::max());
    glm::vec2 maxXZ(std::numeric_limits<float>::lowest());
    bool foundSurfaceVertices = false;

    for (Mesh& mesh : cubeModel.meshes) {
        for (Vertex& vertex : mesh.vertices) {
            if (std::abs(vertex.Position.y - surfaceYValue) < surfaceYThreshold) {
                surfaceVertices.push_back(&vertex);
                minXZ.x = std::min(minXZ.x, vertex.Position.x);
                minXZ.y = std::min(minXZ.y, vertex.Position.z); // Using .y for Z here for glm::vec2
                maxXZ.x = std::max(maxXZ.x, vertex.Position.x);
                maxXZ.y = std::max(maxXZ.y, vertex.Position.z); // Using .y for Z here for glm::vec2
                foundSurfaceVertices = true;
            }
        }
    }

    if (!foundSurfaceVertices || surfaceVertices.empty()) {
        std::cout << "MillingManager: No surface vertices found to build Quadtree." << std::endl;
        return;
    }
    // Ensure bounds are valid (min < max)
    if (minXZ.x >= maxXZ.x || minXZ.y >= maxXZ.y) {
         // Handle degenerate case, e.g. all points on a line or single point.
         // For simplicity, create a small default area around the points or log an error.
         std::cout << "MillingManager: Degenerate bounds for Quadtree. Expanding slightly." << std::endl;
         // Provide a small default size if bounds are too small, e.g., if maxXZ is not greater than minXZ.
         // This can happen if all surface vertices are collinear or coincident.
         if (minXZ.x >= maxXZ.x) maxXZ.x = minXZ.x + 0.1f; // Add a small epsilon
         if (minXZ.y >= maxXZ.y) maxXZ.y = minXZ.y + 0.1f; // Add a small epsilon
    }


    quadtree_ = std::make_unique<Quadtree>(minXZ, maxXZ, quadtreeMaxLevels, quadtreeMaxVertsPerNode);
    std::cout << "MillingManager: Building Quadtree with bounds: (" 
              << minXZ.x << ", " << minXZ.y << ") to (" 
              << maxXZ.x << ", " << maxXZ.y << ") for " 
              << surfaceVertices.size() << " vertices." << std::endl;

    for (Vertex* vertex : surfaceVertices) {
        quadtree_->insert(vertex);
    }
    std::cout << "MillingManager: Quadtree built." << std::endl;

#if 0
    // 新增：打印四叉树内容以供调试
    if (quadtree_) {
        std::cout << "MillingManager: Attempting to print Quadtree contents..." << std::endl;
        quadtree_->printTreeContents();
    } else {
        std::cout << "MillingManager: Quadtree is null, cannot print contents." << std::endl;
    }
#endif
}

bool MillingManager::processMilling(Model& cubeModel,
                                    const glm::vec3& cubeWorldPosition,
                                    const glm::vec3& toolBaseWorldPosition,
                                    bool isMillingEnabled) {
    if (!isMillingEnabled) {
        return false;
    }

    glm::mat4 model_cube_matrix = glm::translate(glm::mat4(1.0f), cubeWorldPosition);
    glm::mat4 world_to_cube_local_matrix = glm::inverse(model_cube_matrix);

    glm::vec3 tool_tip_effective_world_position = toolBaseWorldPosition;
    tool_tip_effective_world_position.y += toolTipLocalYOffset_;
    glm::vec4 tool_tip_world_homogeneous = glm::vec4(tool_tip_effective_world_position, 1.0f);
    glm::vec4 tool_tip_cube_local_homogeneous = world_to_cube_local_matrix * tool_tip_world_homogeneous;
    glm::vec3 tool_tip_cube_local = glm::vec3(tool_tip_cube_local_homogeneous / tool_tip_cube_local_homogeneous.w);

    bool vertices_modified = false;

    if (quadtree_) {
        //std::cout << "use quadTree!" << std::endl;
        // Optimized path using Quadtree
        std::vector<Vertex*> candidateVertices = quadtree_->queryRange(glm::vec2(tool_tip_cube_local.x, tool_tip_cube_local.z), toolRadius_);
        //std::cout << "Queried vertices: " << candidateVertices.size() << std::endl;

        for (Vertex* current_vertex_ptr : candidateVertices) {
            Vertex& current_vertex = *current_vertex_ptr; // Dereference pointer
            
            // XZ distance check might be redundant if queryRange is accurate enough, but it's safer.
            // The queryRange in QuadtreeNode already does a precise circle check for leaf nodes.
            // float dx = current_vertex.Position.x - tool_tip_cube_local.x;
            // float dz = current_vertex.Position.z - tool_tip_cube_local.z;
            // float dist_xz_squared = dx * dx + dz * dz;
            // if (dist_xz_squared < (toolRadius_ * toolRadius_)) { // This check is effectively done by queryRange

            float target_y_cut = glm::max(tool_tip_cube_local.y, cubeMinLocalY_);
            if (current_vertex.Position.y > target_y_cut) {
                float old_y = current_vertex.Position.y;
                switch (toolheadType_) {
                    case ToolType::flat:
                        current_vertex.Position.y = target_y_cut;
                        current_vertex.Color = glm::vec3(0.0f, 1.0f, 0.0f); // Set color to red
                        // 对于平面切削，法线直接指向上方 (Y轴正方向)
                        current_vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                        break;
                    case ToolType::ball: {
                        // For ball nose, the effective cutting Y depends on XZ distance from tool center
                        float dx_ball = current_vertex.Position.x - tool_tip_cube_local.x;
                        float dz_ball = current_vertex.Position.z - tool_tip_cube_local.z;
                        float dist_xz_squared_ball = dx_ball * dx_ball + dz_ball * dz_ball;
                        float radius_squared = toolRadius_ * toolRadius_;
                        if (dist_xz_squared_ball < radius_squared) { // Ensure it's within the tool's footprint for ball calculation
                            float y_offset_ball = std::sqrt(radius_squared - dist_xz_squared_ball);
                            float ball_surface_y = tool_tip_cube_local.y + toolRadius_ - y_offset_ball; // Lowest point of tool is tool_tip_cube_local.y
                            float actual_cut_y = glm::max(ball_surface_y, cubeMinLocalY_);                             
                            if (current_vertex.Position.y > actual_cut_y) {
                                current_vertex.Position.y = actual_cut_y;
                                current_vertex.Color = glm::vec3(0.0f, 1.0f, 0.0f); // Set color to red
                                // 对于球面切削，法线是从球心指向顶点位置
                                glm::vec3 sphere_center_local = tool_tip_cube_local + glm::vec3(0.0f, toolRadius_, 0.0f);
                                current_vertex.Normal = glm::normalize(current_vertex.Position - sphere_center_local);
                            }
                        } // else: vertex is outside the direct spherical cut, no change from ball specific part
                        break;
                    }
                    default:
                        break;
                }
                if (std::abs(current_vertex.Position.y - old_y) > 0.00001f) { // Check if Y actually changed
                     vertices_modified = true;
                }
            }
        }

    } else {
        // Fallback to old behavior if Quadtree is not initialized (or keep this as an error/warning)
        std::cerr << "MillingManager: Quadtree not initialized. Falling back to unoptimized milling." << std::endl;
        for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
            Mesh& current_mesh = cubeModel.meshes[i];
            for (unsigned int j = 0; j < current_mesh.vertices.size(); ++j) {
                Vertex& current_vertex = current_mesh.vertices[j];
                float dx = current_vertex.Position.x - tool_tip_cube_local.x;
                float dz = current_vertex.Position.z - tool_tip_cube_local.z;
                float dist_xz_squared = dx * dx + dz * dz;

                if (dist_xz_squared < (toolRadius_ * toolRadius_)) {
                    float target_y_cut = glm::max(tool_tip_cube_local.y, cubeMinLocalY_);
                    if (current_vertex.Position.y > target_y_cut) {
                         float old_y = current_vertex.Position.y;
                        switch (toolheadType_) {
                            case ToolType::flat:
                                current_vertex.Position.y = target_y_cut;
                                current_vertex.Color = glm::vec3(0.0f, 1.0f, 0.0f); // Set color to red
                                // 对于平面切削，法线直接指向上方 (Y轴正方向)
                                current_vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                                break;
                            case ToolType::ball: {
                                float radius_squared = toolRadius_ * toolRadius_;
                                float y_offset_ball = std::sqrt(radius_squared - dist_xz_squared); // dist_xz_squared is already calculated
                                float ball_surface_y = tool_tip_cube_local.y + toolRadius_ - y_offset_ball;
                                float actual_cut_y = glm::max(ball_surface_y, cubeMinLocalY_);
                                 if (current_vertex.Position.y > actual_cut_y) {
                                    current_vertex.Position.y = actual_cut_y;
                                    current_vertex.Color = glm::vec3(0.0f, 1.0f, 0.0f); // Set color to red
                                    // 对于球面切削，法线是从球心指向顶点位置
                                    glm::vec3 sphere_center_local = tool_tip_cube_local + glm::vec3(0.0f, toolRadius_, 0.0f);
                                    current_vertex.Normal = glm::normalize(current_vertex.Position - sphere_center_local);
                                }
                                break;
                            }
                            default:
                                break;
                        }
                        if (std::abs(current_vertex.Position.y - old_y) > 0.00001f) {
                            vertices_modified = true;
                        }
                    }
                }
            }
        }
    }

    if (vertices_modified) {
        for (unsigned int i = 0; i < cubeModel.meshes.size(); ++i) {
            cubeModel.meshes[i].updateVertexBuffer();
        }
    }
    return vertices_modified;
} 