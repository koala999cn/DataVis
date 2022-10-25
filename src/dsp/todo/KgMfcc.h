#pragma once


class KgMfcc
{
public:

	struct KpOptions
	{
		unsigned idim;
		unsigned numCeps;  // number of cepstral coeffs(default: 13)
		double cepsLifter;  // constant that controls scaling of MFCCs(default = 22). if 0.0, no liftering is done.
	};

	KgMfcc() = default;
	KgMfcc(KgMfcc&& mfcc) noexcept;
	KgMfcc(const KpOptions& opts);
	~KgMfcc();

	// 返回输入、输出的dim
	unsigned idim() const;
	unsigned odim() const;

	// @in: 以fbank输出为输入
	// @out: mfcc结果
	void process(const double* in, double* out) const;

private:
	unsigned idim_;
	unsigned numCeps_;
	void* dptr_;
};
