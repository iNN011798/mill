#pragma once

// --- �������� ---
// ����Ϊ 1 ������ͷ���� ����Ϊ 0 ʹ��ƽ�׵�
#if 1
#define Type ball
#elif
#define Type flat
#endif

// --- ·������ ---
// ����Ϊ 1 ʹ��������·��, ����Ϊ 0 ʹ��Z������·��
#define USE_SPIRAL_PATH 0

// --- �����Ż����� ---
// ����Ϊ 1 �����Ĳ����ռ����, ����Ϊ 0 ʹ�ñ�������
#define ENABLE_QUADTREE_OPTIMIZATION 1

// ����Ϊ 1 �ڹ����Ĳ���������Z�������Ż�
#define ENABLE_Z_ORDER_OPTIMIZATION 1

// ����Ϊ 1 ��Z�������Ż��Ĳ�ѯ����������ʽ��֦
#define ENABLE_Z_ORDER_QUERY_HEURISTIC_PRUNING 0

// --- �������� ---
// ����Ϊ 1 �ڹ����Ĳ������ӡ�������Թ�����
#define ENABLE_QUADTREE_DEBUG_PRINT 1 