#pragma once
#include <queue>
#include <assert.h>
#include "KtAdjIter.h"


// 基于优先队列的图遍历



// PRIORITOR - 计算优先度的函数子，输入参数为：from, to, value
// COMP - 比较优先度的函数子
template <typename GRAPH, typename PRIORITOR, typename COMP, bool fullGraph = false>
class KtPfsIter
{
public:
    using edge_type = typename GRAPH::edge_type;
    using prior_type = decltype(std::declval<PRIORITOR>()(0, 0, edge_type(0))); // 得到PRIORITOR算子的返回类型
    using element_type = std::pair<std::pair<unsigned, unsigned>, prior_type>;

    struct Comp {
        bool operator()(const element_type& a, const element_type& b) {
            return COMP{}(b.second, a.second);
        }
    };

    using pfs_queue = std::priority_queue<element_type, std::vector<element_type>, Comp>;

public:
    KtPfsIter(GRAPH& g, unsigned v0)
        : graph_(g)
        , v0_(v0)
        , isPushed_(g.order(), false)
        , st_(g.order(), -1) {
        start(v0);
    }


    void operator++() {
        assert(!isEnd());
        auto x = pq_.top().first; pq_.pop();
        unsigned w = x.second;
        st_[w] = x.first;

        auto iter= KtAdjIter(graph_, w);
        for (; !iter.isEnd(); ++iter) {
            unsigned t = *iter;
            if (!isPushed(t)) {
                pq_.emplace(std::pair<unsigned, unsigned>{w, t}, PRIORITOR{}(w, t, iter.edge())); // put it
                isPushed_[t] = true;
             }
            else if(!isPopped(t) // isPopped(v0)始终为false
                && (graph_.isDigraph() || t != x.first && t != v0_) // 忽略无向图的回边
                ) 
                pq_.emplace(std::pair<unsigned, unsigned>{w, t}, PRIORITOR{}(w, t, iter.edge())); // update it
        }


        if (fullGraph && isEnd()) {
            auto pos = std::find(isPushed_.begin(), isPushed_.end(), false);
            if (pos != isPushed_.end()) 
                start(unsigned(std::distance(isPushed_.begin(), pos)));
        }
    }

    // 返回当前正在游历的顶点
    unsigned operator*() const {
        assert(!isEnd());
        return pq_.top().first.second;
    }


    // 与当前顶点（to顶点）构成边的from顶点
    unsigned from() const {
        assert(!isEnd());
        return pq_.top().first.first;
    }


    auto edge() const {
        assert(!isEnd());
        return pq_.top().first;
    }


    auto prior() const {
        assert(!isEnd());
        return pq_.top().second;
    }


    unsigned from(unsigned w) const {
        return st_[w];
    }


    bool isEnd() const { return pq_.empty(); }


    // 从顶点v开始接续进行广度优先遍历
    void start(unsigned v) {
        assert(isEnd() && !isPushed(v));
        pq_.emplace(std::pair<unsigned, unsigned>{-1, v}, 0);
        isPushed_[v] = true;
    }


    bool isPushed(unsigned v) const { return isPushed_[v]; }
    bool isPopped(unsigned v) const { return st_[v] != -1; }


private:
    GRAPH& graph_;
    unsigned v0_;
    std::vector<bool> isPushed_;

    pfs_queue pq_; // 边缘带
    std::vector<unsigned> st_; // 树
};

