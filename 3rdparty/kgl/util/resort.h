#pragma once
#include <vector>
#include <assert.h>
#include "../core/KtAdjIter.h"


// 对图g的顶点按照order进行重排序，即g[v]变为g[order[v]]
//   GRAPH - 图类型
//   ORDER - 具[]操作符的类型
template<class GRAPH, class ORDER>
void resort(GRAPH& g, const ORDER& order)
{
    using edge_type = typename GRAPH::edge_type;
    using vertex_index_t = typename GRAPH::vertex_index_t;

    auto E = g.size(); // 用于verify
    std::vector<bool> flags(g.order(), false); // 用于标记顶点flags[i]是否已重排序

    // 轮询顶点进行重排，主要操作是将顶点v的出边调整为顶点order[v]的出边
    for (auto v = 0u; v < g.order(); v++) {
        if (flags[v]) continue;

        // 收集顶点v的出边
        std::vector<std::pair<vertex_index_t, edge_type>> v_outEdges;
        auto iter = KtAdjIter(g, v);
        for (; !iter.isEnd(); ++iter)
            v_outEdges.emplace_back(*iter, iter.edge());


        do {
            flags[v] = true;
            auto nv = order[v]; // 顶点v将重排为nv

            // 将顶点v的出边调整为nv的出边之前，先保存nv的出边，以免信息丢失
            std::vector<std::pair<vertex_index_t, edge_type>> nv_outEdges;
            if (!flags[nv]) { // 若nv已处理，则出边信息已重排过，不必再收集保留
                auto iter = KtAdjIter(g, nv);
                for (; !iter.isEnd(); ++iter)
                    nv_outEdges.emplace_back(*iter, iter.edge());
            }

            // 出边调整：outEdges(v) -> outEdges(nv)
            g.eraseOutEdges(nv);
            for (const auto& e : v_outEdges)
                g.addEdge(nv, order[e.first], e.second);


            // 顺着v -> order[v] -> order[order[v]] -> ... 一直往前走
            v = nv;
            std::swap(v_outEdges, nv_outEdges);
        } while (!flags[v]);
    }

    assert(g.size() == E);
}
