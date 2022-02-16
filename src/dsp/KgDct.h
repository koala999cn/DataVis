#pragma once
#include "kDsp.h"
#include <vector>


// 固定数据尺寸的离散余弦变换
class KgDct
{
public:
    KgDct(unsigned inSize, unsigned outSize = 0, bool htkCompat = false);

    void forward(const kReal in[/*inSize*/], kReal out[/*outSize*/]) const;
    void backward(const kReal in[/*inSize*/], kReal out[/*outSize*/]) const;

    unsigned inSize() const { return inSize_; }
    unsigned outSize() const { return outSize_; }

protected:
    unsigned inSize_, outSize_;
    bool htkCompat_;
    std::vector<std::vector<kReal>> cosTable_; // [outSize][inSize]

private:
    /// prepareDctMatrix_ computes a matrix corresponding to the DCT, such that
    /// M * v equals the DCT of vector v.  M must be square at input.
    /// This is the type = III DCT with normalization, corresponding to the
    /// following equations, where x is the signal and X is the DCT:
    /// X_0 = 1/sqrt(2*N) \sum_{n = 0}^{N-1} x_n
    /// X_k = 1/sqrt(N) \sum_{n = 0}^{N-1} x_n cos( \pi/N (n + 1/2) k )
    /// This matrix's transpose is its own inverse, so transposing this
    /// matrix will give the inverse DCT.
    /// Caution: the type III DCT is generally known as the "inverse DCT" (with the
    /// type II being the actual DCT), so this function is somewhatd mis-named.  It
    /// was probably done this way for HTK compatibility.  We don't change it
    /// because it was this way from the start and changing it would affect the
    /// feature generation.
    void prepareDctMatrix_();
};

