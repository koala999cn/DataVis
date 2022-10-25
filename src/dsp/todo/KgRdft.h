#pragma once
#include <vector>

// 实数fft，基于praat实现. 提供kaldi兼容选项

class KgRdft
{
public:

    // @nrCompat: 影响forward结果的布局，具体参见forward
    // @normalize: 不影响forward结果，只对backward结果有影响，具体参见backward
    KgRdft(unsigned sizeT, bool compatNR = false, bool normalize = false);
    ~KgRdft();

    /*
      Calculates the Fourier Transform of a set of n real - valued data points.
      Replaces this data in array data[n] by the positive frequency half
      of its complex Fourier Transform, with a minus sign in the exponent.

      data结果有两种布局：
      一是praat模式(compatNR=false)
        data[0] contains real valued first component (Direct Current)
        data[1..n-2] even index : real part; odd index: imaginary part of DFT.
        data[n-1] contains real valued last component (Nyquist frequency)
        比如: 4 samples -> cos0 cos1 sin1 cos2; 5 samples -> cos0 cos1 sin1 cos2 sin2

      二是NR模式(compatNR=true)，兼容kaldi
        data [0] contains real valued first component (Direct Current)
        data [1] contains real valued last component (Nyquist frequency)
        data [2..n-1] odd index : real part; even index: imaginary part of DFT.
    */

    // 输出结果与fftw一致，比praat放大了sampleRate倍
    void forward(double data[]) const;

    /*
      逆fft变换，输入为forward的结果
      如果normalize为false，逆变换不对结果执行除N操作，否则除N，可保证逆变换结果与原始数据尺度一致
    */
    void backward(double data[]) const;

    // 获取fft频谱第idx个频点
    // @fft: forward结果, 长度为N
    // @idx: [0, N/2+1)
    std::pair<double, double> unpack(const double* fft, unsigned idx) const;

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
    void powerSpectrum(const double fft[], double spec[]) const;


    // in-place compute. Contents of the remaining (n/2) - 1 elements are undefined at output.
    void powerSpectrum(double fft[]/*inout*/) const {
        return powerSpectrum(fft, fft);
    }

    // 输入数据的长度
    unsigned idim() const { return sizeT_; }

    // 输出频谱的长度
    unsigned odim() const { return idim() / 2 + 1; }

private:
    unsigned sizeT_;
    bool compatNR_; // compatible with NR fft's (the kaldi fft result)
    bool normalize_;
    void* internalTable_;
};
