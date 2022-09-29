#pragma once


// ת��ΪDIGRAPH������ͼ
template<typename DIGRAPH>
using graph_of = KtGraph<typename DIGRAPH::graph_impl, false, DIGRAPH::isMultiEdges(), DIGRAPH::isAlwaysSorted()>;

// ת��ΪGRAPH������ͼ
template<typename GRAPH>
using digraph_of = KtGraph<typename GRAPH::graph_impl, true, GRAPH::isMultiEdges(), GRAPH::isAlwaysSorted()>;

// ת��ΪG��ƽ��ͼ
template<typename G>
using parallel_of = KtGraph<typename G::graph_impl, G::isDigraph(), true, G::isAlwaysSorted()>;

// ת��ΪG������ͼ
template<typename G>
using sorted_of = KtGraph<typename G::graph_impl, G::isDigraph(), G::isMultiEdges(), true>;

// ת��ΪG��flatͼ
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using flat_of = KtGraph<KtFlatGraphVectorImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

// ת��ΪG��denseͼ
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using dense_of = KtGraph<KtAdjGraphDenseImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

// ת��ΪG��sparseͼ
template<typename G, typename E = typename G::edge_type, typename V = typename G::vertex_type>
using sparse_of = KtGraph<KtAdjGraphSparseImpl<E, V>, G::isDigraph(), G::isMultiEdges(), G::isAlwaysSorted()>;

