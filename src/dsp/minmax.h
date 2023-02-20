#pragma once
#include <functional>
#include "KuMath.h"


// 求解连续函数FUN在[x0, x1]区间的最大最小值
std::pair<double, double> minmaxOnceIter_(
	std::function<double(double* x)> fn, unsigned dim, 
	double* x0, double* x1, double* dx, double* x)
{
	double omin = std::numeric_limits<double>::max();
	double omax = std::numeric_limits<double>::lowest();

	if (dim == 1) {
		for (x[0] = x0[0]; x[0] <= x1[0]; x[0] += dx[0]) 
			KuMath::updateRange(omin, omax, fn(x));
	}
	else {
		--dim;
		for (x[dim] = x0[dim]; x[dim] <= x1[dim]; x[dim] += dx[dim]) {
			auto r = minmaxOnceIter_(fn, dim, x0, x1, dx, x);
			KuMath::uniteRange(omin, omax, r.first, r.second);
		}
	}

	return { omin, omax };
}


std::pair<double, double> minmaxOnce_(
	std::function<double(double* x)> fn, unsigned dim,
	double* x0, double* x1, double* dx)
{
	std::vector<double> x(x0, x0 + dim);
	return minmaxOnceIter_(fn, dim, x0, x1, dx, x.data());
}


// N为迭代次数
std::pair<double, double> minmaxN_(
	std::function<double(double* x)> fn, unsigned dim,
	double* x0, double* x1, double* dx, unsigned N) 
{
	N = 1 << N;

	std::vector<double> dx_(dx, dx + dim);
	KuMath::scale(dx_.data(), dim, 2. / N);

	std::vector<double> x0_(x0, x0 + dim);
	for (unsigned i = 0; i < dim; i++)
		x0_[i] += dx_[i] / 2;
	
	double omin = std::numeric_limits<double>::max();
	double omax = std::numeric_limits<double>::lowest();
	for (unsigned i = 0; i < N; i++) {
		for (unsigned j = 0; j < dim; j++)
			x0_[j] += dx_[j];

		auto r = minmaxOnce_(fn, dim, x0_.data(), x1, dx);
		KuMath::uniteRange(omin, omax, r.first, r.second);
	}

	return { omin, omax };
}

std::pair<double, double> minmax(
	std::function<double(double* x)> fn, unsigned dim,
	double* x0, double* x1, double* dx)
{
	auto r = minmaxOnce_(fn, dim, x0, x1, dx);
	for (unsigned n = 1; n <= 6; n++) {
		auto rn = minmaxN_(fn, dim, x0, x1, dx, n);
		auto diff = KuMath::max(r.first - rn.first, rn.second - r.second, 0.);
		KuMath::uniteRange(r.first, r.second, rn.first, rn.second);

		if (diff == 0 || diff / (r.second - r.first) < 1e-6)
			break;
	}

	if (r.first > r.second)
		r.first = r.second = 0;

	return r;
}