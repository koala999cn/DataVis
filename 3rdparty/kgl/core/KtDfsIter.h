#pragma once
#include <vector>
#include <set>
#include <assert.h>


// ͳһ����ͼ������ͼ��������ȱ���ģ���ܣ�ͨ��ģ�������֧�ֶ��ֱ�����ʽ��
// ģ�������
//    -- stopAtPopping����Ϊtrue����ÿ�������ջʱ��������ͣ���û��л��ᴦ���ջ����.
//    -- fullGraph���μ�KtBfsIter.
//    -- modeEdge������ģʽ��falseΪ�������ģʽ��trueΪ�ߵ���ģʽ
//    -- stopAtPopping���Ƿ����ջ���
// ��ģʽ�£���stopAtPoppingΪfalse����ʼ״̬��Ϊ��Ч�ߣ� ��ʱfrom()������ʼ����
// ��ģʽ�£���stopAtPoppingΪtrue����ʼ״̬�Է�����ʼ���㣬��ʱfrom()����-1��*������������ʼ����.
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

    constexpr static bool trace_multi_edges = !GRAPH::isDigraph() && GRAPH::isMultiEdges(); // ��������ƽ��ͼ��dfs��׷��ƽ�б�
    using tracing_element_t = std::tuple<vertex_index_t, vertex_index_t, const_edge_ref>;
    using tracing_container_t = std::multiset<tracing_element_t>;


    // graph -- ��������ͼ
    // startVertex -- ��������ʼ���㣬-1��ʾֻ��������������Ҫ�������start������ʼ����
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

        if (isPopping()) { // �����ջ����
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

    // ���ص�ǰ���������Ķ���
    vertex_index_t operator*() const { return v_; }


    // �뵱ǰ���㣨to���㣩���ɱߵ�from����
    vertex_index_t from() const {
        assert(!isEnd());
        return isPopping() ? grandpa_() :
                            todo_.size() > 1 ? todo_.back().from() : null_vertex;
    }


    // ���ر�(from, to)��ֵ
    // TODO: �ݲ�֧���޸�Ȩֵ����֧�֣���ͬ������pedges_
    const_edge_ref edge() const {
        assert(!isEnd() && from() != null_vertex);
        return todo_.back().edge();
    }


    bool isEnd() const { return v_ == null_vertex; }


    // �Ӷ���v��ʼ�������й�����ȱ���
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

        if (isPopping()) { // �����ջ����
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
    BGL�й������ߡ��رߡ��±ߡ���ߵĶ��壺
    - Tree edges are edges in the search tree(or forest) constructed by running a graph search algorithm over a graph. 
      An edge(u, v) is a tree edge if v was first discovered while exploring edge(u, v).
    - Back edges connect vertices to their ancestors in a search tree. 
      So for edge(u, v) the vertex v must be the ancestor of vertex u.
      Self loops are considered to be back edges.
    - Forward edges are non-tree edges(u, v) that connect a vertex u to a descendant v in a search tree.
    - Cross edges are edges that do not fall into the above three categories.
    */

    // ���ߣ���ʾ�ݹ���ã�����һ�η��ʸýڵ㣩
    bool isTree() const {
        return pushOrd_[v_] == -1;
    }


    // �رߣ���ʾ��ǰ�ڵ���ǰ��ڵ������
    bool isBack() const {
        return !isTree() && !isPopping() && popOrd_[v_] == -1;
    }


    // �±�/ǰ�ߣ���ʾ��ǰ�ڵ���ǰ��ڵ������
    bool isDown() const {
        //return !isTree() && !isBack() && pushOrd_[**this] > pushOrd_[from()];
        assert(pushOrd_[from()] != -1);
        return static_cast<int>(pushOrd_[v_]) > static_cast<int>(pushOrd_[from()]);
    }


    // ��ߣ���ʾ��ǰ�ڵ�Ȳ���ǰ��ڵ�����ȣ�Ҳ��������
    bool isCross() const {
        //return !isTree() && !isBack() && !isDown();
        return GRAPH::isDigraph() && popOrd_[v_] != -1; // ֻ������ͼ���п��
    }

    // ��ǰ�ڵ��Ƿ�������ջ����Ӧ�ڵݹ�����
    bool isPushing() const { return isTree(); }

    // ��ǰ�ڵ��Ƿ����ڳ�ջ����Ӧ�ڵݹ�ĳ���
    bool isPopping() const { 
        return stopAtPopping && !isPushing() && todo_.back().isEnd(); 
    }

    // ��ȡ����v����ջ/��ջ���������º���
    unsigned pushIndex(vertex_index_t v) const { return pushOrd_[v]; }
    unsigned popIndex(vertex_index_t v) const { return popOrd_[v]; }

    // ��ȡ��ǰ����ջ/��ջ���
    unsigned pushingIndex() const { return pushIdx_; }
    unsigned poppingIndex() const { return popIdx_; }


    vertex_index_t firstUnvisited() const {
        auto pos = std::find(pushOrd_.begin(), pushOrd_.end(), -1);
        return pos == pushOrd_.end() ? null_vertex
            : static_cast<vertex_index_t>(std::distance(pushOrd_.begin(), pos));
    }


private:

    // ���ص�ǰ������游���㣬��from֮from
    vertex_index_t grandpa_() const {
        return todo_.size() > 2 ? todo_[todo_.size() - 2].from() : null_vertex;
    }


    // ������ɾ���ĺ���
    void fixStack_();

    // �����Ƿ���Ҫ������ǰ����/��
    bool testSkip_() const;

    // �Ѽ�graph_�е�����ƽ�б�
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    void collectMultiEdges_();
      
    // ���ƽ�б�<from, to, wt>�ѱ���
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    void markMultiEdge_();

    // ����ƽ�б�<from, to, wt>����״̬������true��ʾ�ѱ���
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    bool testMultiEdge_() const;

    // ������������λ��ǰ����graph_�е�λ��
    template<bool dummy = trace_multi_edges, typename = std::enable_if_t<dummy>>
    typename tracing_container_t::const_iterator findMultiEdges_() const {

        auto adj = todo_.back();
        auto v = adj.from(), w = *adj;
        if (v > w) std::swap(v, w);

        return pedges_.find({ v, w, adj.edge() });
    }

private:
    graph_type& graph_;

    // ��������ڽӶ��������
    std::vector<adj_vertex_iter> todo_;

    vertex_index_t v_; // ���ڱ����Ķ���

    std::vector<unsigned> pushOrd_, popOrd_; // ���ڼ�¼�������ѹջ/��ջ˳��
    unsigned pushIdx_, popIdx_; // ��ǰѹջ/��ջ���

    tracing_container_t pedges_; // �洢graph_����δ������ƽ�б�
};


template<typename GRAPH, bool fullGraph, bool modeEdge, bool stopAtPopping>
void KtDfsIter<GRAPH, fullGraph, modeEdge, stopAtPopping>::fixStack_()
{
    // ��⵱ǰ�����Ƿ���Ҫ����
    while (todo_.size() > 1) {
        auto& iter = todo_.back();

        // �Ƴ��ѽ����ĵ�����
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

    // ����v_�ͽ�������
    if (todo_.size() <= 1) {
        v_ = null_vertex; // ������ֹ���

        if constexpr (fullGraph) {
            unsigned unvisted = firstUnvisited();
            if (unvisted != null_vertex)
                start(unvisted); // ��������
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

    if constexpr (!modeEdge) // ���ڶ���ģʽ��ÿ������ֻ����һ��
        return pushOrd_[v] != -1;

    // ��������ͼ�ıߵ���ģʽ
    if constexpr (!GRAPH::isDigraph()) {

        if (popOrd_[v] != -1) // ��������ͼ����ĳ�����ѳ�ջ������֮�ڽӵı߱�Ȼ�ѱ���
            return true;

        if (v == grandpa_()) { // ��ֹ����ͼ�Ķ�����ݣ�����ֹ�ѱ����������(v, w)�ٴ�ͨ��(w, v)����
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
