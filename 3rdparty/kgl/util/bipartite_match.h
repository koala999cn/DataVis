#pragma once
#include <vector>
#include "../GraphX.h"
#include "../core/KtMaxFlow.h"
#include "../core/KtBipartite.h"
#include "../core/KtAdjIter.h"


/// ����������㷨ʵ�ֵ�����������ƥ�䣬�㷨���Ӷ�O(VE)
/// 
/// ����������ƥ�䣺����һ������ͼ���ҳ�һ�����������ı߼���
/// �Ӷ�ʹ��������������һ���������������ӡ�
/// 

// @GRAPH: ����ͼ������ͼ��
// @return: ƥ��Ķ���ԣ�empty��ʾ���������ͼ�Ƕ���ͼ��
template<typename GRAPH>
auto bipartite_match(const GRAPH& g) -> std::vector<std::pair<unsigned, unsigned>>
{
	std::vector<std::pair<unsigned, unsigned>> res;
	KtBipartite<GRAPH> bip(g);
	if (bip.ok()) {
		DigraphSx<int> newG(g.order() + 2);
		auto s = g.order(); // �����������Ϊst����
		auto t = s + 1;

		for (unsigned v = 0; v < g.order(); v++) {
			if (bip.color(v) == 1) {
				newG.addEdge(s, v, 1);

				auto adj = KtAdjIter(g, v);
				for (; !adj.isEnd(); ++adj)
					newG.addEdge(v, *adj, 1);
			}
			else
				newG.addEdge(v, t, 1);
		}

		KtMaxFlowPre mf(newG, s, t);
		assert(mf.check(s, t));

		res.reserve(g.order() / 2);
		for (unsigned v = 0; v < g.order(); v++) {
			auto adj = KtAdjIter(newG, v);
			for (; !adj.isEnd(); ++adj) {
				if (*adj != t && mf.flow(v, *adj) == 1) {
					res.push_back({ v, *adj }); 
					break; // ���ֻ��һ����������
				}
			}
		}
	}

	return res;
}

