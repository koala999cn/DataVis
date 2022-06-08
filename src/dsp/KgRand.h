#pragma once
#include <complex>

/*
 * 随机信号生成器
 * 
 * 代码参考《数字信号处理C语言程序集》，殷福亮、宋爱军
 * 
 * DATA: 20220608
 */


// 各类随机数发生器
class KgRand
{
public:
	enum { k_rand_max = 0x7fffu	};

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

	// 正态分布
	//   pdf(x) = exp(-(x-mu)^2/(2*sigma^2)) / (sqrt(2pi)*sigma)
	// 均值：mu
	// 方差：sigma^2
	double gauss(double mu, double sigma);

	// Complex Gauss
	std::complex<double> gaussComplex();

#if 0
	/* random number with a N(mu,sigma^2) distribution */
	// HTK实现
	double gaussDeviate(double mu, double sigma);

	double gauss();

	// additive white Gauss noise
	void awgn(std::complex<double>& _x, double _nstd);

	// Returns a pair of gaussian random numbers. Uses Box-Muller transform
	void gauss2(double& a, double& b);
#endif

	// 指数分布
	//	 pdf(x) = exp(-x/beta) / beta, x >= 0 
	//          = 0, 其他
	// 均值：beta
	// 方差：beta^2
	double exponent(double beta);

	// 拉普拉斯分布(双指数分布)
	//   pdf(x) = exp(-|x|/beta) / (2*beta)
	// 均值：0
	// 方差：2*beta^2
	double laplace(double beta);

	// 瑞利分布
	//   pdf(x) = x * exp(-x^2/(2*sigma^2)) / sigma^2, x > 0
	// 均值：sigma * sqrt(pi/2)
	// 方差：(2-pi/2) * sigma^2
	double rayleigh(double sigma);

	// 对数正态分布
	//   pdf(x) = exp(-(ln(x)-mu)^2/(2*sigma^2)) / (x*sqrt(2pi)*sigma, x > 0
	//          = 0, x <= 0 
	// 均值：exp(mu+sigma^2/2)
	// 方差：(exp(sigma^2)-1)*exp(2mu+sigma^2)
	double lognorm(double mu, double sigma);

	// 柯西分布
	//   pdf(x) = beta / (pi*(beta^2+(x-alpha)^2)), beta > 0
	double cauchy(double alpha, double beta);

	// 韦伯分布
	//   pdf(x) = (alpha/beta^a) * x^(alpha-1) * exp(-(x/beta)^alpha), x >= 0, alpha > 0, beta > 0
	//          = 0, x < 0
	double weibull(double alpha, double beta);

	// 埃尔朗分布
	//   pdf(x) = beta^(-m) * x^(m-1) * exp(-x/beta) / (m-1)!, x >= 0, beta > 0
	//          = 0
	// 均值：m*beta
	// 方差：m*beta^2
	// 当m=1时退化为指数分布
	double erlang(int m, double beta);

	// 贝努利分布
	//   pdf(x) = p, x = 1
	//          = 1 - p, x = 0
	// 均值：p
	// 方差：p(1-p)
	int bernoulli(double p); // 以p的概率返回1，以(1-p)的概率返回0

	// 贝努利-高斯分布
	//   pdf(x) = bernoulli(x) * gauss(x)
	// 均值：p*mu
	// 方差：p
	double bngauss(double p, double mu, double sigma);

	// 二项式分布
	//   pdf(x) = C(n, x) * p^x * (1-p)^(n-x)
	// 均值：np
	// 方差：np(1-p)
	// 当n=1时退化为贝努利分布 
	int binomial(int n, double p);

	// 泊松分布
	//   pdf(x) = lambda^x * exp(-lambda) / x!, x = 0, 1, ..., lambda
	// 均值：lambda
	// 方差：lambda
	int poisson(double lambda);

	// 莱斯分布
	//   pdf(x) = (R/sigma^2) * exp(-(R^2+A^2)/(2*sigma^2)) * I0(R*A/sigma^2)
	// @R: 正弦信号加窄带高斯随机信号的包络
	// @A: 主信号幅度的峰值
	// @sigma^2: 多径信号分量的功率
	// @I0: 0阶第一类贝塞尔函数
	// 定义K = A^2 / (2*sigma^2)为莱斯因子，表示主信号的功率与多径分量功率之比
	// 当A趋于0时，莱斯分布退化为瑞利分布
	double rice(double K, double omega);

private:
	int seed_;
};

