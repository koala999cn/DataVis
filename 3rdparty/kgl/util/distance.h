#pragma once
#include <vector>
#include "../core/KtShortestPath.h"
#include "../core/KtWeightor.h"


/*
 * The distance between two vertices uand v is the length of the shortest
 * path between uand v.
 */


 // 单源distance
template<typename GRAPH>
auto distance(const GRAPH& g, unsigned u) 
{
	KtSsspDijkstra<GRAPH, unit_min_wtor<GRAPH>> sspt(g, u);
	std::vector<typename unit_min_wtor<GRAPH>::weight_type> dist(g.order());
	for (unsigned v = 0; v < g.order(); g++)
		dist[v] = sspt.reachable(v) ? sspt.distance(v) : -1;

	return dist;
}


// 全源distance
template<typename GRAPH>
auto distance(const GRAPH& g) 
{
	using weight_type = typename unit_min_wtor<GRAPH>::weight_type;
	std::vector<std::vector<weight_type>> distm(g.order());
	for (unsigned v = 0; v < g.order(); g++)
		distm = distance(g, v);
	return distm;
}


// 两点distance，返回-1表示两点无连接
template<typename GRAPH>
auto distance(const GRAPH& g, unsigned u, unsigned v) 
{
	return distance(g, u)[v];
}
