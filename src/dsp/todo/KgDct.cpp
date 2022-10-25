#include "KgDct.h"
#include "KtuMath.h"


KgDct::KgDct(unsigned idim, unsigned odim, bool norm)
	: idim_(idim)
	, odim_(odim)
	, norm_(norm)
	, cosinTable_(nullptr)
{
	initCosinTable_();
}


KgDct::~KgDct()
{
    delete cosinTable_;
}


void KgDct::forward(const double* in, double* out) const
{
	// TODO: 使用矩阵运算
	for(unsigned k = 0; k < odim(); k++) {
		out[k] = 0;
		for(unsigned j = 0; j < idim(); j++)
			out[k] += in[j] * (*this)(k, j);
	}
}


void KgDct::backward(const double* in, double* out) const
{
	double factor = 2.0 / idim(); // TODO: 

	for(unsigned j = 0; j < idim(); j++) {
		out[j] = 0.5 * in[0] * (*this)(0, j);
		for(unsigned k = 1; k < odim(); k++)
			out[j] += in[k] * (*this)(k, j);
		out[j] *= factor;
	}
}


void KgDct::initCosinTable_()
{
	//generate cosin table
	cosinTable_ = new double[idim() * odim()]; // if dim is too big, this will crash

	for (unsigned k = 0; k < odim(); k++)
		for (unsigned j = 0; j < idim(); j++)
			(*this)(k, j) = cos(KtuMath<double>::pi * k * (j + 0.5) / idim());

	if (norm_) {
		double norm = sqrt(1.0 / idim());  // normalizer for X_0.
		for (unsigned j = 0; j < idim(); j++)
			(*this)(0, j) *= norm;  // TODO: 其实(0, j) == 1

		norm = sqrt(2.0 / idim());  // normalizer for other elements.
		for (unsigned k = 1; k < odim(); k++)
			for (unsigned j = 0; j < idim(); j++)
				(*this)(k, j) *= norm;
	}
}