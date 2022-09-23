#pragma once
#include "KtDfsIter.h"
#include <assert.h>
#include <queue>


// 基于源点（入度为0的顶点）队列的拓扑排序算法
// 拓扑排序的2种定义：
//   1.重新编号：对DAG顶点重新编号从而使每条边由较小编号顶点指向较大编号顶点
//   2.重新整理：将DAG顶点在一条水平线上重新整理，从而使所有边均由左指向右
// 以上两种定义是等价和互逆的，若已知重新整理ts，则重新编号计算方法如下：tsI[ts[v]] = v
// 同样，若已知重新编号tsI，则重新整理计算方法如下：ts[tsI[v]] = v
template<typename DAG>
class KtTopologySort
{
    static_assert(DAG::isDigraph(), "KtTopologySort must instantiated with Digraph.");

public:
    KtTopologySort(const DAG& dag) {
        //assert(!dag.hasLoop());
        unsigned V = dag.order();
        ts_.resize(V, -1); tsI_.resize(V, -1);

        // 计算各顶点的入度
        std::vector<unsigned> ins(V, 0);
        for (unsigned v = 0; v < V; v++)
            ins[v] = dag.indegree(v);
        
        // 将源点推入队列q
        std::queue<unsigned> q; // 源点队列
        for(unsigned v = 0; v < V; v++) 
            if(ins[v] == 0) q.push(v);
        assert(!q.empty());


        // 按照FIFO顺序，对源点进行拓扑排序
        for(unsigned i = 0; !q.empty(); i++) {
            unsigned v = q.front(); q.pop();
            ts_[i] = v; 
            tsI_[v] = i;
            auto iter = KtAdjIter(dag, v);
            for (; !iter.isEnd(); ++iter)
                if(0 == --ins[*iter]) 
                    q.push(*iter);
        }
    }

    // 返回重新整理
    unsigned operator[](unsigned v) const {
        return ts_[v];
    }
    
    // 返回重新编号
    unsigned relabel(unsigned v) const {
        return tsI_[v];
    }


    const auto& relabels() const {
        return tsI_;
    }


private:
    std::vector<unsigned> ts_, tsI_; 
};



// 基于Dfs算法的逆拓扑排序
// DFS中的后序编号可以得到一个逆拓扑排序
// 适用于非DAG
template<typename GRAPH>
class KtTopologySortInv
{
    static_assert(GRAPH::isDigraph(), "KtTopologySortInv must instantiated with Digraph.");

public:
    KtTopologySortInv(const GRAPH& g) : dfs_(g, 0) {

        while(!dfs_.isEnd())
            ++dfs_;
        
        unsigned V = g.order();
        popI_.resize(V);
        for (unsigned v = 0; v < V; v++) {
            assert(relabel(v) < V);
            popI_[relabel(v)] = v;
        }
    }


    unsigned operator[](unsigned v) const {
        return popI_[v];
    }
    
    unsigned relabel(unsigned v) const {
        return dfs_.popIndex(v);
    }


private:
    KtDfsIter<const GRAPH, true> dfs_;
    std::vector<unsigned> popI_;
};
