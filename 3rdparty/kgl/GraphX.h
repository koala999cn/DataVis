#pragma once 
#include "core/KtAdjGraphDenseImpl.h"
#include "core/KtAdjGraphSparseImpl.h"
#include "core/KtFlatGraphVectorImpl.h"
#include "core/KtGraph.h"
#include "core/KtAdjIter.h"
#include "core/KtBfsIter.h"
#include "core/KtDfsIter.h"
#include "core/KtPfsIter.h"


// 对一些常用迭代器的封装
template<typename GRAPH>
class KtGraphX : public GRAPH
{
public:
	using graph_type = GRAPH;
	using const_graph_type = std::add_const_t<graph_type>;

	using adj_iter = KtAdjIter<graph_type>;
	using const_adj_iter = KtAdjIter<const_graph_type>;

	template<bool fullGraph, bool modeEdge>
	using bfs_iter = KtBfsIter<graph_type, fullGraph, modeEdge>;
	template<bool fullGraph, bool modeEdge>
	using const_bfs_iter = KtBfsIter<const_graph_type, fullGraph, modeEdge>;

	template<bool fullGraph, bool modeEdge, bool stopAtPopping>
	using dfs_iter = KtDfsIter<graph_type, fullGraph, modeEdge, stopAtPopping>;
	template<bool fullGraph, bool modeEdge, bool stopAtPopping>
	using const_dfs_iter = KtDfsIter<const_graph_type, fullGraph, modeEdge, stopAtPopping>;

	using edge_iter = bfs_iter<true, true>;
	using const_edge_iter = const_bfs_iter<true, true>;

	using typename graph_type::vertex_index_t;
	using graph_type::graph_type;


	auto adjIter(vertex_index_t s) {
		return adj_iter(*this, s);
	}

	auto adjIter(vertex_index_t s) const {
		return const_adj_iter(*this, s);
	}


	template<bool fullGraph, bool modeEdge>
	auto bfsIter(vertex_index_t s) {
		return bfs_iter<fullGraph, modeEdge>(*this, s);
	}

	template<bool fullGraph, bool modeEdge>
	auto bfsIter(vertex_index_t s) const {
		return const_bfs_iter<fullGraph, modeEdge>(*this, s);
	}


	template<bool fullGraph, bool modeEdge, bool stopAtPopping>
	auto dfsIter(vertex_index_t s) {
		return dfs_iter<fullGraph, modeEdge, stopAtPopping>(*this, s);
	}

	template<bool fullGraph, bool modeEdge, bool stopAtPopping>
	auto dfsIter(vertex_index_t s) const {
		return const_dfs_iter<fullGraph, modeEdge, stopAtPopping>(*this, s);
	}


	auto edgeIter() {
		return edge_iter(*this, 0);
	}

	auto edgeIter() const {
		return const_edge_iter(*this, 0);
	}
};


/// 稠密无向图
template<typename EDGE_TYPE, typename VERTEX_TYPE = void>
using GraphDx = KtGraphX<KtGraph<KtAdjGraphDenseImpl<EDGE_TYPE, VERTEX_TYPE>, false, false, false>>;

using GraphDi = GraphDx<int>;
using GraphDd = GraphDx<double>;
using GraphDf = GraphDx<float>;


/// 稠密有向图
template<typename EDGE_TYPE, typename VERTEX_TYPE = void>
using DigraphDx = KtGraphX<KtGraph<KtAdjGraphDenseImpl<EDGE_TYPE, VERTEX_TYPE>, true, false, false>>;

using DigraphDi = DigraphDx<int>;
using DigraphDd = DigraphDx<double>;
using DigraphDf = DigraphDx<float>;


/// 稀疏无向图
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using GraphSx = KtGraphX<KtGraph<KtAdjGraphSparseImpl<EDGE_TYPE, VERTEX_TYPE>, false, false, alwaysSorted>>;

template<bool alwaysSorted = false>
using GraphSi = GraphSx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphSd = GraphSx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphSf = GraphSx<float, void, alwaysSorted>;


/// 稀疏有向图
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using DigraphSx = KtGraphX<KtGraph<KtAdjGraphSparseImpl<EDGE_TYPE, VERTEX_TYPE>, true, false, alwaysSorted>>;

template<bool alwaysSorted = false>
using DigraphSi = DigraphSx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphSd = DigraphSx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphSf = DigraphSx<float, void, alwaysSorted>;


/// 平行无向图（允许重边/平行边）
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using GraphPx = KtGraphX<KtGraph<KtAdjGraphSparseImpl<EDGE_TYPE, VERTEX_TYPE>, false, true, alwaysSorted>>;

template<bool alwaysSorted = false>
using GraphPi = GraphPx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphPd = GraphPx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphPf = GraphPx<float, void, alwaysSorted>;


/// 平行有向图（允许重边/平行边）
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using DigraphPx = KtGraphX<KtGraph<KtAdjGraphSparseImpl<EDGE_TYPE, VERTEX_TYPE>, true, true, alwaysSorted>>;

template<bool alwaysSorted = false>
using DigraphPi = DigraphPx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphPd = DigraphPx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphPf = DigraphPx<float, void, alwaysSorted>;


/// 扁平无向图（允许重边/平行边）
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using GraphFx = KtGraphX<KtGraph<KtFlatGraphVectorImpl<EDGE_TYPE, VERTEX_TYPE>, false, true, alwaysSorted>>;

template<bool alwaysSorted = false>
using GraphFi = GraphFx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphFd = GraphFx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using GraphFf = GraphFx<float, void, alwaysSorted>;


/// 扁平有向图（允许重边/平行边）
template<typename EDGE_TYPE, typename VERTEX_TYPE = void, bool alwaysSorted = false>
using DigraphFx = KtGraphX<KtGraph<KtFlatGraphVectorImpl<EDGE_TYPE, VERTEX_TYPE>, true, true, alwaysSorted>>;

template<bool alwaysSorted = false>
using DigraphFi = DigraphFx<int, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphFd = DigraphFx<double, void, alwaysSorted>;

template<bool alwaysSorted = false>
using DigraphFf = DigraphFx<float, void, alwaysSorted>;
