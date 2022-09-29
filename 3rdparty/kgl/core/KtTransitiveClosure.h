#pragma once
#include <type_traits>
#include <assert.h>
#include "../GraphX.h"
#include "KtDfsIter.h"
#include "KtStronglyConnected.h"


// 计算图的闭包的基类
// @useSpare: 是否使用稀疏图保存闭包结果
// TODO: 闭包结果不包含路径信息和权值信息
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


    // 判断闭包图中v是否可达w
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
    closure_graph_t clsg_; // 闭包图
};


/*
  基于Warshall算法的闭包实现.
  Warshall算法将基于布尔矩阵的传递闭包实现时间复杂度从V ^ 3 * log2V降低为V ^ 3
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
                if (v == w || g.hasEdge(v, w)) // 用谓词初始化，并确保自环
                    set_(v, w);

        // Warshall算法
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
  基于DFS的闭包实现.
  它以G的各个顶点作为开始完成一个单独的DFS，计算其可达的顶点集，以此计算传递闭包。
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
  DAG的传递闭包，基于DFS算法实现。
  相比KtClosureDfs，主要利用DAG的特性进行优化.
    1.没有回边
    2.下边可以忽略，即不用递归展开，也不用回溯传递闭包
    3.跨边不用递归展开，直接回溯即可
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

            assert(!iter.isBack()); // DAG没有回边

            if (iter.isCross() || iter.isPopping()) {
                auto iter = super_::adjIter_(v);
                for (; !iter.isEnd(); ++iter)
                    set_(p, *iter);
            }
        }
    }
};


/*
 基于强分量的传递闭包实现.
 */

template<typename GRAPH, bool useSpare = false>
class KtTcScc
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtTcScc(const GRAPH& g) : scc_(g) {
        // 以每个强连通分量为一个顶点构建DAG
        DigraphDx<bool> K(scc_.count());
        for (unsigned v = 0; v < g.order(); v++) {
            auto iter = KtAdjIter(g, v);
            while (!iter.isEnd()) {
                auto x = scc_[v];
                auto y = scc_[*iter];
                if (x != y && !K.hasEdge(x, y)) // 消除自环
                    K.addEdge(x, y);
                ++iter;
            }
        }

        // 使用基于DAG的传递闭包优化算法
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
