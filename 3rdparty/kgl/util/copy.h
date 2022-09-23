#pragma once
#include <assert.h>
#include "../core/KtWeightor.h"


template<typename SRC, typename DST, typename WTOR = KtWeightSelf<typename SRC::edge_type>>
DST copy(const SRC& src)
{
    DST dst;
    dst.reserve(src.order(), src.size());
    dst.reset(src.order()); 
    for (unsigned v = 0; v < src.order(); v++) {
        unsigned wMax = src.isDigraph() ? unsigned(src.order()) : v + 1;
        for (unsigned w = 0; w < wMax; w++)
            for (auto r = src.edges(v, w); !r.empty(); ++r)
                if constexpr (std::is_same_v<typename DST::edge_type, bool>) {
                    dst.addEdge(v, w);
                    if constexpr (DST::isDigraph() && !SRC::isDigraph()) // 无向图转有向图，加双边
                        dst.addEdge(w, v);
                }
                else {
                    dst.addEdge(v, w, WTOR{}(*r));
                    if constexpr (DST::isDigraph() && !SRC::isDigraph()) // 无向图转有向图，加双边
                        dst.addEdge(w, v, WTOR{}(*r));
                }

        if constexpr (SRC::hasVertex() && DST::hasVertex())
            dst.vertexAt(v) = src.vertexAt(v);
    }

    assert(src.size() == dst.size());
    return dst;
}


template<typename SRC, typename DST, typename WTOR = KtWeightSelf<typename SRC::edge_type>>
void copy(const SRC& src, DST& dst)
{
    dst = copy<SRC, DST, WTOR>(src);
}