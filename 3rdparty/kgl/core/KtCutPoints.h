#pragma once
#include <vector>
#include <algorithm>
#include "KtDfsIterX.h"


// Ѱ��ͼ�ĸ��
// ���(cut-point)Ҳ�ƹؽڵ�(articulation point)�����ɾ���ö��㣬����һ����ͨͼ�ֽ�Ϊ�����������ཻ����ͼ
// û�и���ͼ��Ϊ����ͨ(Biconnected)�򶥵���ͨ
// ����ͨͼ�е�ÿһ�Զ��㣬�����������ཻ��·������
template<typename GRAPH>
class KtCutPoints
{
public:
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtCutPoints(const GRAPH& g) {
        vertex_index_t root(0);
        unsigned sons_of_root(0); // ���ڵ����������

        KtDfsIterX<const GRAPH, false> iter(g, root);
        while (!iter.isEnd()) {
            vertex_index_t p = iter.from();
            if (p == root && iter.isTree()) // ����popping״̬ 
                ++sons_of_root;

            if (p != -1 && p != root/*���ڵ��ں��洦��*/ && iter.isPopping()) {
                vertex_index_t w = *iter;
                if (iter.lowIndex(w) >= iter.pushIndex(p))
                    cutpoints_.push_back(p);
            }

            ++iter;
        }

        if (sons_of_root > 1) cutpoints_.push_back(root);  // ������ڵ��ж�����������ڵ�Ϊ���
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

