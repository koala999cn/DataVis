#pragma once
#include <vector>


// 返回源点集合
template<typename GRAPH>
std::vector<unsigned> sources(const GRAPH& g) 
{
    std::vector<unsigned> s;
    for (unsigned v = 0; v < unsigned(g.order()); v++)
        if (g.indegree(v) == 0)
            s.push_back(v);

    return s;
}