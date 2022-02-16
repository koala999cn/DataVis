#include "KgDct.h"
#include <assert.h>
#include "KtuMath.h"


KgDct::KgDct(unsigned inSize, unsigned outSize, bool htkCompat) :
    inSize_(inSize),
    outSize_(outSize),
    htkCompat_(htkCompat)
{
    if (outSize_ == 0)
        outSize_ = inSize_;

    prepareDctMatrix_();
}


void KgDct::forward(const kReal in[/*inSize*/], kReal out[/*outSize*/]) const
{
    for(unsigned k = 0; k < outSize_; k++) {
        out[k] = 0;
        for(unsigned j = 0; j < inSize_; j++)
            out[k] += in[j] * cosTable_[k][j];
    }
}

void KgDct::backward(const kReal in[/*inSize*/], kReal out[/*outSize*/]) const
{
    if(!htkCompat_) {
        kReal factor = 2.0 / inSize_;
        for(unsigned j = 0; j < inSize_; j++) {
            out[j] = in[0] * cosTable_[0][j] / 2;
            for(unsigned k = 1; k < outSize_; k++)
                out[j] += in[k] * cosTable_[k][j];
            out[j] *= factor;
        }
    }
    else {
        for (unsigned j = 0; j < inSize_; j++) {
            out[j] = 0;
            for (unsigned k = 0; k < outSize_; k++)
                out[j] += in[k] * cosTable_[k][j];
        }
    }
}

void KgDct::prepareDctMatrix_()
{
    assert(inSize_ > 0 && outSize_ > 0);

    //generate cosin table
    cosTable_.resize(outSize_, std::vector<kReal>(inSize_));

    for (unsigned k = 0; k < outSize_; k++)
        for (unsigned j = 0; j < inSize_; j++)
            cosTable_[k][j] = std::cos(KtuMath<kReal>::pi * k * (j + 0.5) / inSize_);


    if(htkCompat_) { // normalize cos-table
        kReal normalizer = std::sqrt(static_cast<kReal>(1) / inSize_);  // normalizer for X_0.
        for (unsigned j = 0; j < inSize_; j++)
            cosTable_[0][j] *= normalizer;

        normalizer *= std::sqrt(static_cast<kReal>(2));  // normalizer for other elements.
        for (unsigned k = 1; k < outSize_; k++)
            for (unsigned j = 0; j < inSize_; j++)
                cosTable_[k][j] *= normalizer;
    }
}
