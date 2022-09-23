#pragma once
#include <vector>
#include <algorithm>
#include "KtDfsIterX.h"


// 寻找图的割点
// 割点(cut-point)也称关节点(articulation point)，如果删除该顶点，将把一个连通图分解为至少两个不相交的子图
// 没有割点的图称为重连通(Biconnected)或顶点连通
// 重连通图中的每一对顶点，都由两条不相交的路径相连
template<typename GRAPH>
class KtCutPoints
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtCutPoints(const GRAPH& g) {
        vertex_index_t root(0);
        unsigned sons_of_root(0); // 根节点的子树数量

        KtDfsIterX<const GRAPH, false> iter(g, root);
        while (!iter.isEnd()) {
            vertex_index_t p = iter.from();
            if (p == root && iter.isTree()) // 忽略popping状态 
                ++sons_of_root;

            if (p != -1 && p != root/*根节点在后面处理*/ && iter.isPopping()) {
                vertex_index_t w = *iter;
                if (iter.lowIndex(w) >= iter.pushIndex(p))
                    cutpoints_.push_back(p);
            }

            ++iter;
        }

        if (sons_of_root > 1) cutpoints_.push_back(root);  // 如果根节点有多个子树，根节点为割点
        std::sort(cutpoints_.begin(), cutpoints_.end());
        cutpoints_.erase(std::unique(cutpoints_.begin(), cutpoints_.end()), cutpoints_.end());
    }


    auto size() const { return cutpoints_.size(); }
    auto begin() const { return cutpoints_.cbegin(); }
    auto end() const { return cutpoints_.cend(); }

    vertex_index_t operator[](unsigned idx) const {
        return cutpoints_[idx];
    }

private:
    std::vector<vertex_index_t> cutpoints_;
};

