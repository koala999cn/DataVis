#pragma once
#include <type_traits>
#include <assert.h>
#include "../GraphX.h"
#include "KtDfsIter.h"
#include "KtStronglyConnected.h"


// ����ͼ�ıհ��Ļ���
// @useSpare: �Ƿ�ʹ��ϡ��ͼ����հ����
// TODO: �հ����������·����Ϣ��Ȩֵ��Ϣ
template<typename GRAPH, bool useSpare>
class KtTcAbstract
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;
    using closure_graph_t = std::conditional_t<useSpare, DigraphSx<bool>, DigraphDx<bool>>;

    explicit KtTcAbstract(const GRAPH& g) : clsg_(g.order()){}

    KtTcAbstract(const KtTcAbstract& c) : clsg_(c.clsg) {}
    KtTcAbstract(KtTcAbstract&& c) : clsg_(std::move(c.clsg_)) {}

    KtTcAbstract& operator =(const KtTcAbstract& rhs) {
        clsg_ = rhs.clsg_;
        return *this;
    }

    KtTcAbstract& operator =(KtTcAbstract&& rhs) {
        clsg_ = std::move(rhs.clsg_);
        return *this;
    }


    // �жϱհ�ͼ��v�Ƿ�ɴ�w
    bool reachable(vertex_index_t v, vertex_index_t w) const {
        return clsg_.hasEdge(v, w);
    }

    auto operator->() const { return &clsg_; }

protected:

    void set_(vertex_index_t v, vertex_index_t w) {
        if (reachable(v, w))
            clsg_.addEdge(v, w);
    }

    auto adjIter_(unsigned v) const {
        return KtAdjIter(clsg_, v);
    }

private:
    closure_graph_t clsg_; // �հ�ͼ
};


/*
  ����Warshall�㷨�ıհ�ʵ��.
  Warshall�㷨�����ڲ�������Ĵ��ݱհ�ʵ��ʱ�临�Ӷȴ�V ^ 3 * log2V����ΪV ^ 3
 */

template<typename GRAPH, bool useSpare = false>
class KtTcWarshall : public KtTcAbstract<GRAPH, useSpare>
{
public:
    using super_ = KtTcAbstract<GRAPH, useSpare>;
    using super_::reachable;
    using super_::set_;

    KtTcWarshall(const GRAPH& g) : super_(g) {

        auto V = g.order();
        for (unsigned v = 0; v < V; v++) 
            for (unsigned w = 0; w < V; w++)
                if (v == w || g.hasEdge(v, w)) // ��ν�ʳ�ʼ������ȷ���Ի�
                    set_(v, w);

        // Warshall�㷨
        for (unsigned i = 0; i < V; i++)
            for (unsigned s = 0; s < V; s++)
                if (reachable(s, i)) {
                    auto iter = super_::adjIter_(i);
                    for (; !iter.isEnd(); ++iter)
                        set_(s, *iter);
                }
    }
};


/*
  ����DFS�ıհ�ʵ��.
  ����G�ĸ���������Ϊ��ʼ���һ��������DFS��������ɴ�Ķ��㼯���Դ˼��㴫�ݱհ���
 */

template<typename GRAPH, bool useSpare = false>
class KtTcDfs : public KtTcAbstract<GRAPH, useSpare>
{
public:
    using super_ = KtTcAbstract<GRAPH, useSpare>;
    using super_::reachable;
    using super_::set_;

    KtTcDfs(const GRAPH& g) : super_(g) {
        unsigned V = g.order();
        for (unsigned i = 0; i < V; i++)
            set_(i, i);

        for (unsigned v = 0; v < V; v++) {
            KtDfsIter<const GRAPH> iter(g, v);
            assert(iter.from() == -1);
            ++iter; // skip edge(-1, v)
            for (; !iter.isEnd(); ++iter)
                if (iter.isTree())
                    set_(v, *iter);
        }
    }
};


/*
  DAG�Ĵ��ݱհ�������DFS�㷨ʵ�֡�
  ���KtClosureDfs����Ҫ����DAG�����Խ����Ż�.
    1.û�лر�
    2.�±߿��Ժ��ԣ������õݹ�չ����Ҳ���û��ݴ��ݱհ�
    3.��߲��õݹ�չ����ֱ�ӻ��ݼ���
 */

template<typename GRAPH, bool useSpare = false>
class KtTcDag : public KtTcAbstract<GRAPH, useSpare>
{
public:
    using super_ = KtTcAbstract<GRAPH, useSpare>;
    using super_::reachable;
    using super_::set_;

    KtTcDag(const GRAPH& g) : super_(g) {
        //assert(g.isDag());

        unsigned V = g.order();
        for (unsigned i = 0; i < V; i++)
            set_(i, i);

        KtDfsIter<const GRAPH, true, true, true> iter(g, 0);
        for (; !iter.isEnd(); ++iter) {
            unsigned p = iter.from();
            if (p == -1)
                continue; // skip edge(-1, v)

            unsigned v = *iter;
            if (!iter.isPopping())
                set_(p, v);

            assert(!iter.isBack()); // DAGû�лر�

            if (iter.isCross() || iter.isPopping()) {
                auto iter = super_::adjIter_(v);
                for (; !iter.isEnd(); ++iter)
                    set_(p, *iter);
            }
        }
    }
};


/*
 ����ǿ�����Ĵ��ݱհ�ʵ��.
 */

template<typename GRAPH, bool useSpare = false>
class KtTcScc
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtTcScc(const GRAPH& g) : scc_(g) {
        // ��ÿ��ǿ��ͨ����Ϊһ�����㹹��DAG
        DigraphDx<bool> K(scc_.count());
        for (unsigned v = 0; v < g.order(); v++) {
            auto iter = KtAdjIter(g, v);
            while (!iter.isEnd()) {
                auto x = scc_[v];
                auto y = scc_[*iter];
                if (x != y && !K.hasEdge(x, y)) // �����Ի�
                    K.addEdge(x, y);
                ++iter;
            }
        }

        // ʹ�û���DAG�Ĵ��ݱհ��Ż��㷨
        dagCls_ = new KtTcDag<DigraphDx<bool>>(K);
    }

    ~KtTcScc() { delete dagCls_; }

    bool reachable(vertex_index_t v, vertex_index_t w) const {
        return dagCls_->reachable(scc_[v], scc_[w]);
    }

private:
    KtStronglyConnectedTar<GRAPH> scc_;
    KtTcDag<DigraphDx<bool>, useSpare>* dagCls_;
};
