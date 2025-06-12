#ifndef MORTON_CODE_H
#define MORTON_CODE_H

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

// Z��/Morton����ع��ߺ���
namespace MortonCode {

    // -------------------------------------------------------------------------------------
    // Morton������ĺ���
    // -------------------------------------------------------------------------------------
    // �ú�������һ��32λ�޷�������������������Ʊ�ʾ��ÿ��λ֮�����һ��0��
    // ����:
    // ����: 1111_1111_1111_1111_1111_1111_1111_1111 (uint32_t)
    // ���: 0101...01_01010101_01010101_01010101 (uint64_t)
    // ���ǽ�һ��ά�ȣ�����X�ᣩ��������չ��64λ��Ϊ��һά�ȵ����꣨����Z�ᣩ�ڳ��ռ�Ĺؼ����衣
    // ��ʵ��ʹ���˸�Ч��λ����"magic bits"��������ѭ����öࡣ
    inline uint64_t part1by1(uint32_t n) {
        uint64_t x = n;
        x = (x | (x << 16)) & 0x0000FFFF0000FFFF;
        x = (x | (x << 8)) & 0x00FF00FF00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F0F0F0F0F;
        x = (x | (x << 2)) & 0x3333333333333333;
        x = (x | (x << 1)) & 0x5555555555555555;
        return x;
    }

    // ������32λ�����������ά���꣩�������Ϊһ��64λMorton�롣
    // X�����λ��ռ��ż��λ��Z�����λ��ռ������λ��
    inline uint64_t encode(uint32_t x, uint32_t z) {
        return (part1by1(x) << 1) | part1by1(z);
    }
    
    // -------------------------------------------------------------------------------------
    // ����ת�������ı�ݺ���
    // -------------------------------------------------------------------------------------
    
    // ����Morton��ʹ�õ���������ϵ�����ֱ��ʡ�
    // 2^16 = 65536����ζ�����ǽ�Ҷ�ӽڵ�İ�Χ�л���Ϊ 65536 x 65536 ������
    // ������ȶ��ڵ����ȸ�������˵�Ѿ��㹻�ˡ�
    // ����ѡ��16λ������0-65535������Ϊ����32λ������õ�64λMorton�룬���á�
    constexpr uint32_t MORTON_RESOLUTION = (1 << 16) - 1;

    // ��Ҷ�ӽڵ��ڵľֲ���������ת��Ϊ��������Morton�����ɢ�������ꡣ
    // vertexPos: ��������������ģ������
    // nodeMinBounds: ����Ҷ�ӽڵ��XZ��С�߽�
    // nodeMaxBounds: ����Ҷ�ӽڵ��XZ���߽�
    inline glm::uvec2 quantizePosition(const glm::vec3& vertexPos, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        // �����Χ�еĳߴ�
        glm::vec2 extent = nodeMaxBounds - nodeMinBounds;
        
        // ��ֹ������
        if (extent.x < 1e-6) extent.x = 1.0f;
        if (extent.y < 1e-6) extent.y = 1.0f;

        // ������λ�ù�һ���� [0, 1] ��Χ
        float normalizedX = (vertexPos.x - nodeMinBounds.x) / extent.x;
        float normalizedZ = (vertexPos.z - nodeMinBounds.y) / extent.y; // Z ��Ӧ bounds.y

        // ����һ���ĸ�����ӳ�䵽�����ֱ�����
        uint32_t ix = static_cast<uint32_t>(glm::clamp(normalizedX, 0.0f, 1.0f) * MORTON_RESOLUTION);
        uint32_t iz = static_cast<uint32_t>(glm::clamp(normalizedZ, 0.0f, 1.0f) * MORTON_RESOLUTION);

        return glm::uvec2(ix, iz);
    }

    // һ����ݺ�����ֱ�ӽ��ն���ͽڵ���Ϣ���������յ�Morton��
    inline uint64_t getVertexMortonCode(const Vertex* vertex, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        if (!vertex) return 0;
        glm::uvec2 intCoords = quantizePosition(vertex->Position, nodeMinBounds, nodeMaxBounds);
        return encode(intCoords.x, intCoords.y); // .y is the integer Z coordinate
    }

    // �������أ�ֱ�Ӵ��������Morton�룬���ⴴ����ʱVertex����
    inline uint64_t getMortonCodeFromCoord(const glm::vec3& position, const glm::vec2& nodeMinBounds, const glm::vec2& nodeMaxBounds) {
        glm::uvec2 intCoords = quantizePosition(position, nodeMinBounds, nodeMaxBounds);
        return encode(intCoords.x, intCoords.y);
    }

    // -------------------------------------------------------------------------------------
    // Morton�뷶Χ��ѯ���ĺ���
    // -------------------------------------------------------------------------------------

    // �ݹ�ز��Ҹ���2D��ѯ���������1D Morton�뷶Χ��
    // 'code': ��ǰ�����������½ǵ�Morton�롣
    // 'level': ��ǰ���޵ļ��𣨴�16��ʼ�����������ռ䣬��0��������������Ԫ�񣩡�
    // 'q_min', 'q_max': ��ѯ���ε��������귶Χ��
    // 's_min', 's_max': ��ǰ�������޵��������귶Χ��
    // 'ranges': ���ڴ洢�����Χ��������
    inline void getMortonRanges(
        uint64_t code, int level,
        const glm::uvec2& q_min, const glm::uvec2& q_max,
        const glm::uvec2& s_min, const glm::uvec2& s_max,
        std::vector<std::pair<uint64_t, uint64_t>>& ranges)
    {
        // 1. �������������ȫ�����ڲ�ѯ�����ڣ�����Morton�뷶Χ����������Ҫ�Ľ����
        if (s_min.x >= q_min.x && s_max.x <= q_max.x && s_min.y >= q_min.y && s_max.y <= q_max.y) {
            uint64_t size = 1ULL << (2 * level); // �˼������ް������������
            ranges.emplace_back(code, code + size - 1);
            return;
        }

        // 2. �������������ȫ�ڲ�ѯ����֮�⣬���֦�����������
        if (s_max.x < q_min.x || s_min.x > q_max.x || s_max.y < q_min.y || s_min.y > q_max.y) {
            return;
        }

        // 3. ��������ཻ�����һ����ٷ֣���ݹ鵽�ĸ������ޡ�
        if (level > 0) {
            int next_level = level - 1;
            uint64_t quadrant_size = 1ULL << (2 * next_level); // ÿ�������޵������
            uint32_t half_dim = 1U << next_level; // �����޵ı߳�

            // ����Z���ߵ�˳��ݹ� (SW -> NW -> SE -> NE)
            getMortonRanges(code,                       next_level, q_min, q_max, s_min,                                         {s_min.x + half_dim - 1, s_min.y + half_dim - 1}, ranges);
            getMortonRanges(code + quadrant_size,       next_level, q_min, q_max, {s_min.x, s_min.y + half_dim},                 {s_min.x + half_dim - 1, s_max.y},                ranges);
            getMortonRanges(code + 2 * quadrant_size,   next_level, q_min, q_max, {s_min.x + half_dim, s_min.y},                 {s_max.x, s_min.y + half_dim - 1},                ranges);
            getMortonRanges(code + 3 * quadrant_size,   next_level, q_min, q_max, {s_min.x + half_dim, s_min.y + half_dim},      s_max,                                            ranges);
        }
    }

} // namespace MortonCode

#endif // MORTON_CODE_H 