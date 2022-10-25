#include "KuCepsUtil.h"
#include "KtuMath.h"


void KuCepsUtil::makeLifter_Juang(size_t N, double Q, double* lifter_coeffs/*out*/)
{
	for (size_t i = 0; i < N; i++)
		lifter_coeffs[i] = 1.0 + 0.5 * Q * sin(KtuMath<double>::pi * i / Q);
}

void KuCepsUtil::makeLifter_Simple(size_t N, double* lifter_coeffs/*out*/)
{
	for (size_t i = 0; i < N; i++)
		lifter_coeffs[i] = 0.5 + 0.5 * sin(KtuMath<double>::pi * i / N);
}

void KuCepsUtil::makeLifter_Exponent(size_t N, double lift, double* lifter_coeffs/*out*/)
{
	lift = KtuMath<double>::clampCeil(lift, 10);
	for (size_t i = 0; i < N; i++)
		lifter_coeffs[i] = pow(i, lift);
}


void KuCepsUtil::lpc2Ceps(size_t n, const double *lpc, double *ceps)
{
	for (size_t i = 0; i < n; i++) {

		double sum = 0.0;
		for (size_t j = 0; j < i; j++)
			sum += static_cast<double>(i - j) * lpc[j] * ceps[i - j - 1];


		ceps[i] = -lpc[i] - sum / static_cast<double>(i + 1);
	}
}

// TODO: TEST
void KuCepsUtil::ceps2Lpc(size_t n, const double *ceps, double *lpc)
{
	for (size_t i = 0; i < n; i++) {

		double sum = 0.0;
		for (size_t j = 0; j < i; j++)
			sum += static_cast<double>(i - j) * lpc[j] * ceps[i - j - 1];


		lpc[i] = -ceps[i] - sum / static_cast<double>(i + 1);
	}
}