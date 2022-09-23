#pragma once
#include "graph_traits.h"
#include "edge_traits.h"
#include "vertex_traits.h"
#include <assert.h>


// 邻接顶点迭代器实现


// 主模板实现sparse图的adj_iter
template<typename GRAPH>
class KtAdjIter
{
public:
    using edge_type = typename graph_traits<GRAPH>::edge_type;
    using deref_type = typename graph_traits<GRAPH>::edge_deref_t;
    using const_deref_type = typename graph_traits<GRAPH>::const_edge_deref_t;
    using edge_range = typename graph_traits<GRAPH>::outedges_result_t;
    using underly_edge_t = typename graph_traits<GRAPH>::underly_edge_t;
    using underly_vertex_t = typename graph_traits<GRAPH>::underly_vertex_t;


    // 构造一个空的adj_iter
    KtAdjIter(GRAPH& g) : graph_(g), from_(-1) {}


    KtAdjIter(GRAPH& g, unsigned v)
        : graph_(g), range_(g.outedges(v)), from_(v) {
        if constexpr (GRAPH::isDense()) {
            // skip null edge
            if (*range_ == edge_traits<edge_type>::null_edge)
                ++(*this);
        }
    }


    unsigned from() const { return from_; }
    unsigned to() const { 
        if constexpr (GRAPH::isDense())
            return static_cast<unsigned>(std::distance(graph_.outedges(from_).begin(), range_.begin()));
        else
            return edge_traits<underly_edge_t>::to(*range_);
    }

    unsigned operator*() const { return to(); }


    void operator++() {
        assert(!isEnd());
        ++range_;

        if constexpr (GRAPH::isDense()) {
            while (!isEnd() && *range_ == edge_traits<edge_type>::null_edge)
                ++range_;
        }
    }

    bool isEnd() const { return range_.empty(); }


    // get the value of current edge
    const edge_type& edge() const { return *range_; }

    // reset the value of current edge
    void reedge(const edge_type& val) {
        if constexpr (GRAPH::isDigraph()) { // 对有向图提供一个快速操作
            *range_ = val;
        }
        else if constexpr (GRAPH::isMultiEdges()) {
            graph_.setEdge(from(), to(), edge(), val);        
        }
        else {
            graph_.setEdge(from(), to(), val);
        }
    }

    // erase the current edge
    void erase() {
        if constexpr (GRAPH::isDense()) {
            graph_.eraseEdge(from(), to());
            ++(*this);
        }
        else {
            if constexpr (!GRAPH::isDigraph()) { // 先删除无向图的(to, from)边
                if (from() != to()) {
                    auto edges = graph_.edges(to(), from());
                    assert(!edges.empty());
                    if constexpr (GRAPH::isMultiEdges())
                        while ((const edge_type&)(*edges.begin()) != (const edge_type&)(*range_.begin()))
                            ++edges;
                    assert((const edge_type&)(*edges.begin()) == (const edge_type&)(*range_.begin()));
                    graph_.template eraseEdge<true>(to(), edges.begin());

                    if constexpr (has_outdegree_v<underly_vertex_t>)
                        vertex_traits<underly_vertex_t>::outdegree(graph_.vertexAt(to()))--;
                }
            }

            // 删除(from, to)边
            auto pos = graph_.eraseEdge(from(), range_.begin());
            auto end = graph_.outedges(from()).end();
            range_.reset(pos, end);

            if constexpr (has_outdegree_v<underly_vertex_t>)
                vertex_traits<underly_vertex_t>::outdegree(graph_.vertexAt(from()))--;
        }
    }

protected:
    GRAPH& graph_;
    edge_range range_;
    unsigned from_;
};
