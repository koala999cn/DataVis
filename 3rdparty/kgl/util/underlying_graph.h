#pragma once
#include "copy.h"
#include "type_converter.h"


// 构造有向图的底图（基础图）
template<typename DIGRAPH, typename GRAPH = graph_of<DIGRAPH>, 
	typename = std::enable_if_t<DIGRAPH::isDigraph() && !GRAPH::isDigraph()>>
GRAPH underlying_graph(const DIGRAPH& dg)
{
	return copy<DIGRAPH, GRAPH>(dg);
}


template<typename DIGRAPH, typename GRAPH = graph_of<DIGRAPH>,
	typename = std::enable_if_t<DIGRAPH::isDigraph() && !GRAPH::isDigraph()>>
void underlying_graph(const DIGRAPH& dg, GRAPH& g)
{
	copy(dg, g);
}
