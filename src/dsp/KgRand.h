#pragma once
#include <complex>


// 各类随机数发生器
class KgRand
{
public:
	enum
	{ 
		k_rand_max = 0x7fffu
	};

	// 种子初始化为固定值，便于调试
	KgRand();

	KgRand(int seed);

	// 随机设置种子
	void reseed(); 

	// 设置指定种子
	void reseed(int seed);

	// 产生[0, k_rand_max]区间的随机数
	int rand();

	// 均匀分布, 返回介于low和up之间的随机数
	double rand(double low, double up);

	int rand(int low, int up);

	// 返回0到1直接的随机浮点数
	double uniform();

	double uniformNonZero();

	/* random number with a N(mu,sigma^2) distribution */
	// HTK实现
	double gaussDeviate(double mu, double sigma);

	// 近似高斯分布
	double gaussFast(double mean, double sigma);

	double gauss();

	// Complex Gauss
	std::complex<double> gaussComplex();

	// additive white Gauss noise
	void awgn(std::complex<double>& _x, double _nstd);

	// Returns a pair of gaussian random numbers. Uses Box-Muller transform
	void gauss2(double& a, double& b);

	// 指数分布, 均值：beta
	// if x>=0 
	//	 f(x) = (1/beta)*exp(-x/beta)
	// else 
	//   f(x) = 0
	// 均值：beta
	// 方差：beta*beta ???
	double exponent(double beta);

	// 拉普拉斯分布(双指数分布)
	// f(x) = (1/2*beta)*exp(-|x|/beta)
	// 均值：0
	// 方差：2*beta*beta
	double laplace(double beta);

	// 瑞利分布
	double rayleigh(double sigma);

	// 柯西分布
	double cauchy(double alpha, double beta);

	// 泊松分布
	int poisson(double lambda);

	// Weibull
	double weibull(double _alpha, double _beta, double _gamma);

	// Rice-K
	double riceK(double _K, double _omega);

private:
	int seed_;
};

