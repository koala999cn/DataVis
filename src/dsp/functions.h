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
        KREAL t = KtuMath<KREAL>::pi * 2 * x;
        return 0.42 - 0.5 * cos(t) + 0.08 * cos(2 * t);
    }
};

// Blackman-harris window [harris:1978]
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
/*		KREAL a0 = 1.000f;
        KREAL a1 = 1.930f;
        KREAL a2 = 1.290f;
        KREAL a3 = 0.388f;
        KREAL a4 = 0.028f;*/
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


template<typename KREAL>
struct besseli0
{
    // [-inf, +inf]
    KREAL operator()(KREAL x) const {
        KREAL t;
        if (x < 0.0) return operator()(-x);
        if (x < 3.75)
        {
            /* Formula 9.8.1. Accuracy 1.6e-7. */
            t = x / 3.75;
            t *= t;
            return 1.0 + t * (3.5156229 + t * (3.0899424 + t * (1.2067492
                + t * (0.2659732 + t * (0.0360768 + t * 0.0045813)))));
        }
        /*
        otherwise: x >= 3.75
        */
        /* Formula 9.8.2. Accuracy of the polynomial factor 1.9e-7. */
        t = 3.75 / x;   /* <= 1.0 */
        return exp(x) / sqrt(x) * (0.39894228 + t * (0.01328592
            + t * (0.00225319 + t * (-0.00157565 + t * (0.00916281
                + t * (-0.02057706 + t * (0.02635537 + t * (-0.01647633
                    + t * 0.00392377))))))));
    }
};


template<typename KREAL>
struct besseli1
{
    // [-inf, +inf]
    KREAL operator()(KREAL x) const {
        KREAL t;
        if (x < 0.0) return -operator()(-x);
        if (x < 3.75)
        {
            /* Formula 9.8.3. Accuracy of the polynomial factor 8e-9. */
            t = x / 3.75;
            t *= t;
            return x * (0.5 + t * (0.87890594 + t * (0.51498869 + t * (0.15084934
                + t * (0.02658733 + t * (0.00301532 + t * 0.00032411))))));
        }
        /*
        otherwise: x >= 3.75
        */
        /* Formula 9.8.4. Accuracy of the polynomial factor 2.2e-7. */
        t = 3.75 / x;   /* <= 1.0 */
        return exp(x) / sqrt(x) * (0.39894228 + t * (-0.03988024
            + t * (-0.00362018 + t * (0.00163801 + t * (-0.01031555
                + t * (0.02282967 + t * (-0.02895312 + t * (0.01787654
                    + t * (-0.00420059)))))))));
    }
};


template<typename KREAL>
struct besselj0
{
    // [-inf, +inf]
    KREAL operator()(KREAL x) const {
        const unsigned NUM_BESSELJ0_ITERATIONS = 16;

        // large signal approximation, see
        // Gross, F. B "New Approximations to J0 and J1 Bessel Functions,"
        //   IEEE Trans. on Antennas and Propagation, vol. 43, no. 8,
        //   August, 1995
        if (abs(x) > 10.0f)
            return sqrt(2 / (KtuMath<KREAL>::pi * abs(x))) * cos(abs(x) - KtuMath<KREAL>::pi / 4);

        KREAL t, y = 0.0f;
        for (unsigned k = 0; k < NUM_BESSELJ0_ITERATIONS; k++) {
            t = pow(x / 2, (KREAL)k) / tgamma((KREAL)k + 1);
            y += (k % 2) ? -t * t : t * t;
        }

        return y;
    }
};


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