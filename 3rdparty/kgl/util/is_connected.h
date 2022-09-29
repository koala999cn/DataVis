#pragma once
#include "../core/KtBfsIter.h"


// 判断是否连通图
template<typename GRAPH>
bool is_connected(const GRAPH& g)
{
    KtBfsIter<const GRAPH> iter(g, 0);
    decltype(g.order()) V(0);
    for (; !iter.isEnd(); ++iter)
        ++V;

    return V == g.order();
}