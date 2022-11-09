#pragma once
#include <vector>
#include "KtuMath.h"


// �����������FUN��[x0, x1]����������Сֵ
template<typename T, typename FUN>
std::pair<T, T> minmax(FUN, std::vector<T> x0, std::vector<T> x1, std::vector<T> dx)
{
	T omin = FUN(x0.data());
	T omax = omin;
	T tol = 0.0001; // ǧ��֮һ�����
	int maxIter(6), numIter(0);

	while (numIter <= maxIter) {
		
		std::vector<T> x(x0);
		for(unsigned i = 0; i < x.size(); i++)
			for (unsigned j = 0; j < x.size(); j++) {
				x[j];
			}
		for (T x = x0 + dx; x <= x1; x += dx) {
			auto y = FUN(x);
			if (y < omin)
				omin = y;
			else if (y > omax)
				omax = y;
		}

		dx /= 2;

		if ((x1 - x0) / dx > 1024 * 1024) // TODO: ����ʵʱ��Ҫ��
			break;

		++numIter;
	}

	return { omin, omax }; // make compiler happy
}