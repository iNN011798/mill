#pragma once

// --- 刀具配置 ---
// 设置为 1 采用球头刀， 设置为 0 使用平底刀
#if 1
#define Type ball
#elif
#define Type flat
#endif

// --- 路径配置 ---
// 设置为 1 使用螺旋线路径, 设置为 0 使用Z字折线路径
#define USE_SPIRAL_PATH 0

// --- 性能优化配置 ---
// 设置为 1 启用四叉树空间分区, 设置为 0 使用暴力遍历
#define ENABLE_QUADTREE_OPTIMIZATION 1

// 设置为 1 在构建四叉树后启用Z阶曲线优化
#define ENABLE_Z_ORDER_OPTIMIZATION 1

// 设置为 1 在Z阶曲线优化的查询中启用启发式剪枝
#define ENABLE_Z_ORDER_QUERY_HEURISTIC_PRUNING 0

// --- 调试配置 ---
// 设置为 1 在构建四叉树后打印其内容以供调试
#define ENABLE_QUADTREE_DEBUG_PRINT 1 