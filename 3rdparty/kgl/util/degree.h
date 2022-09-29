#pragma once
#include "loop.h"


// KtGraph::degree���޶���
// ��Ҫ������������ͼ�Ի��Ķ������㲻ͬ
//   KtGraph::degree�趨����ͼ���Ի�����Ϊ1
//   �˴��趨����ͼ���Ի�����Ϊ2

template<typename GRAPH>
unsigned degree(const GRAPH& g, unsigned v)
{
	unsigned d = g.degree(v);

	if constexpr (!g.isDigraph())
		d += selfloops(g, v);

	return d;
}