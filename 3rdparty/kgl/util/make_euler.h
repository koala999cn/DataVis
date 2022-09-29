#pragma once
#include <vector>
#include "randgen.h"
#include "make_connect.h"
#include "degree.h"


// ����ŷ��ͼ����ʱֻ֧������ͼ

template<typename GRAPH>
void make_euler(GRAPH& g, bool cycle)
{
	make_connect(g);

	// �Ѽ�����Ϊ�����Ķ���
	std::vector<unsigned> odds; odds.reserve(g.order() / 2);
	for (unsigned v = 0; v < g.order(); v++)
		if (degree(g, v) % 2)
			odds.push_back(v);

	assert(odds.size() % 2 == 0); // ��������ĸ�����Ϊż��

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