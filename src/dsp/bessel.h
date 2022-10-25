#pragma once


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
