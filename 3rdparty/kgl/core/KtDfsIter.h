#pragma once
#include <vector>
#include <set>
#include <assert.h>


// 统一有向图与无向图的深度优先遍历模板框架，通过模板参数可支持多种遍历范式。
// 模板参数：
//    -- stopAtPopping，若为true，则每个顶点出栈时迭代器暂停，用户有机会处理出栈顶点.
//    -- fullGraph，参见KtBfsIter.
//    -- modeEdge，迭代模式，false为顶点迭代模式，true为边迭代模式
//    -- stopAtPopping，是否处理出栈情况
// 边模式下，若stopAtPopping为false，初始状态即为有效边， 此时from()返回起始顶点
// 边模式下，若stopAtPopping为true，初始状态仍返回起始顶点，此时from()返回-1，*操作符返回起始顶点.
template<typename GRAPH, bool fullGraph = false, bool modeEdge = false, bool stopAtPopping = false>
class KtDfsIter
{
public:
    using graph_type = GRAPH;
    using vertex_index_t = typename graph_type::vertex_index_t;
    using edge_type = typename GRAPH::edge_type;
    using adj_vertex_iter = KtAdjIter<graph_type>;
    using const_edge_ref = decltype(std::declval<adj_vertex_iter>().edge());
    constexpr static vertex_index_t null_vertex = -1;

    constexpr static bool trace_multi_edges = !GRAPH::isDigraph() && GRAPH::isMultiEdges(); // 对于无向平行图，dfs须追踪平行边
    using tracing_element_t = std::tuple<vertex_index_t, vertex_index_t, const_edge_ref>;
    using tracing_container_t = std::multiset<tracing_element_t>;


    // graph -- 待遍历的图
    // startVertex -- 遍历的起始顶点，-1表示只构建迭代器，需要另外调用start方法开始遍历
    KtDfsIter(GRAPH& graph, vertex_index_t startVertex)
        : graph_(graph)
        , v_(null_vertex)
        , pushOrd_(graph.order(), -1)
        , popOrd_(graph.order(), -1)
        , pushIdx_(0)
        , popIdx_(0) {
        if (startVertex != null_vertex) 
            start(startVertex);

        if constexpr (trace_multi_edges)
            collectMultiEdges_();
    }

    void operator++() {
        assert(!isEnd());

        if (isPopping()) { // 处理出栈顶点
            assert(popOrd_[v_] == -1);
            popOrd_[v_] = popIdx_++;
            todo_.pop_back();
        }
        else {
            if (todo_.size() > 1) {
                if constexpr (trace_multi_edges) 
                    markMultiEdge_();

                ++todo_.back();
            }

            if (isPushing()) {
                pushOrd_[v_] = pushIdx_++;
                todo_.push_back(adj_vertex_iter(graph_, v_));
            }
        }
        
        fixStack_();
    }

    // 返回当前正在游历的顶点
    vertex_index_t operator*() const { return v_; }


    // 与当前顶点（to顶点）构成边的from顶点
    vertex_index_t from() const {
        assert(!isEnd());
        return isPopping() ? grandpa_() :
                            todo_.size() > 1 ? todo_.back().from() : null_vertex;
    }


    // 返回边(from, to)的值
    // TODO: 暂不支持修改权值。若支持，须同步更新pedges_
    const_edge_ref edge() const {
        assert(!isEnd() && from() != null_vertex);
        return todo_.back().edge();
    }


    bool isEnd() const { return v_ == null_vertex; }


    // 从顶点v开始接续进行广度优先遍历
    void start(vertex_index_t v) {
        assert(isEnd() && pushOrd_[v] == -1);
        todo_.clear();
        todo_.push_back(adj_vertex_iter(graph_));
        v_ = v;
        if constexpr (modeEdge && !stopAtPopping) 
            ++(*this); // skip v0
    }


    void erase() {
        assert(!isEnd() && from() != null_vertex);

        if (isPopping()) { // 处理出栈顶点
            assert(popOrd_[v_] == -1);
            popOrd_[v_] = popIdx_++;
            todo_.pop_back();
        }
        else {
            if constexpr (trace_multi_edges)
                markMultiEdge_();
        }

        todo_.back().erase();

        fixStack_();
    }


    /*
    BGL中关于树边、回边、下边、跨边的定义：
    - Tree edges are edges in the search tree(or forest) constructed by running a graph search algorithm over a graph. 
      An edge(u, v) is a tree edge if v was first discovered while exploring edge(u, v).
    - Back edges connect vertices to their ancestors in a search tree. 
      So for edge(u, v) the vertex v must be the ancestor of vertex u.
      Self loops are considered to be back edges.
    - Forward edges are non-tree edges(u, v) that connect a vertex u to a descendant v in a search tree.
    - Cross edges are edges that do not fall into the above three categories.
    */

    // 树边，表示递归调用（即第一次访问该节点）
    bool isTree() const {
        return pushOrd_[v_] == -1;
    }


    // 回边，表示当前节点是前序节点的祖先
    bool isBack() const {
        return !isTree() && !isPopping() && popOrd_[v_] == -1;
    }


    // 下边/前边，表示当前节点是前序节点的子孙
    bool isDown() const {
        //return !isTree() && !isBack() && pushOrd_[**this] > pushOrd_[from()];
        assert(pushOrd_[from()] != -1);
        return static_cast<int>(pushOrd_[v_]) > static_cast<int>(pushOrd_[from()]);
    }


