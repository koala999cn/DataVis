#pragma once
#include <queue>
#include <assert.h>
#include "KtAdjIter.h"


// ������ȱ���
// ģ�������
//    -- fullGraph����Ϊtrue�������ȫͼ�����������������v����ͨ�Ľڵ��ߡ�
//    -- modeEdge����Ϊtrue����Ա߽��й�����ȱ�������ʱ������ܻ����ظ�����ÿ����ֻ�����һ�Ρ�
//                 ��Ϊfalse����Զ�����й�����ȱ�������ʱ���ܱ�֤ÿ���߶����������ֻ����һ�Ρ�
// ��modeEdgeģʽ�µĳ�ʼ״̬��operator*()����startVertex��from()��������-1
// modeEdgeģʽ�µĳ�ʼ״̬��operator*()����startVertex�ĵ�һ���ڽӶ��㣬from()��������startVertex
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


    // graph -- ��������ͼ
    // startVertex -- ��������ʼ���㣬-1��ʾֻ��������������Ҫ�������start������ʼ����
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

            // �Ƴ��ѵ�ĩβ�ĵ�����
            if (iter.isEnd()) {
                isPopped_[from()] = true;
                todo_.pop();
                continue;
            }

            // �����ѱ����Ķ�����
            if (modeEdge) {
                if (!graph_type::isDigraph() && isPopped_[*iter]) { // ��������ͼ����ĳ�����ѳ�ջ������֮�ڽӵı߱�Ȼ�ѱ���
                    ++iter;
                    continue;
                }
            } 
            else {
                if (isPushed_[*iter]) { // �����ѱ����Ķ��㣬ȷ��ÿ������ֻ����һ��
                    ++iter;
                    continue;
                }
            }

            break;
        }

        if (todo_.empty()) {
            isPopped_[v0_] = true;
            v0_ = null_vertex;  // ������ֹ���
        }

        if (fullGraph && isEnd()) {
            auto pos = std::find(isPushed_.begin(), isPushed_.end(), false);
            if (pos != isPushed_.end()) 
                start(static_cast<vertex_index_t>(std::distance(isPushed_.begin(), pos)));
        }
    }

    // ���ص�ǰ���������Ķ���
    vertex_index_t operator*() const {
        return todo_.empty() ? v0_ : *(todo_.front());
    }


    // �뵱ǰ���㣨to���㣩���ɱߵ�from����
    vertex_index_t from() const {
        assert(!isEnd());
        return todo_.empty() ? null_vertex : todo_.front().from();
    }


    const_edge_ref edge() const { return todo_.front().edge(); }

    bool isEnd() const { return v0_ == null_vertex; }


    // �Ӷ���v��ʼ�������й�����ȱ���
    void start(vertex_index_t v) {
        assert(isEnd() && !isPushed_[v]);
        v0_ = v;

        if (modeEdge) ++(*this); // skip v0
    }


    bool isPushed(vertex_index_t v) const { return isPushed_[v]; }
    bool isPopped(vertex_index_t v) const { return isPopped_[v]; }

private:
    graph_type& graph_;

    // ��������ڽӶ��������
    std::queue<adj_vertex_iter> todo_;

    vertex_index_t v0_; // ��ʼ����
    std::vector<bool> isPushed_, isPopped_;  // ���ڱ�Ƕ����Ƿ���ѹջ/��ջ
};
