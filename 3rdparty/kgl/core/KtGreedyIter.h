#pragma once
#include "../GraphX.h"
#include "../util/copy.h"


// 贪婪模式下的边迭代器
// 与KtDfsIter的区别主要在于对环的处理：
//  -- KtDfsIter遇到环时，不会将环的闭合顶点入栈，始终保持堆栈中各顶点的唯一性
//  -- KtGreedyIter遇到环时，会将闭合顶点入栈，继续沿着该顶点进行深度优先搜索，闭合顶点在堆栈中存在多个实例

template<typename GRAPH, bool fullGraph = false, bool stopAtPopping = false>
class KtGreedyIter
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;
    using edge_type = typename GRAPH::edge_type;
	using flat_graph_t = KtGraph<KtFlatGraphVectorImpl<edge_type>, // 忽略顶点对象
		GRAPH::isDigraph(), GRAPH::isMultiEdges(), GRAPH::isAlwaysSorted()>;
    using const_edge_ref = decltype(std::declval<const flat_graph_t>().edgeAt(0));

    // 堆栈元素类型，<0>为from顶点，<1><2>为待遍历edge_index的range
    using stack_element_t = std::tuple<vertex_index_t, vertex_index_t, vertex_index_t>; 


	KtGreedyIter(GRAPH& graph, vertex_index_t startVertex) {
		copy(graph, graph_);
		vedges_.resize(graph.isDigraph() ? graph.size() : graph.size() * 2, false);
        start(startVertex);
	}


    void operator++() {
        assert(!isEnd());

        if constexpr (stopAtPopping)
            if (isPopping()) {
                todo_.pop_back();
                fixStack_();
                return;
            }

        auto& edgeidx = std::get<1>(todo_.back());
        assert(!vedges_[edgeidx]);
        markEdge_(edgeidx);
        auto to = **this;
        ++edgeidx;
        inStack_(to);
        fixStack_();
    }


    // 返回当前正在游历的顶点
    vertex_index_t operator*() const { 
        return isPopping() ? std::get<0>(todo_.back())
                           : edge_traits<typename graph_traits<flat_graph_t>::underly_edge_t>::to(edge());
    }


    // 与当前顶点（to顶点）构成边的from顶点
    vertex_index_t from() const {
        assert(!isEnd());
        if (isPopping())
            return todo_.size() > 1 ? std::get<0>(todo_[todo_.size() - 2]) : -1;
        else 
            return std::get<0>(todo_.back());
    }


    // 返回边(from, to)的值
    const_edge_ref edge() const {
        assert(!isEnd());
        return graph_.edgeAt(std::get<1>(todo_.back()));
    }


    bool isEnd() const { return todo_.empty(); }


    // 从顶点v开始接续进行广度优先遍历
    void start(vertex_index_t v) {
        assert(todo_.empty());
        inStack_(v);
    }


    template<bool dummy = stopAtPopping, typename = std::enable_if_t<dummy>>
    bool isPopping() const {
        return testStack_();
    }

private:

    // 辅助函数，测试堆栈顶部的tuple是否<1> == <2>
    bool testStack_() const {
        return std::get<1>(todo_.back()) == std::get<2>(todo_.back());
    }

    // 入栈
    void inStack_(vertex_index_t v) {
        auto r = graph_.outedges(v);
        auto edgeidx = graph_.edgeIndex(v);
        todo_.push_back({ v,  edgeidx, static_cast<vertex_index_t>(edgeidx + r.size()) });
    }

    void fixStack_() {

        // 检测当前顶点是否需要跳过
        while (!todo_.empty()) {

            // 移除已结束的迭代器
            if (testStack_()) {
                if constexpr (stopAtPopping) 
                    return; // stop at popping

                // popOrd_[iter.from()] = popIdx_++;
                todo_.pop_back();
                continue;
            }

            //assert(!isPopping() && !iter.isEnd());

            if (testSkip_()) {
                ++std::get<1>(todo_.back());
                continue;
            }

            break;
        }

        // 接续遍历
        if constexpr (fullGraph) {
            if (todo_.empty()) {
                unsigned edgeidx = firstUnvisited_();
                if (edgeidx != -1)
                    start(graph_.edgeFrom(edgeidx)); // 接续遍历
            }
        }
    }


    unsigned firstUnvisited_() const {
        auto pos = std::find(vedges_.begin(), vedges_.end(), false);
        return pos == vedges_.end() ? -1
            : static_cast<vertex_index_t>(std::distance(vedges_.begin(), pos));
    }


    bool testSkip_() const {
        assert(!testStack_());
        auto edgeidx = std::get<1>(todo_.back());
        return vedges_[edgeidx];
    }


    // 标记第edgeidx边为已遍历
    void markEdge_(unsigned edgeidx) {
        assert(!vedges_[edgeidx]);

        vedges_[edgeidx] = true; // 设置为已遍历
        auto edgefrom = from();
        auto edgeto = **this;

        // 对于无向图，还须标记反向边
        if constexpr (!flat_graph_t::isDigraph()) {
            if (edgefrom != edgeto) {
                const edge_type& wt = graph_.edgeAt(edgeidx);
                auto posbase = graph_.outedges(edgeto).begin();
                auto idxbase = graph_.edgeIndex(edgeto);
                auto edges = graph_.edges(edgeto, edgefrom);
                assert(!edges.empty());
                auto pos = edges.begin();
                auto idx = idxbase + std::distance(posbase, pos); // from pos to edgeidx

                if constexpr (flat_graph_t::isMultiEdges()) {
                    while (wt != *pos || vedges_[idx]) {
                        ++edges;
                        pos = edges.begin();
                        idx = idxbase + std::distance(posbase, pos);
                    }
                }

                assert(!vedges_[idx]);
                vedges_[idx] = true;
            }
        }
    }

private:
	flat_graph_t graph_;
	std::vector<bool> vedges_; // edges' visit-flag
    std::vector<stack_element_t> todo_;
};