    // 跨边，表示当前节点既不是前序节点的祖先，也不是子孙
    bool isCross() const {
        //return !isTree() && !isBack() && !isDown();
        return GRAPH::isDigraph() && popOrd_[v_] != -1; // 只有有向图才有跨边
    }

    // 当前节点是否正在入栈，对应于递归的入口
    bool isPushing() const { return isTree(); }

    // 当前节点是否正在出栈，对应于递归的出口
    bool isPopping() const { 
        return stopAtPopping && !isPushing() && todo_.back().isEnd(); 
    }

    // 获取顶点v的入栈/出栈次序，用于事后检测
    unsigned pushIndex(vertex_index_t v) const { return pushOrd_[v]; }
    unsigned popIndex(vertex_index_t v) const { return popOrd_[v]; }

    // 获取当前的入栈/出栈序号
    unsigned pushingIndex() const { return pushIdx_; }
    unsigned poppingIndex() const { return popIdx_; }


    vertex_index_t firstUnvisited() const {
        auto pos = std::find(pushOrd_.begin(), pushOrd_.end(), -1);
        return pos == pushOrd_.end() ? null_vertex
            : static_cast<vertex_index_t>(std::distance(pushOrd_.begin(), pos));
    }


private:

    // 返回当前顶点的祖父顶点，即from之from
    vertex_index_t grandpa_() const {
        return todo_.size() > 2 ? todo_[todo_.size() - 2].from() : null_vertex;
    }


    // 步进或删除的后处理
    void fixStack_();

    // 测试是否需要跳过当前顶点/边
    bool testSkip_() const;

    // 搜集graph_中的所有平行边
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    void collectMultiEdges_();
      
    // 标记平行边<from, to, wt>已遍历
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    void markMultiEdge_();

    // 测试平行边<from, to, wt>遍历状态，返回true表示已遍历
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    bool testMultiEdge_() const;

    // 辅助函数：定位当前边在graph_中的位置
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    typename tracing_container_t::const_iterator findMultiEdges_() const {

        auto adj = todo_.back();
        auto v = adj.from(), w = *adj;
        if (v > w) std::swap(v, w);

        return pedges_.find({ v, w, adj.edge() });
    }

private:
    graph_type& graph_;

    // 待处理的邻接顶点迭代器
    std::vector<adj_vertex_iter> todo_;

    vertex_index_t v_; // 正在遍历的顶点

    std::vector<unsigned> pushOrd_, popOrd_; // 用于记录各顶点的压栈/出栈顺序
    unsigned pushIdx_, popIdx_; // 当前压栈/出栈序号

    tracing_container_t pedges_; // 存储graph_所有未遍历的平行边
};


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
void KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::fixStack_()
{
    // 检测当前顶点是否需要跳过
    while (todo_.size() > 1) {
        auto& iter = todo_.back();

        // 移除已结束的迭代器
        if (iter.isEnd()) {
            if constexpr (stopAtPopping) {
                v_ = iter.from();
                return; // stop at popping
            }
 
            popOrd_[iter.from()] = popIdx_++;
            todo_.pop_back();
            continue;
        }

        assert(!isPopping() && !iter.isEnd());

        if (testSkip_()) {
            ++iter;
            continue;
        }

        break;
    }

    // 更新v_和接续遍历
    if (todo_.size() <= 1) {
        v_ = null_vertex; // 设置终止标记

        if constexpr (fullGraph) {
            unsigned unvisted = firstUnvisited();
            if (unvisted != null_vertex)
                start(unvisted); // 接续遍历
        }
    }
    else {
        assert(!todo_.back().isEnd());
        v_ = *todo_.back();
    }
}


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
bool KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::testSkip_() const
{
    auto v = *todo_.back();

    if constexpr (!modeEdge) // 对于顶点模式，每个顶点只遍历一次
        return pushOrd_[v] != -1;

    // 处理无向图的边迭代模式
    if constexpr (!GRAPH::isDigraph()) {

        if (popOrd_[v] != -1) // 对于无向图，若某顶点已出栈，则与之邻接的边必然已遍历
            return true;

        if (v == grandpa_()) { // 防止无向图的顶点回溯，即防止已遍历的无向边(v, w)再次通过(w, v)遍历
            if constexpr (trace_multi_edges)
                if (!testMultiEdge_())
                    return false;

            return true;
        }
    }

    return false;
}


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
template<bool dummy, typename>
void KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::collectMultiEdges_()
{
    assert(pedges_.empty());
    for (vertex_index_t v = 0; v < graph_.order(); v++) 
        for (vertex_index_t w = v; w < graph_.order(); w++) {
            auto r = graph_.edges(v, w);
            if (r.size() > 1) {
                for (; !r.empty(); ++r)
                    pedges_.emplace(v, w, *r);
            }
        }

}


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
template<bool dummy, typename>
void KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::markMultiEdge_()
{
    auto it = findMultiEdges_();
    if (it != pedges_.cend())
        pedges_.erase(it);
}


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
template<bool dummy, typename>
bool KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::testMultiEdge_() const
{
    auto it = findMultiEdges_();
    return it == pedges_.cend();
}
