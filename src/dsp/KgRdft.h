#pragma once
#include "kDsp.h"


// 实数fft，基于praat实现. 提供kaldi兼容选项

class KgRdft
{
public:
    KgRdft(unsigned sizeT, bool kaldiCompat = false, bool normalize = false);
    ~KgRdft();

    /*
      Calculates the Fourier Transform of a set of n real - valued data points.
      Replaces this data in array data[n] by the positive frequency half
      of its complex Fourier Transform, with a minus sign in the exponent.

      data结果有两种布局：
      一是praat模式(kaldiCompat=false)
        data[0] contains real valued first component (Direct Current)
        data[1..n-2] even index : real part; odd index: imaginary part of DFT.
        data[n-1] contains real valued last component (Nyquist frequency)

      二是kalid模式(kaldiCompat=true)
        data [0] contains real valued first component (Direct Current)
        data [1] contains real valued last component (Nyquist frequency)
        data [2..n-1] odd index : real part; even index: imaginary part of DFT.
    */
    void forward(kReal data[]) const;

    /*
      逆fft变换，输入为forward的结果
      如果normalize为false，逆变换不对结果执行除N操作，否则除N，可保证逆变换结果与原始数据尺度一致
    */
    void backward(kReal data[]) const;

    // converts a complex FFT (as produced by the forward functions),
    // and converts it into a power spectrum.
    // If the complex FFT is a vector of size n (representing
    // half the complex FFT of a real signal of size n, as described there),
    // this function computes in the first (n/2) + 1 elements of it, the
    // energies of the fft bins from zero to the Nyquist frequency.
    // @fft: fft result produced by the forward function. [N]
    // @spec: caculated power spectrum. [N/2+1]
    // NOTE: 如果normalize为false，结果为各复频率分量的模平方，spec[i] = A = a^2 + b^2
    //       如果normalize为true，结果为真实的频率谱，sepc[0] = sqrt(A)/N, 其他spec[i] = 2*sqrt(A)/N
    void powerSpectrum(const kReal fft[], kReal spec[]) const;


    // in-place compute. Contents of the remaining (n/2) - 1 elements are undefined at output.
    void powerSpectrum(kReal fft[]/*inout*/) const {
        return powerSpectrum(fft, fft);
    }


    unsigned sizeT() const { return sizeT_; }

    unsigned sizeF() const { return sizeT() / 2 + 1; }

private:
    unsigned sizeT_;
    bool kaldiCompat_; // compatible with NR fft's (the kaldi fft result)
    bool normalize_;
    void* internalTable_;
};
