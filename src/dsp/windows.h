#pragma once
#include "KtuMath.h"


// 一些常用的数学函数集合

// Hamming window [Nuttall:1981]
template<typename KREAL>
struct hamming
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        return 0.53836 - 0.46164 * cos(KtuMath<KREAL>::pi * 2 * x);
    }
};

// Hann window
template<typename KREAL>
struct hann
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        return 0.5 - 0.5 * cos(KtuMath<KREAL>::pi * 2 * x);
    }
};

// Povey window
template<typename KREAL>
struct povey
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        return pow((0.5 - 0.5 * cos(KtuMath<KREAL>::pi * 2 * x)), 0.85);
    }
};

template<typename KREAL>
struct blackman
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        KREAL alpha = 0.16;
        KREAL a0 = 0.5 * (1- alpha);
        KREAL a1 = 0.5;
        KREAL a2 = 0.5 * alpha;

        KREAL t = KtuMath<KREAL>::pi * 2 * x;
        return a0 - a1 * cos(t) + a2 * cos(2 * t);
    }
};

// Blackman-harris window
// see "On the Use of Windows for Harmonic Analysis with the Discrete Fourier Transform." harris, fredric j. 1978.
template<typename KREAL>
struct blackmanharris
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        KREAL a0 = 0.35875f;
        KREAL a1 = 0.48829f;
        KREAL a2 = 0.14128f;
        KREAL a3 = 0.01168f;
        KREAL t = KtuMath<KREAL>::pi * 2 * x;

        return a0 - a1 * cos(t) + a2 * cos(2 * t) - a3 * cos(3 * t);
    }
};


// 一个不同系数版本的blackmanharris，具有稍低的旁瓣
// see "Some Windows with Very Good Sidelobe Behavior." Nuttall, Albert H. 1981.
template<typename KREAL>
struct nuttall
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        KREAL a0 = 0.3635819;
        KREAL a1 = 0.4891775;
        KREAL a2 = 0.1365995;
        KREAL a3 = 0.0106411;
        KREAL t = KtuMath<KREAL>::pi * 2 * x;

        return a0 - a1 * cos(t) + a2 * cos(2 * t) - a3 * cos(3 * t);
    }
};

// 7th-order Blackman-harris window
template<typename KREAL>
struct blackmanharris7
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        KREAL a0 = 0.27105f;
        KREAL a1 = 0.43329f;
        KREAL a2 = 0.21812f;
        KREAL a3 = 0.06592f;
        KREAL a4 = 0.01081f;
        KREAL a5 = 0.00077f;
        KREAL a6 = 0.00001f;
        KREAL t = KtuMath<KREAL>::pi * 2 * x;

        return a0 - a1 * cos(t) + a2 * cos(2 * t) - a3 * cos(3 * t)
            + a4 * cos(4 * t) - a5 * cos(5 * t) + a6 * cos(6 * t);
    }
};


// Flat-top window
template<typename KREAL>
struct flattop
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        const KREAL a0 = 0.21557895;
        const KREAL a1 = 0.41663158;
        const KREAL a2 = 0.277263158;
        const KREAL a3 = 0.083578947;
        const KREAL a4 = 0.006947368;

        KREAL t = KtuMath<KREAL>::pi * 2 * x;

        return a0 - a1 * cos(t) + a2 * cos(2 * t) -
            a3 * cos(3 * t) + a4 * cos(4 * t);
    }
};


// Bohman window
// f(x) = (1−|x|)*cos(pi*|x|) + sin(pi*|x|)/pi, , −1 <= x <= 1
template<typename KREAL>
struct bohman
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        x = abs(2 * x - 1); // 从[0, 1]转换到[-1, 1]
        auto pix = KtuMath<KREAL>::pi * x;
        return (1 - x) * cos(pix) + sin(pix) / KtuMath<KREAL>::pi;
    }
};


// Gaussian window
template<typename KREAL>
struct gaussian
{
    // @aplha: 1 / covar
    gaussian(KREAL alpha = 2.5) : alpha_(alpha) {}

    // [0, 1]
    KREAL operator()(KREAL x) const {
        x = 2 * x - 1; // 从[0, 1]转换到[-1, 1]
        return exp(-0.5 * (alpha_ * x) * (alpha_ * x));
    }

private:
    KREAL alpha_;
};


// Parzen window
template<typename KREAL>
struct parzen
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        x = abs(2 * x - 1); // 从[0, 1]转换到[-1, 1]
        return x <= 0.5 ? 1 - 6 * x * (x - x * x) : 2 * (1 - x) * (1 - x) * (1 - x);
    }
};


// Triangular window
template<typename KREAL>
struct triangular
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        return 1 - abs(2 * x - 1);
    }
};


// raised-cosine tapering window
template<typename KREAL>
struct rcostaper
{
    rcostaper(KREAL alpha) : alpha_(alpha) {}

    // [0, 1]
    KREAL operator()(KREAL x) const {
        // reverse time for ramp-down section
        if (x > 1 - alpha_)
            x = 1 - x;

        // return ramp or flat component
        return (x < alpha_) ? 0.5f - 0.5f * cos(KtuMath<KREAL>::pi * (x + 0.5f) / alpha_) : 1.0f; // TODO: test (x+0.5f) ????
        //  return (_i < _t) ? 0.5f - 0.5f*cosf(KtuMath<KREAL>::pi*((float)_i + 0.5f) / (float)_t) : 1.0f;
    }

private:
    KREAL alpha_; // taper length, in [0, 0.5]
                  // alpha_ = 0， 则rcostaper相当于矩形波
                  // alpha_ = 0.5， 则rcostaper相当于余弦波
};


#include "bessel.h"
// Kaiser window [Kaiser:1980]
template<typename KREAL>
struct kaiser
{
    kaiser(KREAL beta) : beta_(beta) {}

    // [0, 1]
    KREAL operator()(KREAL x) const {
        besseli0<KREAL> bessel_;

        KREAL r = 2 * x - 1; // 将x从[0, 1]映射到[-1, 1]
        KREAL a = bessel_(beta_ * sqrt(1 - r * r));
        KREAL b = bessel_(beta_);
        return a / b;
    }

private:
    KREAL beta_; // window taper parameter
};


// bartlett_hann window
template<typename KREAL>
struct bartletthann
{
    // [0, 1]
    KREAL operator()(KREAL x) const {
        x = x - 0.5;
        return 0.62 - 0.48 * abs(x) + 0.38 * cos(2 * KtuMath<KREAL>::pi * x);
    }
};
