#pragma once
#include <algorithm>
#include <assert.h>
#include "../base/KtRange.h"


// 扁平图：顶点和边分别按顺序存储，每个顶点保存边的偏移信息，同一顶点的出边连续存储
// @EDGE_CONTAINER: 边容器
// @VERTEX_CONTAINER: 顶点容器
// 2个容器须具以下基本属性：
//     一是begin(), end()成员方法
//     二是size()成员方法
//     三是at(int)成员方法
//     四是iterator, const_iterator成员类型
template<typename EDGE_CONTAINER, typename VERTEX_CONTAINER>
class KtFlatGraphBase
{
	using vertex_container = VERTEX_CONTAINER; 
	using edge_container = EDGE_CONTAINER;
	
	using vertex_iter = std::conditional_t<std::is_const_v<vertex_container>,
		typename vertex_container::const_iterator, typename vertex_container::iterator>;
	using const_vertex_iter = typename vertex_container::const_iterator;
	using edge_iter = std::conditional_t<std::is_const_v<edge_container>,
		typename edge_container::const_iterator, typename edge_container::iterator>;
	using const_edge_iter = typename edge_container::const_iterator;

public:

	using const_vertex_deref_t = decltype(*std::declval<const_vertex_iter>());
	using vertex_deref_t = decltype(*std::declval<vertex_iter>());
	using vertex_type = typename std::remove_reference_t<vertex_deref_t>;
	using vertex_traits_t = vertex_traits<vertex_type>;

	using edge_deref_t = decltype(*std::declval<edge_iter>());
	using edge_type = typename std::remove_reference_t<edge_deref_t>;

	using edge_range = KtRange<edge_iter>;
	using const_edge_range = KtRange<const_edge_iter>;


	unsigned order() const { return static_cast<unsigned>(vertexes_.size()); }
	unsigned size() const { return static_cast<unsigned>(edges_.size()); }

	decltype(auto) edgeAt(unsigned idx) { return edges_.at(idx); }
	decltype(auto) edgeAt(unsigned idx) const { return edges_.at(idx); }

	decltype(auto) vertexAt(unsigned v) { return vertexes_.at(v); }
	decltype(auto) vertexAt(unsigned v) const { return vertexes_.at(v); }

	edge_range outedges(unsigned v) {
		auto start = std::next(std::begin(edges_), edgeIndex(v));
		auto last = (v == order() - 1) ? std::end(edges_)
			: std::next(std::begin(edges_), edgeIndex(v + 1));

		return edge_range(start, last);
	}

	const_edge_range outedges(unsigned v) const {
		auto start = std::next(std::cbegin(edges_), edgeIndex(v));
		auto last = (v == order() - 1) ? std::cend(edges_)
			: std::next(std::cbegin(edges_), edgeIndex(v + 1));

		return const_edge_range(start, last);
	}


	decltype(auto) edgeIndex(unsigned vertexIdx) const {
		return vertex_traits_t::edgeindex(vertexAt(vertexIdx));
	}

	decltype(auto) edgeIndex(unsigned vertexIdx) {
		return vertex_traits_t::edgeindex(vertexAt(vertexIdx));
	}

	// 根据边索引edgeIdx, 推算from顶点
	unsigned edgeFrom(unsigned edgeIdx) const {
		assert(edgeIdx < size());

		struct {
			bool operator()(unsigned idx, const_vertex_deref_t v) const {
				return idx < vertex_traits_t::edgeindex(v);
			}
		} comp;

		auto pos = std::upper_bound(std::cbegin(vertexes_), std::cend(vertexes_), edgeIdx, comp);
		auto from = std::distance(std::cbegin(vertexes_), pos);
		assert(from != 0 && from - 1 < static_cast<decltype(from)>(order()));
		return static_cast<unsigned>(from - 1);
	}


protected:
	edge_container edges_;
	vertex_container vertexes_;
};
