#pragma once
#include <vector>
#include "KtDfsIterX.h"
#include "KtTopologySort.h"
#include "../util/inverse.h"


/*
 对于一个有向图顶点的子集S，如果在S内任取两个顶点u和v，都能找到一条从u到v的路径，那么S是强连通的。
 如果在强连通的顶点集合S中加入其他任意顶点，它都不再是强连通的，那么S是原图的一个强连通分量（SCC：Strongly Connected Component）。
 任意有向图都可以分解成若干不相干的SCC，这就是强连通分量分解。
 把分解后的SCC缩成一个顶点，就得到一个DAG。
*/

class KvStronglyConnected
{
public:

    // 返回强连通分量数目
    unsigned count() const {
        return numScc_;
    }

    // 判断节点v和w是否属于同一强连通分量
    bool reachable(unsigned v, unsigned w) const {
        return idScc_[v] == idScc_[w];
    }

    // 返回节点v所在强连通分量的ID
    unsigned operator[](unsigned v) const {
        return idScc_[v];
    }


protected:
    unsigned numScc_; // 强连通分量数目
    std::vector<unsigned> idScc_; // 强连通分量的id，相同id的顶点属于同一强连通分量
};


// 基于Kosaraju算法的SCC分解
// 算法通过两遍简单的DFS实现：
//   第一遍：对所有顶点进行后序编号。完成后，越接近图的尾部（搜索树的叶子），顶点的编号越小。
//   第二遍：先将所有边反向（或进行逆图操作），然后以编号最大的顶点为起点进行DFS，遍历顶点集合即为一个SCC。
//          之后，只要还有尚未访问的顶点，就从中选取编号最大的不断重复DFS。
template<typename GRAPH>
class KtStronglyConnectedKos : public KvStronglyConnected
{
    static_assert(GRAPH::isDigraph(), "KtStronglyConnectedKos must instantiated with DiGraph.");

public:
    KtStronglyConnectedKos(const GRAPH& g) {
        // 第一遍DFS
        KtTopologySortInv<GRAPH> ts(g); // 拓扑排序算法只对DAG有效，所以此处使用基于DFS的逆拓扑排序计算后序编号

        // 第二遍DFS
        auto gR = inverse(g); 
        unsigned V = g.order();
        numScc_ = 0;
        idScc_.assign(V, -1);        
        unsigned i(V-1);
        KtDfsIter<const GRAPH> iter(gR, -1);
    
        while(true) {
            iter.start(ts[i]);

            while(!iter.isEnd()) {
                if(iter.isTree()) {
                    assert(idScc_[*iter] == -1);
                    idScc_[*iter] = numScc_;
                }
                ++iter;      
            }

            ++numScc_;

            // 搜索尚未访问的最大编号顶点
            while(i != 0 && idScc_[ts[--i]] != -1);

            if(idScc_[ts[i]] != -1) // 所有顶点均已访问
                break;
        }
    }
};


// 基于Tarjan算法的SCC分解
// 与findBridge算法相似，通过low值回溯以聚合连通分量
template<typename GRAPH>
class KtStronglyConnectedTar : public KvStronglyConnected
{
    static_assert(GRAPH::isDigraph(), "KtStronglyConnectedTar must instantiated with DiGraph.");

public:
    KtStronglyConnectedTar(const GRAPH& g) {
        unsigned V = g.order();
        numScc_ = 0;
        idScc_.resize(V, -1);       
        std::vector<unsigned> S; 
        KtDfsIterX<const GRAPH, true> iter(g, 0);
        while(!iter.isEnd()) {
            unsigned v = *iter;
            if(iter.isPushing()) 
                S.push_back(v);

            if(iter.isBridge()) {
                unsigned w;
                do {
                    w = S.back(); S.pop_back();
                    idScc_[w] = numScc_; 
                    iter.resetLowIndex(w); 
                }while(w != v);
                numScc_++;
            } 

            ++iter;
        }
    }
};


// 基于Gabow算法的SCC分解
template<typename GRAPH>
class KtStronglyConnectedGab : public KvStronglyConnected
{
    static_assert(GRAPH::isDigraph(), "KtStronglyConnectedGab must instantiated with DiGraph.");

public:
    KtStronglyConnectedGab(const GRAPH& g) {
        unsigned V = g.order();
        numScc_ = 0;
        idScc_.resize(V, -1);       
        std::vector<unsigned> S, path;
        KtDfsIter<const GRAPH, true, true, true> iter(g, 0);
        while(!iter.isEnd()) {
            unsigned v = *iter;
            if(iter.isTree()) {
                S.push_back(v);
                path.push_back(v);
            }
            else if(idScc_[v] == -1) {
                assert(iter.pushIndex(v) != -1);
                while(iter.pushIndex(path.back()) > iter.pushIndex(v))
                    path.pop_back();
            }

            if(iter.isPopping() && path.back() == v) {
                path.pop_back();

                unsigned w;
                do {
                    w = S.back(); S.pop_back();
                    idScc_[w] = numScc_;
                }while(w != v);    
                numScc_++;     
            }                

            ++iter;
        }
    }
};