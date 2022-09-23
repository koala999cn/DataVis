#pragma once
#include "../GraphX.h"
#include "../core/KtMaxFlow.h"
#include "copy.h"


/// ����������㷨ʵ�ֵĿ������ж�
/// 
/// ���������壺�ٶ�Ϊ������ÿ�����㸳��Ȩֵ����ʾ��Ӧ��ȨֵΪ����������ȨֵΪ������
/// ���Ҷ���Ȩֵ֮��Ϊ0����ô���һ������ÿ����������������������Ĳ���ڸö����Ȩֵ��
/// ��ô������ǡ����еġ���
/// 
/// �������ж��ɹ�ԼΪ��������⣬��Ҫ˼·���£�
/// ����һ�����������⣬����һ��������������ͬ�Ķ���ͱߣ����Ƕ�����Ȩֵ��
/// ���⣬����һ��Դ��s������ÿ����Ӧ���㣨ȨֵΪ��������һ��Ȩֵ���ڸö���
/// ��Ӧ���ıߣ�������һ�����t����ÿ�����󶥵㵽t����һ��Ȩֵ���ڸö���
/// ����������ֵ�ıߡ���������Ͻ����������⣬�����ǵ��ҽ���һ�����д�Դ��
/// ���������бߺ͵���������б߶����������������������ԭ���Ŀ�������


// @GRAPH: st�����Ȩ�����ߵ�ȨֵΪ���ͣ�����������������������Ϊ���ͣ�����Ӧ����ֵ�������󣨸�ֵ��
template<typename GRAPH>
bool feasible(const GRAPH& g)
{
	static_assert(std::is_integral_v<typename GRAPH::vertex_type>, "graph must have integral vertex type");

	DigraphSx<typename GRAPH::edge_type> newG;
	copy(g, newG);
	auto s = newG.addVertex();
	auto t = newG.addVertex();

	for (unsigned v = 0; v < g.order(); v++) {
		typename GRAPH::edge_type wt = g.vertexAt(v);
		if (wt >= 0)
			newG.addEdge(s, v, wt);
		else
			newG.addEdge(v, t, -wt);
	}

	KtMaxFlowPre mf(newG, s, t);
	assert(mf.check(s, t));

	for (unsigned v = 0; v < g.order(); v++) {
		typename GRAPH::edge_type wt = g.vertexAt(v);
		if (wt >= 0) {
			if (mf.residual(s, v) != 0)
				return false;
		}
		else {
			if (mf.residual(v, t) != 0)
				return false;
		}
	}

	return true;
}

