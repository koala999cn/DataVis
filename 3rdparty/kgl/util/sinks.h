#include <vector>


// 返回汇点集合
template<typename GRAPH>
std::vector<unsigned> sinks(const GRAPH& g)
{
    std::vector<unsigned> s;
    for (unsigned v = 0; v < unsigned(g.order()); v++)
        if (g.outdegree(v) == 0)
            s.push_back(v);

    return s;
}