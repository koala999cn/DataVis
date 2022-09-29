#pragma once
#include <vector>
#include "degree.h"


// 计算图g的最大最小度
template<typename GRAPH>
std::pair<unsigned, unsigned> minmax_degree(const GRAPH& g)
{
	unsigned maxdeg = 0, mindeg = std::numeric_limits<unsigned>::max();
	for (unsigned v = 0; v < g.order(); v++) {
		auto vdeg = degree(g, v);
		if (vdeg > maxdeg)
			maxdeg = vdeg;
		if (vdeg < mindeg)
			mindeg = vdeg;
	}

	return { mindeg, maxdeg };
}

