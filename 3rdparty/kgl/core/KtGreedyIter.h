#pragma once
#include "../GraphX.h"
#include "../util/copy.h"


// ̰��ģʽ�µıߵ�����
// ��KtDfsIter��������Ҫ���ڶԻ��Ĵ���
//  -- KtDfsIter������ʱ�����Ὣ���ıպ϶�����ջ��ʼ�ձ��ֶ�ջ�и������Ψһ��
//  -- KtGreedyIter������ʱ���Ὣ�պ϶�����ջ���������Ÿö��������������������պ϶����ڶ�ջ�д��ڶ��ʵ��

template<typename GRAPH, bool fullGraph = false, bool stopAtPopping = false>
class KtGreedyIter
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;
    using edge_type = typename GRAPH::edge_type;
	using flat_graph_t = KtGraph<KtFlatGraphVectorImpl<edge_type>, // ���Զ������
		GRAPH::isDigraph(), GRAPH::isMultiEdges(), GRAPH::isAlwaysSorted()>;
    using const_edge_ref = decltype(std::declval<const flat_graph_t>().edgeAt(0));

    // ��ջԪ�����ͣ�<0>Ϊfrom���㣬<1><2>Ϊ������edge_index��range
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


    // ���ص�ǰ���������Ķ���
    vertex_index_t operator*() const { 
        return isPopping() ? std::get<0>(todo_.back())
                           : edge_traits<typename graph_traits<flat_graph_t>::underly_edge_t>::to(edge());
    }


    // �뵱ǰ���㣨to���㣩���ɱߵ�from����
    vertex_index_t from() const {
        assert(!isEnd());
        if (isPopping())
            return todo_.size() > 1 ? std::get<0>(todo_[todo_.size() - 2]) : -1;
        else 
            return std::get<0>(todo_.back());
    }


    // ���ر�(from, to)��ֵ
    const_edge_ref edge() const {
        assert(!isEnd());
        return graph_.edgeAt(std::get<1>(todo_.back()));
    }


    bool isEnd() const { return todo_.empty(); }


    // �Ӷ���v��ʼ�������й�����ȱ���
    void start(vertex_index_t v) {
        assert(todo_.empty());
        inStack_(v);
    }


    template<bool dummy = stopAtPopping, typename = std::enable_if_t<dummy>>
    bool isPopping() const {
        return testStack_();
    }

private:

    // �������������Զ�ջ������tuple�Ƿ�<1> == <2>
    bool testStack_() const {
        return std::get<1>(todo_.back()) == std::get<2>(todo_.back());
    }

    // ��ջ
    void inStack_(vertex_index_t v) {
        auto r = graph_.outedges(v);
        auto edgeidx = graph_.edgeIndex(v);
        todo_.push_back({ v,  edgeidx, static_cast<vertex_index_t>(edgeidx + r.size()) });
    }

    void fixStack_() {

        // ��⵱ǰ�����Ƿ���Ҫ����
        while (!todo_.empty()) {

            // �Ƴ��ѽ����ĵ�����
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

        // ��������
        if constexpr (fullGraph) {
            if (todo_.empty()) {
                unsigned edgeidx = firstUnvisited_();
                if (edgeidx != -1)
                    start(graph_.edgeFrom(edgeidx)); // ��������
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


    // ��ǵ�edgeidx��Ϊ�ѱ���
    void markEdge_(unsigned edgeidx) {
        assert(!vedges_[edgeidx]);

        vedges_[edgeidx] = true; // ����Ϊ�ѱ���
        auto edgefrom = from();
        auto edgeto = **this;

        // ��������ͼ�������Ƿ����
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

