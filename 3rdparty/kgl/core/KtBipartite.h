#pragma once
#include <assert.h>
#include "KtDfsIter.h"


// ����ͼ�ж�
// ������true����ʾTHIS�Ƕ���ͼ��resԪ��ֵΪ1����2���ֱ�����Ӧ�ڵ��ڲ�ͬ��ͼ
// ������false����ʾTHIS�Ƕ���ͼ��resֵ����-1��Ԫ��Ϊ��ͻ����
template<typename GRAPH>
class KtBipartite
{
    static_assert(!GRAPH::isDigraph(), "KtBipartite cannot instantiated with Digraph.");

public:

    // ��ȡ2-��ɫ��ʵ�֣�ͨ��DFS�㷨�����϶��ڽڵ���з���ɫ�����޳�ͻ���ж��ɹ���
    KtBipartite(const GRAPH& g) : color_(g.order(), 0), bipartite_(true) {

        int color = 1;
        KtDfsIter<const GRAPH, true, true> iter(g, 0);
        color_[0] = color;

        for (; !iter.isEnd(); ++iter) {
            unsigned from = iter.from();
            assert(from != -1 && color_[from] > 0);
            color = 3 - color_[from]; // flip 1 & 2�����븸�ڵ㲻ͬ����ɫ

            unsigned v = *iter;
            if (color_[v] == 3 - color) { // ��ɫ��ͻ
                color_[v] = -1;
                bipartite_ = false;
                break;
            }

            color_[v] = color; // ��ɫ
        }
    }


    // �Ƿ����ͼ
    bool ok() const { return bipartite_; }


    // ���ض���v����ɫ��ֵΪ1����2���ֱ�����Ӧ�ڵ��ڲ�ͬ��ͼ��-1�����ͻ��
    int color(unsigned v) const { return color_[v]; }


private:
    std::vector<int> color_;
    bool bipartite_; 
};

