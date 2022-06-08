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


double KgRand::gauss(double mu, double sigma)
{
	double x(-6.0);
	for (int i = 0; i < 12; i++)
		x += uniform();

	return mu + x * sigma;
}


std::complex<double> KgRand::gaussComplex()
{
	// generate two uniform random numbers
	double u1 = uniformNonZero();
	double u2 = uniform();

	return sqrt(-2 * log(u1)) * std::exp(std::complex<double>(0, 1) * 2.0 * KtuMath<double>::pi * u2);
}


#if 0
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

double KgRand::gauss()
{
	// generate two uniform random numbers
	double u1 = uniformNonZero();
	double u2 = uniform();

	return sqrt(-2 * log(u1)) * sin(2 * KtuMath<double>::pi * u2);
	//return sqrt(-2 * log(u1)) * cos(2*KtuMath<double>::pi*u2);
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
#endif

double KgRand::exponent(double beta) 
{
	return -beta * log(uniformNonZero());
}


double KgRand::laplace(double beta)
{
	double x;
	double u1 = uniform();

	if (u1 <= 0.5) {
		auto u2 = uniform();
		while(u2 == 1.0)
			u2 = uniform();
		x = -beta * log(1.0 - u2);
	}
	else
		x = beta * log(uniformNonZero());

	return x;
}


double KgRand::rayleigh(double sigma) 
{
	double x = -2.0 * log(uniformNonZero());
	x = sigma * sqrt(x);
	return x;
}


double KgRand::lognorm(double mu, double sigma)
{
	return exp(gauss(mu, sigma));
}


double KgRand::cauchy(double alpha, double beta) 
{
	assert(beta > 0);
	return alpha - beta / tan(KtuMath<double>::pi * uniform());
}


double KgRand::weibull(double alpha, double beta)
{
	assert(alpha > 0 && beta > 0);

	return beta * pow(-log(uniformNonZero()), 1.0 / alpha);
}


double KgRand::erlang(int m, double beta)
{
	double u(1.0);
	for (int i = 0; i < m; i++)
		u *= uniform();
	return -beta * log(u);
}


int KgRand::bernoulli(double p)
{
	return uniform() <= p ? 1 : 0;
}


double KgRand::bngauss(double p, double mu, double sigma)
{
	return bernoulli(p) ? gauss(mu, sigma) : 0;
}


int KgRand::binomial(int n, double p)
{
	int x(0);
	for (int i = 0; i < n; i++)
		x += bernoulli(p);

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
	} while (b >= a);

	return i - 1;
}


// TODO: test
double KgRand::rice(double K, double omega)
{
	std::complex<double> x, y;
	double s = sqrt((omega * K) / (K + 1));
	double sig = sqrt(0.5 * omega / (K + 1));
	x = gaussComplex();
	y = std::complex<double>(0, 1) * (x.real() * sig + s) + x.imag() * sig;
	return std::abs(y);
}