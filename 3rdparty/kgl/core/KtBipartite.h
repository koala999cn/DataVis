#pragma once
#include <assert.h>
#include "KtDfsIter.h"


// 二分图判定
// 若返回true，表示THIS是二分图，res元素值为1或者2，分别代表对应节点在不同分图
// 若返回false，表示THIS非二分图，res值等于-1的元素为冲突顶点
template<typename GRAPH>
class KtBipartite
{
    static_assert(!GRAPH::isDigraph(), "KtBipartite cannot instantiated with Digraph.");

public:

    // 采取2-着色法实现，通过DFS算法，不断对邻节点进行反着色，若无冲突则判定成功。
    KtBipartite(const GRAPH& g) : color_(g.order(), 0), bipartite_(true) {

        int color = 1;
        KtDfsIter<const GRAPH, true, true> iter(g, 0);
        color_[0] = color;

        for (; !iter.isEnd(); ++iter) {
            unsigned from = iter.from();
            assert(from != -1 && color_[from] > 0);
            color = 3 - color_[from]; // flip 1 & 2，着与父节点不同的颜色

            unsigned v = *iter;
            if (color_[v] == 3 - color) { // 着色冲突
                color_[v] = -1;
                bipartite_ = false;
                break;
            }

            color_[v] = color; // 着色
        }
    }


    // 是否二分图
    bool ok() const { return bipartite_; }


    // 返回顶点v的着色，值为1或者2，分别代表对应节点在不同分图，-1代表冲突点
    int color(unsigned v) const { return color_[v]; }


private:
    std::vector<int> color_;
    bool bipartite_; 
};

