#pragma once

class KuCepsUtil
{
public:

	// 对于不同的识别任务，每个倒谱系数的贡献率不同，这时需要选用不同的提升算法，
	// 提升贡献率大的倒谱系数，抑制贡献率小甚至起反作用的倒谱系数

	// see BIING-HWANG JUANG, 1987, On the Use of Bandpass Liftering in Speech Recognition
	// w(k) = 1 + 0.5*Q*sin(PI*k/Q), 0 <= k < N
	// It is htk & kaldi compatible
	// 文中建议Q取12.
	static void makeLifter_Juang(size_t N, double Q, double* lifter_coeffs/*out*/);

	// w(k) = 0.5 + 0.5*sin(PI*k/N), 0 <= k < N
	static void makeLifter_Simple(size_t N, double* lifter_coeffs/*out*/);

	// see http://labrosa.ee.columbia.edu/matlab/rastamat/lifter.m
	// w(k) = k^lift, 0 <= k < N
	// lift建议取0.6
	static void makeLifter_Exponent(size_t N, double lift, double* lifter_coeffs/*out*/);


	/*
	Convert between LP Cepstral Coef and LP Coef
	*/

	// length(pLPC) = length(pCepst) = n
	static void lpc2Ceps(size_t n, const double *lpc, double *ceps);
	static void ceps2Lpc(size_t n, const double *ceps, double *lpc);

private:
	KuCepsUtil() { }
	~KuCepsUtil() { }
};

