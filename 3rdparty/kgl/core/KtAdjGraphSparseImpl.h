#pragma once
#include "KtAdjGraphBase.h"
#include "edge_traits.h"
#include "graph_traits.h"
#include <assert.h>


// 基于稀疏阵的邻接矩阵图实现

namespace kPrivate
{
    // 基于vector的稀疏矩阵
    template<typename EDGE_TYPE>
    using underly_edge_t = edge_has_to_t<EDGE_TYPE>;

    template<typename EDGE_TYPE>
    using row_type = std::vector<underly_edge_t<EDGE_TYPE>>;

    template<typename EDGE_TYPE>
    class KtSpMatrix : public std::vector<row_type<EDGE_TYPE>>
    {
    public:
        using super_ = std::vector<row_type<EDGE_TYPE>>;

        auto rows() const { return super_::size(); }

        decltype(auto) row(unsigned v) { 
            return KtRange(super_::at(v).begin(), 
            static_cast<unsigned>(super_::at(v).size())); 
        }

        decltype(auto) row(unsigned v) const { 
            return KtRange(super_::at(v).cbegin(), 
            static_cast<unsigned>(super_::at(v).size())); 
        }
    };
}


template<typename EDGE_TYPE, typename VERTEX_TYPE = void>
class KtAdjGraphSparseImpl : public KtAdjGraphBase<kPrivate::KtSpMatrix<EDGE_TYPE>, VERTEX_TYPE>
{
public:
    using super_ = KtAdjGraphBase<kPrivate::KtSpMatrix<EDGE_TYPE>, VERTEX_TYPE>;
    using edge_type = EDGE_TYPE;
    using vertex_type = VERTEX_TYPE;
    using edge_iter = typename graph_traits<super_>::edge_iter;
    using const_edge_iter = typename graph_traits<super_>::const_edge_iter;
    using underly_edge_t = typename graph_traits<super_>::underly_edge_t;
    constexpr static const edge_type& null_edge = edge_traits<edge_type>::null_edge;

    static_assert(std::is_same_v<kPrivate::underly_edge_t<EDGE_TYPE>, underly_edge_t>, "edge type mismatch");
    static_assert(edge_traits_helper<edge_traits<underly_edge_t>>::has_to, "edge type construct error");

    using super_::E_;
    using super_::adjMat_;
    using super_::order;
    using super_::outedges;


    KtAdjGraphSparseImpl() = default;

    // 重置图为nv顶点.
    void reset(unsigned nv) {
        E_ = 0;
        adjMat_.clear(); adjMat_.resize(nv);
        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.resize(nv);
    }

    // 预留nv个顶点和ne条边的存储.
    void reserve(unsigned nv, unsigned ne) {
        adjMat_.reserve(nv);
        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.reserve(nv);
    }

    // 对顶点v预留ne条边的存储.
    void reserveEdges(unsigned v, unsigned ne) {
        adjMat_[v].reserve(ne);
    }


    template<typename T = vertex_type, std::enable_if_t<std::is_void_v<T>, bool> = true>
    unsigned addVertex() {
        adjMat_.push_back(kPrivate::row_type<edge_type>());
        return static_cast<unsigned>(adjMat_.rows()) - 1;
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>
        && std::is_convertible_v<T, vertex_type>, bool> = false>
    unsigned addVertex(const T& v) {
        adjMat_.push_back(kPrivate::row_type<edge_type>());
        super_::vertexes_.push_back(vertex_type(v));
        return super_::vertexes_.size();
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>
        && std::is_same_v<T, vertex_type>, bool> = false>
    unsigned addVertex(T&& v) {
        adjMat_.push_back(kPrivate::row_type<edge_type>());
        super_::vertexes_.push_back(std::move(v));
        return static_cast<unsigned>(super_::vertexes_.size());
    }


    template<bool dummy = false>
    edge_iter addEdge(unsigned from, edge_iter pos, const edge_type& edge) {
        assert(from < order());

        if constexpr (!dummy) ++E_;

        return adjMat_[from].insert(pos, edge);
    }


    // 删除顶点v
    // 调用该函数前，确保已删除v相接的所有边
    void eraseVertex(unsigned v) {
        assert(v < order() && adjMat_[v].empty());

        adjMat_.erase(std::next(adjMat_.begin(), v));

        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.erase(std::next(super_::vertexes_.begin(), v));
    }

    // 删除顶点v的出边e
    template<bool dummy = false>
    edge_iter eraseEdge(unsigned v, edge_iter e) {
        assert(e >= outedges(v).begin() && e < outedges(v).end());
        if constexpr (!dummy) --E_;

        return adjMat_[v].erase(e);
    }

    // const_edge_iter版本
    template<bool dummy = false>
    edge_iter eraseEdge(unsigned v, const_edge_iter e) {
        assert(e >= outedges(v).cbegin() && e < outedges(v).cend());
        if constexpr (!dummy) --E_;

        return adjMat_[v].erase(e);
    }


    // 删除first到last之间的边
    template<bool dummy = false>
    edge_iter eraseEdges(unsigned v, edge_iter first, edge_iter last) {
        assert(first >= outedges(v).begin() && last <= outedges(v).end());

        if constexpr (!dummy) 
            E_ -= static_cast<decltype(E_)>(std::distance(first, last));

        return adjMat_[v].erase(first, last);
    }
};


template<typename EDGE_TYPE, typename VERTEX_TYPE>
struct graph_traits<KtAdjGraphSparseImpl<EDGE_TYPE, VERTEX_TYPE>>
    : public graph_traits<KtAdjGraphBase<kPrivate::KtSpMatrix<EDGE_TYPE>, VERTEX_TYPE>>
{
    constexpr static bool reshapable = true;
    constexpr static bool immutable = false;
};
