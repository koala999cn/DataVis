#pragma once
#include <vector>
#include "KtGreedyIter.h"
#include "../util/is_connected.h"
#include "../util/degree.h"
#include "../util/underlying_graph.h"


// 欧拉图判定和求解，实现
// 参考'Graph Algorithms', 2nd Edition, Shimon Even, 2012
/* 
 * 判定原则：
 * 
 * A finite(undirected) connected graph is an Euler graph if and
 * only if exactly two vertices are of odd degree or all vertices are of even degree.
 * In the latter case, every Euler path of the graph is a cycle, and in the former
 * case, none is.
 *
 * A finite digraph is an Euler digraph if and only if its underlying
 * graph is connected and one of the following two conditions holds:
 * (i) There is one vertex a such that outdegree(a) = indegree(a)+1,
 * and another vertex b such that outdegree(b)+1 = indegree(b),
 * while for every other vertex v, outdegree(v) = indegree(v).
 * (ii) For every vertex v, outdegree(v) = indegree(v).
 * In the former case, every directed Euler path starts at a and ends in b. In the
 * latter, every directed Euler path is a directed Euler cycle.
 *
 */

 /*
  * 求解算法：Hierholzer算法(插入回路法)，复杂度O(V+E).
  *
  * 算法过程（设置两个栈，curPath和cycle）：
  *  （1）选择任一顶点为起点，入栈curPath，深度搜索访问顶点，将经过的边都删除，经过的顶点入栈curPath。
  *  （2）如果当前顶点没有相邻边，则将该顶点从curPath出栈到cycle。
  *  （3）cycle栈中的顶点出栈顺序，就是从起点出发的欧拉回路。
  *
  */


// 使用KtGreedyIter模拟实现Hierholzer算法
// TODO: more test

template<typename GRAPH>
class KtEuler
{
public:
	KtEuler(const GRAPH& g) : g_(g) {}

	// test iff has euler-cycle or euler-path
	// 返回0表示非欧拉图，返回1表示有欧拉环，返回2表示有欧拉路径
	// @oddVertex: 若非nullptr，则返回第一个奇度顶点
	int test(unsigned* oddVertex = nullptr) const {

		if constexpr (GRAPH::isDigraph()) {
			graph_of<flat_of<GRAPH, bool, void>> g;
			underlying_graph(g_, g);
			if (!is_connected(g))
				return 0;

			bool odd[2] = { false }; // odd[0]为true，表示检测到outdegree(a) = indegree(a)+1
			                         // odd[1]为true，表示检测到outdegree(b) = indegree(b)-1

			for (unsigned v = 0; v < g_.order(); v++) {
				auto ideg = g_.indegree(v);
				auto odeg = g_.outdegree(v);
				if (odeg == ideg) continue;

				if (odeg == ideg + 1) {
					if (odd[0]) return 0;
					odd[0] = true;
					if (oddVertex) {
						*oddVertex = v;
						oddVertex = nullptr;
					}
				}
				else if (odeg == ideg - 1) {
					if (odd[1]) return 0;
					odd[1] = true;
				}
				else {
					return 0;
				}
			}

			if (odd[0] && odd[1]) return 2;
			if (!odd[0] && !odd[1]) return 1;
			return 0;
		}
		else {
			if (!is_connected(g_))
				return 0;

			int odd(0); // 奇度顶点数
			for (unsigned v = 0; v < g_.order(); v++) {
				if (degree(g_, v) % 2) {
					++odd;
					if (oddVertex) {
						*oddVertex = v;
						oddVertex = nullptr;
					}
				}
				if (odd > 2) return 0;
			}

			assert(odd != 1); // 无向图不可能只有1个奇度顶点

			return odd == 0 ? 1 : 2;
		}
	}


	std::vector<unsigned> getCycle(unsigned startVertex = 0) const {
		std::vector<unsigned> cycle;
		KtGreedyIter<const GRAPH, false, true> dfs(g_, startVertex);
		for (; !dfs.isEnd(); ++dfs) {
			if (dfs.isPopping())
				cycle.push_back(*dfs);
		}

		if constexpr (GRAPH::isDigraph())
			std::reverse(cycle.begin(), cycle.end());

		return cycle;
	}


	std::vector<unsigned> getPath(unsigned startVertex) const {
		assert(degree(g_, startVertex) % 2); // startVertex为奇度顶点
		return getCycle(startVertex);
	}

private:
	const GRAPH& g_;
};
