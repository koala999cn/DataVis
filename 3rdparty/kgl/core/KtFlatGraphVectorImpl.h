#pragma once
#include "edge_traits.h"
#include "vertex_traits.h"
#include "KtFlatGraphBase.h"
#include <vector>
#include <assert.h>


/// 基于std::vector的KtFlatGraphBase实现

namespace kPrivate
{
	template<typename EDGE_TYPE>
	using flatvg_edge_container = std::vector<edge_has_to_t<EDGE_TYPE>>;

	template<typename VERTEX_TYPE>
	using flatvg_vertex_container = std::vector<vertex_has_edgeindex_t<VERTEX_TYPE>>;

	// 传递引用给KtFlatGraphBase
	template<typename EDGE_TYPE, typename VERTEX_TYPE>
	using flatvg_base = KtFlatGraphBase<flatvg_edge_container<EDGE_TYPE>, 
		flatvg_vertex_container<VERTEX_TYPE>>;
}


template<typename EDGE_TYPE, typename VERTEX_TYPE = void>
class KtFlatGraphVectorImpl : public kPrivate::flatvg_base<EDGE_TYPE, VERTEX_TYPE>
{
public:
	using super_ = kPrivate::flatvg_base<EDGE_TYPE, VERTEX_TYPE>;
	using edge_container = kPrivate::flatvg_edge_container<EDGE_TYPE>;
	using vertex_container = kPrivate::flatvg_vertex_container<VERTEX_TYPE>;

	using edge_type = EDGE_TYPE;
	using vertex_type = VERTEX_TYPE;

	using edge_iter = typename edge_container::iterator;
	using edge_traits_t = edge_traits<typename super_::edge_type>;
	using edge_traits_helper_t = edge_traits_helper<edge_traits_t>;

	using vertex_traits_t = vertex_traits<typename super_::vertex_type>;
	using vertex_traits_helper_t = vertex_traits_helper<vertex_traits_t>;

	using super_::vertexes_;
	using super_::edges_;
	using super_::order;
	using super_::outedges;


	unsigned size() const {
		return static_cast<unsigned>(super_::size() - dummyEdges_);
	}


	void reset(unsigned nv) {
		vertexes_.resize(nv), edges_.clear(), dummyEdges_ = 0;
		for (auto& v : vertexes_)
			vertex_traits_t::edgeindex(v) = 0;
	}

	void reserve(unsigned nv, unsigned ne) {
		vertexes_.reserve(nv), edges_.reserve(ne);
	}

	void reserveEdges(unsigned v, unsigned ne) {}


	template<typename T, std::enable_if_t<!std::is_void_v<T>
		&& std::is_convertible_v<T, vertex_type>, bool> = false>
	unsigned addVertex(const T& v) {
		vertexes_.push_back(vertex_type(v));
		vertex_traits_t::edgeindex(vertexes_.back()) = static_cast<unsigned>(edges_.size());

		return vertexes_.size();
	}

	template<typename T, std::enable_if_t<!std::is_void_v<T>
		&& std::is_same_v<T, vertex_type>, bool> = false>
	unsigned addVertex(T&& v) {
		vertexes_.push_back(std::move(v));
		vertex_traits_t::edgeindex(vertexes_.back()) = static_cast<unsigned>(edges_.size());

		return static_cast<unsigned>(vertexes_.size());
	}

	template<typename T = vertex_type, std::enable_if_t<std::is_void_v<T>, bool> = true>
	unsigned addVertex() {
		vertexes_.push_back(static_cast<unsigned>(edges_.size()));
		return static_cast<unsigned>(vertexes_.size());
	}


	template<bool dummy = false>
	edge_iter addEdge(unsigned from, edge_iter pos, const edge_type& edge) {
		assert(from < order());
		assert(pos >= outedges(from).begin() && pos <= outedges(from).end());

		auto iter = edges_.insert(pos, edge);

		updateEdgeIndex_(from, 1);

		if (dummy) ++dummyEdges_;

		return iter;
	}

	// 删除顶点v
	void eraseVertex(unsigned v) {
		assert(outedges(v).size() == 0);
		vertexes_.erase(vertexes_.begin() + v);
	}


	// 删除顶点v的出边e
	template<bool dummy = false>
	edge_iter eraseEdge(unsigned v, edge_iter e) {
		assert(v == super_::edgeFrom(unsigned(std::distance(edges_.begin(), e))));

		updateEdgeIndex_(v, -1);

		if (dummy) --dummyEdges_;

		return edges_.erase(e);
	}


	template<bool dummy = false>
	edge_iter eraseEdges(unsigned v, edge_iter first, edge_iter last) {
		assert(first >= outedges(v).begin() && last <= outedges(v).end());

		int ne = int(std::distance(first, last));
		if (dummy) dummyEdges_ -= ne;

		updateEdgeIndex_(v, -ne);
		return edges_.erase(first, last);
	}


private:

	// 更新v后接顶点（不含v）的偏移：edgeIndex += diff
	void updateEdgeIndex_(unsigned v, int diff) {
		for (++v; v < order(); v++) 
			super_::edgeIndex(v) += diff;
	}

	
private:
	unsigned dummyEdges_; // 对于无向图添加的反边数量
};



template<typename EDGE_TYPE, typename VERTEX_TYPE>
struct graph_traits<KtFlatGraphVectorImpl<EDGE_TYPE, VERTEX_TYPE>>
	: public graph_traits<kPrivate::flatvg_base<EDGE_TYPE, VERTEX_TYPE>>
{
	constexpr static bool reshapable = true;
	constexpr static bool immutable = false;
};
