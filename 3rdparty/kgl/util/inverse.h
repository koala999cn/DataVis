#pragma once
#include "../core/KtBfsIter.h"


// 计算当前图的逆
template<typename INGRAPH, typename OUTGRAPH = INGRAPH>
OUTGRAPH inverse(const INGRAPH& g)
{
    OUTGRAPH gR(g.order()); gR.reserve(g.order(), g.size());
    KtBfsIter<const INGRAPH, true, true> iter(g, 0);
    for (; !iter.isEnd(); ++iter)
        gR.addEdge(*iter, iter.from(), iter.edge());

    if constexpr (OUTGRAPH::hasVertex() && INGRAPH::hasVertex())
        for (unsigned v = 0; v < g.order(); v++)
            gR.vertexAt(v) = g.vertexAt(v);

    return gR;
}


template<typename INGRAPH, typename OUTGRAPH>
void inverse(const INGRAPH& g, OUTGRAPH& gR)
{
    gR = inverse<INGRAPH, OUTGRAPH>(g);
}
