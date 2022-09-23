#pragma once
#include <vector>


// ����Դ�㼯��
template<typename GRAPH>
std::vector<unsigned> sources(const GRAPH& g) 
{
    std::vector<unsigned> s;
    for (unsigned v = 0; v < unsigned(g.order()); v++)
        if (g.indegree(v) == 0)
            s.push_back(v);

    return s;
}