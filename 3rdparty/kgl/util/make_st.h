#pragma once
#include <assert.h>
#include <vector>
#include "is_connected.h"
#include "sources.h"
#include "sinks.h"


// ��Ҫʱ��g����ʵ��Ķ���ͱߣ�ȷ��gֻ��1��Դ���1����㡣
// ����Դ��ͻ���idx�������ߵ�ȨֵΪĬ��ֵ��
template<typename GRAPH>
std::pair<unsigned, unsigned> make_st(GRAPH& g)
{
    static_assert(GRAPH::isDigraph(), "make_st can only operate on digraph.");
    assert(is_connected(g));

    unsigned s, t;

    auto src = sources(g);
    assert(src.size() > 0);
    if (src.size() != 1) { // ����Դ��
        unsigned s = g.addVertex();
        assert(g.degree(s) == 0);
        for (auto w : src)
            g.addEdge(s, w);
    }
    else
        s = src[0];


    auto sik = sinks(g);
    assert(sik.size() > 0);
    if (sik.size() != 1) { // �������
        unsigned t = g.addVertex();
        assert(g.degree(t) == 0);
        for (auto v : sik)
            g.addEdge(v, t);
    }
    else
        t = sik[0];

    return { s, t };
}
