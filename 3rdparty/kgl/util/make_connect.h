#pragma once
#include <vector>
#include <assert.h>
#include "../core/KtConnected.h"
#include "is_connected.h"


// ��ͼg�Ļ��������ӱߣ��Ա�֤gΪ��ͨͼ
// �������ӵı�
template<typename GRAPH>
auto make_connect(GRAPH& g)
{
    KtConnected<GRAPH> cc(g);
    std::vector<std::pair<unsigned, unsigned>> edges;
    if (cc.count() > 1) {
        std::vector<unsigned> v(cc.count(), -1);
        unsigned cnt(0);
        for (unsigned i = 0; i < g.order() && cnt < cc.count(); i++) {
            if (v[cc[i]] == -1) {
                v[cc[i]] = i; // ѹ��ÿ����ͨ�����ĵ�1������
                ++cnt;
            }
        }

        for (unsigned i = 1; i < v.size(); i++) {
            assert(!g.hasEdge(v[i - 1], v[i]));
            g.addEdge(v[i - 1], v[i]);
            edges.push_back({ v[i - 1], v[i] });
        }

        assert(is_connected(g));
    }

    return edges;
}

