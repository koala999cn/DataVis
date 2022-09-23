#pragma once
#include <vector>
#include <assert.h>
#include "../core/KtAdjIter.h"


// ��ͼg�Ķ��㰴��order���������򣬼�g[v]��Ϊg[order[v]]
//   GRAPH - ͼ����
//   ORDER - ��[]������������
template<class GRAPH, class ORDER>
void resort(GRAPH& g, const ORDER& order)
{
    using edge_type = typename GRAPH::edge_type;
    using vertex_index_t = typename GRAPH::vertex_index_t;

    auto E = g.size(); // ����verify
    std::vector<bool> flags(g.order(), false); // ���ڱ�Ƕ���flags[i]�Ƿ���������

    // ��ѯ����������ţ���Ҫ�����ǽ�����v�ĳ��ߵ���Ϊ����order[v]�ĳ���
    for (auto v = 0u; v < g.order(); v++) {
        if (flags[v]) continue;

        // �ռ�����v�ĳ���
        std::vector<std::pair<vertex_index_t, edge_type>> v_outEdges;
        auto iter = KtAdjIter(g, v);
        for (; !iter.isEnd(); ++iter)
            v_outEdges.emplace_back(*iter, iter.edge());


        do {
            flags[v] = true;
            auto nv = order[v]; // ����v������Ϊnv

            // ������v�ĳ��ߵ���Ϊnv�ĳ���֮ǰ���ȱ���nv�ĳ��ߣ�������Ϣ��ʧ
            std::vector<std::pair<vertex_index_t, edge_type>> nv_outEdges;
            if (!flags[nv]) { // ��nv�Ѵ����������Ϣ�����Ź����������ռ�����
                auto iter = KtAdjIter(g, nv);
                for (; !iter.isEnd(); ++iter)
                    nv_outEdges.emplace_back(*iter, iter.edge());
            }

            // ���ߵ�����outEdges(v) -> outEdges(nv)
            g.eraseOutEdges(nv);
            for (const auto& e : v_outEdges)
                g.addEdge(nv, order[e.first], e.second);


            // ˳��v -> order[v] -> order[order[v]] -> ... һֱ��ǰ��
            v = nv;
            std::swap(v_outEdges, nv_outEdges);
        } while (!flags[v]);
    }

    assert(g.size() == E);
}
