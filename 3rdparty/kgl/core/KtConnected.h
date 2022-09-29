#pragma once
#include "KtBfsIter.h"

// 无向图的连通分量

template<typename GRAPH>
class KtConnected
{
    static_assert(!GRAPH::isDigraph(), "KtConnected cannot work for digraph.");

public:
    KtConnected(const GRAPH& g) 
        : count_(0),
          cc_(g.order(), -1) {

        KtBfsIter<const GRAPH, true> bfs(g, 0);
        unsigned id(-1);
        for (; !bfs.isEnd(); ++bfs) {
            if (bfs.from() == -1)
                ++id;
            cc_[*bfs] = id;
        }

        count_ = id + 1;
    }


    // 返回连通分量数量
    auto count() const { return count_; }

    // 顶点v和顶点w是否连通
    bool reachable(unsigned v, unsigned w) const {
        return cc_[v] == cc_[w];;
    }

    // 返回节点v所在连通分量的id, 0 <= id < count().
    unsigned operator[](unsigned v) const {
        return cc_[v];
    }


private:
    unsigned count_; // 连通分量的数量
    std::vector<unsigned> cc_; // cc_[i]表示顶点i对应的连通分量序号
};
