#pragma once
#include "../core/KtBfsIter.h"


// 顶点v是否可达w，即是否存在一条从v到w的路径
template<typename GRAPH>
bool is_reachable(const GRAPH& g, unsigned v, unsigned w) 
{
	KtBfsIter<const GRAPH> iter(g, v);
	for (; !iter.isEnd(); ++iter)
		if (*iter == w)
			return true;

	return false;
}


// 设置v可达的顶点标记
template<typename GRAPH>
std::vector<bool> get_reachable(const GRAPH& g, unsigned v) 
{
	std::vector<bool> flags(g.order(), false);
	KtBfsIter<const GRAPH> iter(g, v);
	for (; !iter.isEnd(); ++iter)
		flags[*iter] = true;

	return flags;
}


// 移除v不可达的顶点
template<typename GRAPH>
void erase_unreachable(GRAPH& g, unsigned v) 
{
	auto flags = get_reachable(v);

	// 逆序删除v不可达的顶点
	for (unsigned i = unsigned(flags.size()) - 1; i != -1; i--)
		if (!flags[i])
			g.eraseVertex(i);
}