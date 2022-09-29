#pragma once


// 转换为DIGRAPH的无向图
template<typename DIGRAPH>
using graph_of = KtGraph<typename DIGRAPH::graph_impl, false, DIGRAPH::isMultiEdges(), DIGRAPH::isAlwaysSorted()>;

// 转换为GRAPH的有向图
template<typename GRAPH>
using digraph_of = KtGraph<typename GRAPH::graph_impl, true, GRAPH::isMultiEdges(), GRAPH::isAlwaysSorted()>;

// 转换为G的平行图
template<typename G>
using parallel_of = KtGraph<typename G::graph_impl, G::isDigraph(), true, G::isAlwaysSorted()>;

// 转换为G的有序图
template<typename G>
using sorted_of = KtGraph<typename G::graph_impl, G::isDigraph(), G::isMultiEdges(), true>;

// 转换为G的flat图
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using flat_of = KtGraph<KtFlatGraphVectorImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

// 转换为G的dense图
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using dense_of = KtGraph<KtAdjGraphDenseImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

// 转换为G的sparse图
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using sparse_of = KtGraph<KtAdjGraphSparseImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

