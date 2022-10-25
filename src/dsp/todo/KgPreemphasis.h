#pragma once


class KgPreemphasis
{
public:

	// @coeff: preemphasisϵ��
	KgPreemphasis(unsigned frameSize, double coeff);

	unsigned idim() const { return frameSize_; }
	unsigned odim() const { return frameSize_; }

	void porcess(double* data/*inout*/) const;

private:
	unsigned frameSize_;
	double preemphasis_;
};

