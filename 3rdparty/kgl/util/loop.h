#pragma once
#include "../core/KtDfsIter.h"


template<typename GRAPH>
bool has_selfloop(const GRAPH& g) 
{
    auto V = g.order();
    for(decltype(V) i = 0; i < V; i++) 
        if(g.hasEdge(i, i)) 
            return true;

    return false;
}


// �Ի�<v, v>������
template<typename GRAPH>
unsigned selfloops(const GRAPH& g, unsigned v)
{
    return g.edges(v, v).size();
}


template<typename GRAPH>
std::pair<unsigned, unsigned> selfloops(const GRAPH& g)
{
    unsigned N(0), V(0);
    for (decltype(g.order()) i = 0; i < g.order(); i++) {
        auto n = selfloops(g, i);
        if (n > 0) {
            ++V;
            N += n;
        }
    }

    return { N, V };
}


// ɾ���Ի�
template<typename GRAPH>
void erase_selfloop(GRAPH& g) 
{
    auto V = g.order();
    for(decltype(V) i = 0; i < V; i++)
        if(g.hasEdge(i, i))
            g.eraseEdge(i, i); 
}


// һ������ͼ��DAG�����ҽ�����ʹ��DFS���ÿ����ʱδ�����κλر�
template<typename GRAPH>
bool has_loop(const GRAPH& g) 
{
    KtDfsIter<const GRAPH, true, true, true> iter(g, 0);
    while(!iter.isEnd()) {
        if(iter.isBack())
            return true;
        ++iter;
    }       

    return false;
}
    

// ɾ����
template<typename GRAPH>
void erase_loop(GRAPH& g) 
{
    // ɾ���ر�. �Ի�Ҳ�ǻر�
    KtDfsIter<GRAPH, true, true> iter(g, 0);
    while(!iter.isEnd()) {
        if(iter.isBack())
            iter.erase();
        else
            ++iter;
    }
}