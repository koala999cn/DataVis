#pragma once
#include "../core/KtBfsIter.h"


// ���㵱ǰͼ����
template<typename GRAPH>
GRAPH inverse(const GRAPH& g) 
{
    GRAPH gR(g.order()); gR.reserve(g.order(), g.size());
    KtBfsIter<const GRAPH, true, true> iter(g, 0);
    for (; !iter.isEnd(); ++iter)
        gR.addEdge(*iter, iter.from(), iter.edge());

    return gR;
}
