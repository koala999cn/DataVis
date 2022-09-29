#pragma once
#include <vector>
#include "randgen.h"
#include "make_connect.h"
#include "degree.h"


// 构造欧拉图，暂时只支持无向图

template<typename GRAPH>
void make_euler(GRAPH& g, bool cycle)
{
	make_connect(g);

	// 搜集度数为奇数的顶点
	std::vector<unsigned> odds; odds.reserve(g.order() / 2);
	for (unsigned v = 0; v < g.order(); v++)
		if (degree(g, v) % 2)
			odds.push_back(v);

	assert(odds.size() % 2 == 0); // 奇数顶点的个数必为偶数

	unsigned resid = cycle ? 0 : 2;
	while (odds.size() > resid) {
		g.addEdge(odds[odds.size() - 2], odds.back());
		odds.pop_back(), odds.pop_back();
	}
}


template<typename GRAPH>
GRAPH make_euler(unsigned V, unsigned E, bool cycle)
{
	auto g = randgen<GRAPH>(V, E);
	make_euler(g, cycle);
	return g;
}