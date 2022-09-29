#pragma once
#include <vector>
#include "KtDfsIterX.h"


// 寻找图的桥
// 桥也称关节边(articulation edge)，如果删除这条边将把一个连通图分解为不相交的两个子图。
// 没有桥的图称为边连通。
// 在任何DFS树中，一条树边v-w是一个桥，条件是当且仅当不存在回边将w的一个子孙与w的一个祖先相连接
template<typename GRAPH>
class KtBridges
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;
    using vertex_pair_t = std::pair<vertex_index_t, vertex_index_t>;
    using bridges_t = std::vector<vertex_pair_t>;

    KtBridges(const GRAPH& g) {
        KtDfsIterX<const GRAPH, true> iter(g, 0);
        while (!iter.isEnd()) {
            if (iter.isBridge()) {
                if (iter.from() != -1)
                    bridges_.push_back({ iter.from(), *iter });
            }

            ++iter;
        }
    }


    auto size() const { return bridges_.size(); }
    auto begin() const { return bridges_.cbegin(); }
    auto end() const { return bridges_.cend(); }

    const vertex_pair_t& operator[](unsigned idx) const {
        return bridges_[idx];
    }

private:
    bridges_t bridges_;
};

