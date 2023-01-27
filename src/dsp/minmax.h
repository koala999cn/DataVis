#pragma once
#include <functional>
#include "KuMath.h"


// 求解连续函数FUN在[x0, x1]区间的最大最小值
std::pair<double, double> minmaxOnceIter_(
	std::function<double(double* x)> fn, unsigned dim, 
	double* x0, double* x1, double* dx, double* x)
{
	if (dim == 1) {
		x[0] = x0[0];
		double omin = fn(x);
		double omax = omin;

		for (x[0] = x0[0] + dx[0]; x[0] <= x1[0]; x[0] += dx[0]) {
			auto val = fn(x);
			if (val < omin)
				omin = val;
			else if (val > omax)
				omax = val;
		}

		return { omin, omax };
	}

	--dim;
	x[dim] = x0[dim];
	auto r = minmaxOnceIter_(fn, dim, x0, x1, dx, x);

	for (x[dim] = x0[dim] + dx[dim]; x[dim] <= x1[dim]; x[dim] += dx[dim]) {
		auto ri = minmaxOnceIter_(fn, dim, x0, x1, dx, x);
		if (ri.first < r.first)
			r.first = ri.first;
		else if (ri.second > r.second)
			r.second = ri.second;
	}

	return r;
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
	
	auto r = minmaxOnce_(fn, dim, x0_.data(), x1, dx);
	for (unsigned i = 1; i < N; i++) {
		for (unsigned j = 0; j < dim; j++)
			x0_[j] += dx_[j];

		auto ri = minmaxOnce_(fn, dim, x0_.data(), x1, dx);
		if (ri.first < r.first)
			r.first = ri.first;
		else if (ri.second > r.second)
			r.second = ri.second;
	}

	return r;
}

std::pair<double, double> minmax(
	std::function<double(double* x)> fn, unsigned dim,
	double* x0, double* x1, double* dx)
{
	auto r = minmaxOnce_(fn, dim, x0, x1, dx);
	for (unsigned n = 1; n <= 6; n++) {
		auto rn = minmaxN_(fn, dim, x0, x1, dx, n);
		auto diff = KuMath::max(r.first - rn.first, rn.second - r.second, 0.);
		if (rn.first < r.first)
			r.first = rn.first;
		else if (rn.second > r.second)
			r.second = rn.second;

		if (diff == 0 || diff / (r.second - r.first) < 1e-6)
			break;
	}

	return r;
}