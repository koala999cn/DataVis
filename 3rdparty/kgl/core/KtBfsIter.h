#pragma once
#include <queue>
#include <assert.h>
#include "KtAdjIter.h"


// 广度优先遍历
// 模板参数：
//    -- fullGraph，若为true，则进行全图迭代，否则仅迭代与v相联通的节点或边。
//    -- modeEdge，若为true，则对边进行广度优先遍历，这时顶点可能会多次重复，但每条边只会遍历一次。
//                 若为false，则对顶点进行广度优先遍历，这时不能保证每条边都会遍历到或只遍历一次。
// 非modeEdge模式下的初始状态，operator*()返回startVertex，from()方法返回-1
// modeEdge模式下的初始状态，operator*()返回startVertex的第一个邻接顶点，from()方法返回startVertex
template <typename GRAPH, bool fullGraph = false, bool modeEdge = false>
class KtBfsIter 
{
public:
    using graph_type = GRAPH;
    using edge_type = typename GRAPH::edge_type;
    using vertex_index_t = typename graph_type::vertex_index_t;
    using adj_vertex_iter = KtAdjIter<graph_type>;
    using const_edge_ref = decltype(std::declval<adj_vertex_iter>().edge());
    constexpr static vertex_index_t null_vertex = -1;


    // graph -- 待遍历的图
    // startVertex -- 遍历的起始顶点，-1表示只构建迭代器，需要另外调用start方法开始遍历
    KtBfsIter(graph_type& graph, vertex_index_t startVertex)
        : graph_(graph),
          v0_(null_vertex),
          isPushed_(graph.order(), false),
          isPopped_(graph.order(), false) {
        if (startVertex != null_vertex) 
            start(startVertex);
    }

    void operator++() {
        assert(!isEnd());

        if (!isPushed_[v0_]) {
            assert(todo_.empty());
            isPushed_[v0_] = true;
            todo_.push(adj_vertex_iter(graph_, v0_));
        } else {
            assert(!todo_.empty());
            auto& iter = todo_.front();
            vertex_index_t v = *iter;
            if (!isPushed_[v]) {
                isPushed_[v] = true;
                todo_.push(adj_vertex_iter(graph_, v));
            }
            ++iter;
        }

        while (!todo_.empty()) {
            auto& iter = todo_.front();

            // 移出已到末尾的迭代器
            if (iter.isEnd()) {
                isPopped_[from()] = true;
                todo_.pop();
                continue;
            }

            // 跳过已遍历的顶点或边
            if (modeEdge) {
                if (!graph_type::isDigraph() && isPopped_[*iter]) { // 对于无向图，若某顶点已出栈，则与之邻接的边必然已遍历
                    ++iter;
                    continue;
                }
            } 
            else {
                if (isPushed_[*iter]) { // 跳过已遍历的顶点，确保每个顶点只遍历一次
                    ++iter;
                    continue;
                }
            }

            break;
        }

        if (todo_.empty()) {
            isPopped_[v0_] = true;
            v0_ = null_vertex;  // 设置终止标记
        }

        if (fullGraph && isEnd()) {
            auto pos = std::find(isPushed_.begin(), isPushed_.end(), false);
            if (pos != isPushed_.end()) 
                start(static_cast<vertex_index_t>(std::distance(isPushed_.begin(), pos)));
        }
    }

    // 返回当前正在游历的顶点
    vertex_index_t operator*() const {
        return todo_.empty() ? v0_ : *(todo_.front());
    }


    // 与当前顶点（to顶点）构成边的from顶点
    vertex_index_t from() const {
        assert(!isEnd());
        return todo_.empty() ? null_vertex : todo_.front().from();
    }


    const_edge_ref edge() const { return todo_.front().edge(); }

    bool isEnd() const { return v0_ == null_vertex; }


    // 从顶点v开始接续进行广度优先遍历
    void start(vertex_index_t v) {
        assert(isEnd() && !isPushed_[v]);
        v0_ = v;

        if (modeEdge) ++(*this); // skip v0
    }


    bool isPushed(vertex_index_t v) const { return isPushed_[v]; }
    bool isPopped(vertex_index_t v) const { return isPopped_[v]; }

private:
    graph_type& graph_;

    // 待处理的邻接顶点迭代器
    std::queue<adj_vertex_iter> todo_;

    vertex_index_t v0_; // 起始顶点
    std::vector<bool> isPushed_, isPopped_;  // 用于标记顶点是否已压栈/出栈
};
