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

	// �������롢�����dim
	unsigned idim() const;
	unsigned odim() const;

	// @in: ��fbank���Ϊ����
	// @out: mfcc���
	void process(const double* in, double* out) const;

private:
	unsigned idim_;
	unsigned numCeps_;
	void* dptr_;
};
