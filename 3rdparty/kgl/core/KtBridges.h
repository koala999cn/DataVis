#pragma once
#include <vector>
#include "KtDfsIterX.h"


// Ѱ��ͼ����
// ��Ҳ�ƹؽڱ�(articulation edge)�����ɾ�������߽���һ����ͨͼ�ֽ�Ϊ���ཻ��������ͼ��
// û���ŵ�ͼ��Ϊ����ͨ��
// ���κ�DFS���У�һ������v-w��һ���ţ������ǵ��ҽ��������ڻر߽�w��һ��������w��һ������������
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

