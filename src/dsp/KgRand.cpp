#include "KgRand.h"
#include "KtuMath.h"
#include <assert.h>
#if defined(__LINUX__)
#include <sys/time.h>
#else
#include <time.h>
#endif


KgRand::KgRand()
{
	seed_ = 20220202;
}


KgRand::KgRand(int seed) 
{
	reseed(seed);
}


void KgRand::reseed()
{
#if defined(__LINUX__)
	timeval t;
	::gettimeofday(&t, 0);
	reseed(t.tv_usec * t.tv_sec);
#else
	reseed((int)::time(0));
#endif
}


void KgRand::reseed(int seed)
{ 
	seed_ = seed;
}


int KgRand::rand() 
{
	// from flipcode by John W. Ratcliff 
	return (((seed_ = seed_ * 214013L + 2531011L) >> 16) & k_rand_max); 
}


double KgRand::rand(double low, double up) 
{
	return low + (rand() * (up - low)) * (1.0 / k_rand_max);
}


int KgRand::rand(int low, int up)
{
	return low + (int)((up - low + 1) * rand(0.0, 1.0));   // round down by truncation, because positive
}


double KgRand::uniform() 
{ 
	return rand(0.0, 1.0); 
}


double KgRand::uniformNonZero() 
{
	auto u = uniform();

	// ensure u does not equal zero
	while (u == 0)
		u = uniform();

	return u;
}


// HTK实现
double KgRand::gaussDeviate(double mu, double sigma)
{
	double fac, r, v1, v2, x;
	static int gaussSaved = 0; /* GaussDeviate generates numbers in pairs */
	static double gaussSave;    /* 2nd of pair is remembered here */


	if (gaussSaved) {
		x = gaussSave; gaussSaved = 0;
	}
	else {
		do {
			v1 = 2.0 * uniform() - 1.0;
			v2 = 2.0 * uniform() - 1.0;
			r = v1 * v1 + v2 * v2;
		} while (r >= 1.0);
		fac = sqrt(-2.0 * log(r) / r);
		gaussSaved = 1;
		gaussSave = v1 * fac;
		x = v2 * fac;
	}
	return x * sigma + mu;
}


double KgRand::gaussFast(double mean, double sigma)
{
	double x(0);
	for (int i = 0; i < 12; i++)
		x += uniform();

	x -= 6.0; 
	x /= 6.0; // 归一化
	x *= sigma;
	x += mean;

	return x;
}


double KgRand::gauss()
{
	// generate two uniform random numbers
	double u1 = uniformNonZero();
	double u2 = uniform();

	return sqrt(-2 * log(u1)) * sin(2 * KtuMath<double>::pi * u2);
	//return sqrt(-2 * log(u1)) * cos(2*KtuMath<double>::pi*u2);
}


std::complex<double> KgRand::gaussComplex()
{
	// generate two uniform random numbers
	double u1 = uniformNonZero();
	double u2 = uniform();

	return sqrt(-2 * log(u1)) * std::exp(std::complex<double>(0, 1) * 2.0 * KtuMath<double>::pi * u2);
}


void KgRand::awgn(std::complex<double>& _x, double _nstd)
{
	_x += gaussComplex() * _nstd * 0.707106781186547;
}


void KgRand::gauss2(double& a, double& b)
{
	// generate two uniform random numbers
	auto u1 = uniformNonZero();
	auto u2 = uniform();

	u1 = sqrt(-2.0f * log(u1));
	u2 = 2.0f * KtuMath<double>::pi * u2;
	a = u1 * cos(u2);
	b = u1 * sin(u2);
}


double KgRand::exponent(double beta) 
{
	double x = uniform();
	x = -beta * log(x);
	return x;
}


double KgRand::laplace(double beta)
{
	double x, u = uniform();
	if (u <= 0.5)
		x = -beta * log(1.0 - u);
	else
		x = beta * log(u);

	return x;
}


double KgRand::rayleigh(double sigma) 
{
	double x, u = uniform();
	x = -2.0 * log(u);
	x = sigma * sqrt(x);
	return x;
}


double KgRand::cauchy(double alpha, double beta) 
{
	double x, u = uniform();
	x = alpha - beta / tan(KtuMath<double>::pi * u);
	return x;
}


int KgRand::poisson(double lambda) 
{
	double a(exp(-lambda)), b(1.0), u;
	int i(0);
	do {
		u = uniform();
		b *= u;
		i++;
	} while (b > a);

	return i - 1;
}


double KgRand::weibull(double _alpha, double _beta, double _gamma)
{
	// validate input
	assert(_alpha > 0 && _beta > 0);

	double u = uniformNonZero();
	return _gamma + _beta * pow(-log(u), 1.0 / _alpha);
}

// Rice-K
double KgRand::riceK(double _K, double _omega)
{
	std::complex<double> x, y;
	double s = sqrt((_omega * _K) / (_K + 1));
	double sig = sqrt(0.5 * _omega / (_K + 1));
	x = gaussComplex();
	y = std::complex<double>(0, 1) * (x.real() * sig + s) + x.imag() * sig;
	return std::abs(y);
}