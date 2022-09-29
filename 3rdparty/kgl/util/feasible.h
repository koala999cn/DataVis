#pragma once
#include "../GraphX.h"
#include "../core/KtMaxFlow.h"
#include "copy.h"


/// 基于最大流算法实现的可行流判定
/// 
/// 可行流定义：假定为流网络每个顶点赋予权值，表示供应（权值为正）和需求（权值为负），
/// 并且顶点权值之和为0，那么如果一个流中每个顶点的流出量和流入量的差等于该顶点的权值，
/// 那么这个流是“可行的”。
/// 
/// 可行流判定可规约为最大流问题，主要思路如下：
/// 给定一个可行流问题，构造一个网，其中有相同的顶点和边，但是顶点无权值。
/// 另外，增加一个源点s，它到每个供应顶点（权值为正）都有一条权值等于该顶点
/// 供应量的边；再增加一个汇点t，从每个需求顶点到t都有一条权值等于该顶点
/// 需求量绝对值的边。在这个网上解决最大流问题，条件是当且仅当一个流中从源点
/// 出发的所有边和到达汇点的所有边都充满至其容量，这个流是原网的可行流。


// @GRAPH: st有向加权网。边的权值为整型，代表容量；顶点类型亦须为整型，代表供应（正值）或需求（负值）
template<typename GRAPH>
bool feasible(const GRAPH& g)
{
	static_assert(std::is_integral_v<typename GRAPH::vertex_type>, "graph must have integral vertex type");

	DigraphSx<typename GRAPH::edge_type> newG;
	copy(g, newG);
	auto s = newG.addVertex();
	auto t = newG.addVertex();

	for (unsigned v = 0; v < g.order(); v++) {
		typename GRAPH::edge_type wt = g.vertexAt(v);
		if (wt >= 0)
			newG.addEdge(s, v, wt);
		else
			newG.addEdge(v, t, -wt);
	}

	KtMaxFlowPre mf(newG, s, t);
	assert(mf.check(s, t));

	for (unsigned v = 0; v < g.order(); v++) {
		typename GRAPH::edge_type wt = g.vertexAt(v);
		if (wt >= 0) {
			if (mf.residual(s, v) != 0)
				return false;
		}
		else {
			if (mf.residual(v, t) != 0)
				return false;
		}
	}

	return true;
}

