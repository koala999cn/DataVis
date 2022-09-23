#pragma once
#include <algorithm>
#include "../core/graph_traits.h"


// 判断两个图g1、g2的拓扑结构是否相同，不对顶点对象进行比较
// 假定顶点顺序一致
template<typename G1, typename G2>
bool is_topo_same(const G1& g1, const G2& g2)
{
    if (g1.order() != g2.order() || g1.size() != g2.size())
        return false;

    for (unsigned v = 0; v < g1.order(); v++) {
        for (unsigned w = 0; w < g1.order(); w++) {
            auto edges1 = g1.edges(v, w);
            auto edges2 = g2.edges(v, w);
            if (edges1.size() != edges2.size())
                return false;

            std::vector<typename G1::edge_type> e1;
            std::vector<typename G2::edge_type> e2;
            while (!edges1.empty()) {
                e1.push_back(*edges1);
                e2.push_back(*edges2);
                ++edges1, ++edges2;
            }


            if constexpr (!g1.isAlwaysSorted())
                std::sort(e1.begin(), e1.end());
            if constexpr (!g2.isAlwaysSorted())
                std::sort(e2.begin(), e2.end());

            for (size_t i = 0; i < e1.size(); i++)
                if (e1[i] != e2[i])
                    return false;
        }
    }

    return true;
}


template<typename G1, typename G2>
bool is_same(const G1& g1, const G2& g2)
{
    // 先比较顶点对象
    if constexpr (!std::is_same_v<graph_traits<G1>::vertex_type, graph_traits<G2>::vertex_type>)
        return false;

    if constexpr (!std::is_void_v<graph_traits<G1>::vertex_type>) {
        for (unsigned v = 0; v < g1.order(); v++)
            if (g1.vertexAt(v) != g2.vertexAt(v))
                return false;
    }


    // 再比较拓扑结构
    return is_topo_same(g1, g2);
}