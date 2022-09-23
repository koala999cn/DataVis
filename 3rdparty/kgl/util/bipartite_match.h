#pragma once
#include <vector>
#include "../GraphX.h"
#include "../core/KtMaxFlow.h"
#include "../core/KtBipartite.h"
#include "../core/KtAdjIter.h"


/// 基于最大流算法实现的最大基数二部匹配，算法复杂度O(VE)
/// 
/// 最大基数二部匹配：给定一个二部图，找出一个有最大基数的边集，
/// 从而使各个顶点至多与一个其它顶点相连接。
/// 

// @GRAPH: 二部图（无向图）
// @return: 匹配的顶点对，empty表示错误（输入的图非二部图）
template<typename GRAPH>
auto bipartite_match(const GRAPH& g) -> std::vector<std::pair<unsigned, unsigned>>
{
	std::vector<std::pair<unsigned, unsigned>> res;
	KtBipartite<GRAPH> bip(g);
	if (bip.ok()) {
		DigraphSx<int> newG(g.order() + 2);
		auto s = g.order(); // 最后两个顶点为st顶点
		auto t = s + 1;

		for (unsigned v = 0; v < g.order(); v++) {
			if (bip.color(v) == 1) {
				newG.addEdge(s, v, 1);

				auto adj = KtAdjIter(g, v);
				for (; !adj.isEnd(); ++adj)
					newG.addEdge(v, *adj, 1);
			}
			else
				newG.addEdge(v, t, 1);
		}

		KtMaxFlowPre mf(newG, s, t);
		assert(mf.check(s, t));

		res.reserve(g.order() / 2);
		for (unsigned v = 0; v < g.order(); v++) {
			auto adj = KtAdjIter(newG, v);
			for (; !adj.isEnd(); ++adj) {
				if (*adj != t && mf.flow(v, *adj) == 1) {
					res.push_back({ v, *adj }); 
					break; // 最多只有一条边有流量
				}
			}
		}
	}

	return res;
}

