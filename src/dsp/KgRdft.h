#pragma once
#include "kDsp.h"


// 固定数据尺寸的实数fft，基于praat实现. 提供kaldi兼容选项

/*
Function:
    Calculates the Fourier Transform of a set of n real-valued data points.
    Replaces this data in array data[n] by the positive frequency half
    of its complex Fourier Transform, with a minus sign in the exponent.

Postconditions:
    data[1] contains real valued first component (Direct Current)
    data[2..n-1] even index : real part; odd index: imaginary part of DFT.
    data[n] contains real valued last component (Nyquist frequency)

Output parameters:

data  r(1) = the sum from i=1 to i=n of r(i)

    If l =(int) (n+1)/2

        then for k = 2,...,l

            r(2*k-2) = the sum from i = 1 to i = n of

                r(i)*cos((k-1)*(i-1)*2*pi/n)

            r(2*k-1) = the sum from i = 1 to i = n of

                -r(i)*sin((k-1)*(i-1)*2*pi/n)

    if n is even

        r(n) = the sum from i = 1 to i = n of

            (-1)**(i-1)*r(i)

    i.e., the ordering of the output array will be for n even
        r(1),(r(2),i(2)),(r(3),i(3)),...,(r(l-1),i(l-1)),r(l).
    Or ...., (r(l),i(l)) for n uneven.

*****  note
    this transform is unnormalized since a call of NUMfft_forward
    followed by a call of NUMfft_backward will multiply the input
    sequence by n.
*/

/**** Compatibility with NR(kaldi) fft's */

/*
Function:
    Calculates the Fourier Transform of a set of n real-valued data points.
    Replaces this data in array data [1...n] by the positive frequency half
    of its complex Fourier Transform, with a minus sign in the exponent.
Preconditions:
    n is an integer power of 2.
Postconditions:
    data [1] contains real valued first component (Direct Current)
    data [2] contains real valued last component (Nyquist frequency)
    data [3..n] odd index : real part; even index: imaginary part of DFT.
*/

class KgRdft
{
public:
    KgRdft(unsigned N, bool kaldiCompat = false);
    ~KgRdft();

    void forward(kReal data[]) const;
    void backward(kReal data[]) const;

    // converts a complex FFT (as produced by the forward functions),
    // and converts it into a power spectrum.
    // If the complex FFT is a vector of size n (representing
    // half the complex FFT of a real signal of size n, as described there),
    // this function computes in the first (n/2) + 1 elements of it, the
    // energies of the fft bins from zero to the Nyquist frequency.
    // @fft: the result of this->forward. [N]
    // @spec: the result of power spectrum, kaldi compatible. [N/2+1]
    void computePowerSpectrum(const kReal *fft/*in*/, kReal* spec/*out*/) const;


    // in-place compute. Contents of the remaining (n/2) - 1 elements are undefined at output.
    void computePowerSpectrum(kReal *fft/*inout*/) const {
        return computePowerSpectrum(fft, fft);
    }


    unsigned size() const { return size_; }

private:
    unsigned size_;
    bool kaldiCompat_; // compatible with NR fft's (the kaldi fft result)
    void* internalTable_;
};
