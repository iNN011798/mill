#ifndef MORTON_CODE_H
#define MORTON_CODE_H

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

// Z阶/Morton码相关工具函数
namespace MortonCode {

    // -------------------------------------------------------------------------------------
    // Morton编码核心函数
    // -------------------------------------------------------------------------------------
    // 该函数接受一个32位无符号整数，并在其二进制表示的每两位之间插入一个0。
    // 例如:
    // 输入: 1111_1111_1111_1111_1111_1111_1111_1111 (uint32_t)
    // 输出: 0101...01_01010101_01010101_01010101 (uint64_t)
    // 这是将一个维度（例如X轴）的坐标扩展到64位，为另一维度的坐标（例如Z轴）腾出空间的关键步骤。
    // 该实现使用了高效的位操作"magic bits"方法，比循环快得多。
    inline uint64_t part1by1(uint32_t n) {
        uint64_t x = n;
        x = (x | (x << 16)) & 0x0000FFFF0000FFFF;
        x = (x | (x << 8)) & 0x00FF00FF00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F0F0F0F0F;
        x = (x | (x << 2)) & 0x3333333333333333;
        x = (x | (x << 1)) & 0x5555555555555555;
        return x;
    }

    // 将两个32位整数（代表二维坐标）交错编码为一个64位Morton码。
    // X坐标的位将占据偶数位，Z坐标的位将占据奇数位。
    inline uint64_t encode(uint32_t x, uint32_t z) {
        return (part1by1(x) << 1) | part1by1(z);
    }
    
    // -------------------------------------------------------------------------------------
    // 坐标转换与编码的便捷函数
    // -------------------------------------------------------------------------------------
    
    // 定义Morton码使用的整数坐标系的最大分辨率。
    // 2^16 = 65536，意味着我们将叶子节点的包围盒划分为 65536 x 65536 的网格。
    // 这个精度对于单精度浮点数来说已经足够了。
    // 我们选择16位整数（0-65535），因为两个32位编码后会得到64位Morton码，正好。
    constexpr uint32_t MORTON_RESOLUTION = (1 << 16) - 1;

    // 将叶子节点内的局部浮点坐标转换为用于生成Morton码的离散整数坐标。
    // vertexPos: 顶点的世界坐标或模型坐标
    // nodeMinBounds: 所在叶子节点的XZ最小边界
    // nodeMaxBounds: 所在叶子节点的XZ最大边界
    inline glm::uvec2 quantizePosition(const glm::vec3& vertexPos, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        // 计算包围盒的尺寸
        glm::vec2 extent = nodeMaxBounds - nodeMinBounds;
        
        // 防止除以零
        if (extent.x < 1e-6) extent.x = 1.0f;
        if (extent.y < 1e-6) extent.y = 1.0f;

        // 将顶点位置归一化到 [0, 1] 范围
        float normalizedX = (vertexPos.x - nodeMinBounds.x) / extent.x;
        float normalizedZ = (vertexPos.z - nodeMinBounds.y) / extent.y; // Z 对应 bounds.y

        // 将归一化的浮点数映射到整数分辨率上
        uint32_t ix = static_cast<uint32_t>(glm::clamp(normalizedX, 0.0f, 1.0f) * MORTON_RESOLUTION);
        uint32_t iz = static_cast<uint32_t>(glm::clamp(normalizedZ, 0.0f, 1.0f) * MORTON_RESOLUTION);

        return glm::uvec2(ix, iz);
    }

    // 一个便捷函数，直接接收顶点和节点信息，返回最终的Morton码
    inline uint64_t getVertexMortonCode(const Vertex* vertex, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        if (!vertex) return 0;
        glm::uvec2 intCoords = quantizePosition(vertex->Position, nodeMinBounds, nodeMaxBounds);
        return encode(intCoords.x, intCoords.y); // .y is the integer Z coordinate
    }

    // 新增重载：直接从坐标计算Morton码，避免创建临时Vertex对象
    inline uint64_t getMortonCodeFromCoord(const glm::vec3& position, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        glm::uvec2 intCoords = quantizePosition(position, nodeMinBounds, nodeMaxBounds);
        return encode(intCoords.x, intCoords.y);
    }

    // -------------------------------------------------------------------------------------
    // Morton码范围查询核心函数
    // -------------------------------------------------------------------------------------

    // 递归地查找覆盖2D查询矩形所需的1D Morton码范围。
    // 'code': 当前搜索象限左下角的Morton码。
    // 'level': 当前象限的级别（从16开始，代表整个空间，到0结束，代表单个单元格）。
    // 'q_min', 'q_max': 查询矩形的整数坐标范围。
    // 's_min', 's_max': 当前搜索象限的整数坐标范围。
    // 'ranges': 用于存储结果范围的向量。
    inline void getMortonRanges(
        uint64_t code, int level,
        const glm::uvec2& q_min, const glm::uvec2& q_max,
        const glm::uvec2& s_min, const glm::uvec2& s_max,
        std::vector<std::pair<uint64_t, uint64_t>>& ranges)
    {
        // 1. 如果搜索象限完全包含在查询矩形内，则其Morton码范围就是我们需要的结果。
        if (s_min.x >= q_min.x && s_max.x <= q_max.x && s_min.y >= q_min.y && s_max.y <= q_max.y) {
            uint64_t size = 1ULL << (2 * level); // 此级别象限包含的码点总数
            ranges.emplace_back(code, code + size - 1);
            return;
        }

        // 2. 如果搜索象限完全在查询矩形之外，则剪枝，无需继续。
        if (s_max.x < q_min.x || s_min.x > q_max.x || s_max.y < q_min.y || s_min.y > q_max.y) {
            return;
        }

        // 3. 如果部分相交，并且还能再分，则递归到四个子象限。
        if (level > 0) {
            int next_level = level - 1;
            uint64_t quadrant_size = 1ULL << (2 * next_level); // 每个子象限的码点数
            uint32_t half_dim = 1U << next_level; // 子象限的边长

            // 按照Z曲线的顺序递归 (SW -> NW -> SE -> NE)
            getMortonRanges(code,                       next_level, q_min, q_max, s_min,                                         {s_min.x + half_dim - 1, s_min.y + half_dim - 1}, ranges);
            getMortonRanges(code + quadrant_size,       next_level, q_min, q_max, {s_min.x, s_min.y + half_dim},                 {s_min.x + half_dim - 1, s_max.y},                ranges);
            getMortonRanges(code + 2 * quadrant_size,   next_level, q_min, q_max, {s_min.x + half_dim, s_min.y},                 {s_max.x, s_min.y + half_dim - 1},                ranges);
            getMortonRanges(code + 3 * quadrant_size,   next_level, q_min, q_max, {s_min.x + half_dim, s_min.y + half_dim},      s_max,                                            ranges);
        }
    }

} // namespace MortonCode

#endif // MORTON_CODE_H 