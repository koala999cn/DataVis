#pragma once
#include "loop.h"


// KtGraph::degree的修订版
// 主要差异在于无向图自环的度数计算不同
//   KtGraph::degree设定无向图的自环度数为1
//   此处设定无向图的自环度数为2

template<typename GRAPH>
unsigned degree(const GRAPH& g, unsigned v)
{
	unsigned d = g.degree(v);

	if constexpr (!g.isDigraph())
		d += selfloops(g, v);

	return d;
}