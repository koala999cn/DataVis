#pragma once
#include "../base/traits_helper.h"

namespace kPrivate 
{
	HAS_MEMBER(vertexAt);

	template<typename GRAPH, bool hasVertexAt>
	struct underly_vertex_helper {
		using type = void;
	};

	template<typename GRAPH>
	struct underly_vertex_helper<GRAPH, true> {
		using type = std::decay_t<decltype(std::declval<GRAPH>().vertexAt(0))>;
	};

	template<typename GRAPH>
	using underly_vertex_t = typename underly_vertex_helper<GRAPH, has_member_vertexAt<GRAPH, unsigned>::value>::type;
}



template<typename GRAPH>
struct graph_traits
{
	/// 须手动设置的标志
	constexpr static bool reshapable = false; // support reset, addEdge, addVertex, eraseEdge, eraseVertex, ...
	constexpr static bool immutable = true; // support setEdge, ...
	constexpr static bool is_dense = false;


	/// 图的相关类型
	using graph_type = GRAPH;
	using const_graph_type = std::add_const_t<graph_type>;
	using edge_type = typename graph_type::edge_type;
	using vertex_type = typename graph_type::vertex_type;
	using outedges_result_t = decltype(std::declval<graph_type>().outedges(0));
	using const_outedges_result_t = decltype(std::declval<const_graph_type>().outedges(0));
	using edge_iter = typename std::decay_t<outedges_result_t>::iterator;
	using const_edge_iter = typename std::decay_t<const_outedges_result_t>::const_iterator;
	using edge_deref_t = decltype(*std::declval<edge_iter>());
	using const_edge_deref_t = decltype(*std::declval<const_edge_iter>());
	using underly_edge_t = typename std::decay_t<decltype(*std::declval<edge_iter>())>;
	using underly_vertex_t = kPrivate::underly_vertex_t<graph_type>;
};

